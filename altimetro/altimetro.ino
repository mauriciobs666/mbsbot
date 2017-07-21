
// (c) 2017 MBS - Mauricio Bieze Stefani

#include <math.h>

#include <Wire.h>

#define SERIALX Serial

#include "circular.hpp"

#ifdef MPL3115A2
#include <SparkFunMPL3115A2.h>
MPL3115A2 altimetro;
#endif

#define BMP180

#ifdef BMP180
#include <SFE_BMP180.h>
SFE_BMP180 barometro;
#endif

#include <toneAC.h>

//#define DEBUG 1

#define TRACE_OFF          0x00
#define TRACE_ARDUINO_PLOT 0x01
#define TRACE_AVISOS       0x02

#ifndef DEBUG

// PRODUCAO !

#define THRESHOLD_DECOLAGEM   6.0 // pes/s
#define THRESHOLD_QUEDA    -150.0 // pes/s
#define THRESHOLD_ABERTURA  -60.0 // pes/s

#define AVISO_SUBIDA_CINTO  1500
#define AVISO_SUBIDA_CHECK 11500

#define AVISO_ALTA_1    6500
#define AVISO_ALTA_2    5500
#define AVISO_ALTA_3    4500
#define ALARME_ALTA     3500

#define AVISO_NAVEGACAO_A      300
#define AVISO_NAVEGACAO_B      600
#define AVISO_NAVEGACAO_C      900
#define AVISO_NAVEGACAO_D     1200

#define TRACE          TRACE_OFF

#else

// DEBUG

#define NUM_VARS 10
#define TAM_TOKEN 10
#define TAM_NOME 5

#include "interpretador.hpp"

#define THRESHOLD_DECOLAGEM  0.8
#define THRESHOLD_QUEDA     -0.8
#define THRESHOLD_ABERTURA   0.8

#define AVISO_SUBIDA_CINTO  6
#define AVISO_SUBIDA_CHECK 12

#define AVISO_ALTA_1       9
#define AVISO_ALTA_2       6
#define AVISO_ALTA_3       3
#define ALARME_ALTA        5

#define AVISO_NAVEGACAO_A        6
#define AVISO_NAVEGACAO_B       12
#define AVISO_NAVEGACAO_C       24
#define AVISO_NAVEGACAO_D       36

#define TRACE           TRACE_ARDUINO_PLOT

#endif

#define ESTADO_DZ           0x00
#define ESTADO_SUBIDA       0x01
#define ESTADO_QUEDA        0x02
#define ESTADO_NAVEGACAO    0x04

class Ponto
{
public:
    Ponto() : altitude(0),
              velocidade(0),
              timestamp(0) {}

    unsigned long timestamp;
    double altitude;
    double velocidade;

    void limpa()
    {
        timestamp = 0;
        altitude = velocidade = 0;
    }
};

class Evento
{
public:

};

class Salto
{
public:
    Salto() : estado( ESTADO_DZ ) {}

    char estado;

    Ponto decolagem;    // area de pouso, DZ
    Ponto saida;
    Ponto abertura;
    Ponto pouso;

    void trocaEstado( char novoEstado )
    {
        estado = novoEstado;
    }

    // TODO: array velocidade altitude
    void log()
    {

    }

    void limpa()
    {
        estado = ESTADO_DZ;

        decolagem.limpa();
        saida.limpa();
        abertura.limpa();
        pouso.limpa();
    }
};

typedef struct
{
    int altura;

    char estado;

    enum TipoAviso
    {
        BIP_UNICO = 0,
        BIP_DUPLO,
        BIP_TRIPLO,
        SIRENE_UNICO,
        SIRENE_DUPLO,
        SIRENE_TRIPLO,
        SIRENE
    } tipo;

    char volume;

    unsigned long atingido;

    void print()
    {
        SERIALX.print("h:");
        SERIALX.print( altura );
        SERIALX.print(" stg:");
        SERIALX.print( (int)estado );
        SERIALX.print(" t:");
        SERIALX.print( (int)tipo );
        SERIALX.print(" v:");
        SERIALX.print( (int)volume );
        SERIALX.print(" qdo:");
        SERIALX.print( atingido );
    }
}
Aviso;

Aviso avisos[] =
{
    { AVISO_SUBIDA_CINTO, ESTADO_SUBIDA, Aviso::BIP_UNICO, 10, 0 },
    { AVISO_SUBIDA_CHECK, ESTADO_SUBIDA, Aviso::BIP_DUPLO, 10, 0 },
    { AVISO_ALTA_1, ESTADO_QUEDA, Aviso::SIRENE_UNICO, 10, 0 },
    { AVISO_ALTA_2, ESTADO_QUEDA, Aviso::SIRENE_DUPLO, 10, 0 },
    { AVISO_ALTA_3, ESTADO_QUEDA, Aviso::SIRENE_TRIPLO, 10, 0 },
    { ALARME_ALTA, ESTADO_QUEDA, Aviso::SIRENE, 10, 0 },
    { AVISO_NAVEGACAO_A, ESTADO_NAVEGACAO, Aviso::BIP_TRIPLO, 10, 0 },
    { AVISO_NAVEGACAO_B, ESTADO_NAVEGACAO, Aviso::BIP_DUPLO, 10, 0 },
    { AVISO_NAVEGACAO_C, ESTADO_NAVEGACAO, Aviso::BIP_UNICO, 10, 0 },
    { AVISO_NAVEGACAO_D, ESTADO_NAVEGACAO, Aviso::BIP_UNICO, 10, 0 }
};

const unsigned int nAvisos = ( sizeof(avisos)/sizeof(avisos[0]) );

class TocadorToneAC
{
    public:
        void limpa()
        {
            leitura = escrita = 0;
            tocando = 0;
        }

        TocadorToneAC()
        {
            limpa();
        }

        void loop( unsigned long agora )
        {
            static bool mudoQdoTerminar = false;

            if( agora > tocando )
            {
                if( leitura != escrita )
                {
                    toneAC( notas[leitura].frequencia,
                            notas[leitura].volume,
                            notas[leitura].duracao,
                            true );

                    tocando = agora + notas[leitura].duracao;

                    incrementa( leitura );

                    mudoQdoTerminar = true;
                }
                else
                {
                    if( mudoQdoTerminar )
                    {
                        noToneAC();
                        mudoQdoTerminar = false;
                    }

                    tocando = agora;
                }
            }
        }

        void insere( int duracao, int frequencia = 0, int volume = 0 )
        {
            notas[escrita].duracao = duracao;
            notas[escrita].frequencia = frequencia;
            notas[escrita].volume = volume;

            incrementa(escrita);

            if( escrita == leitura )
                incrementa( leitura );
        }

        void bipe( int nbips = 1, int vol = 10, int freq = 2700, int dur = 250, int inter = 250 )
        {
//            SERIALX.print("bipe(");
//            SERIALX.print( nbips );
//            SERIALX.print( ", " );
//            SERIALX.print( vol );
//            SERIALX.print( ", " );
//            SERIALX.print( freq );
//            SERIALX.println( ")" );

            for( int n = 0; n < nbips; n++ )
            {
                insere( dur, freq, vol );
                insere( inter );
            }
        }

        void insere( Aviso & aviso )
        {
//            SERIALX.print("insere(");
//            aviso.print();
//            SERIALX.println(")");
            int repeticoes = 1;

            switch( aviso.tipo )
            {
                case Aviso::BIP_TRIPLO:
                    repeticoes++;
                case Aviso::BIP_DUPLO: //fall trough
                    repeticoes++;
                case Aviso::BIP_UNICO: //fall trough
                    bipe( repeticoes, aviso.volume );
                    break;

                case Aviso::SIRENE:
                    repeticoes+=7;
                case Aviso::SIRENE_TRIPLO: //fall trough
                    repeticoes++;
                case Aviso::SIRENE_DUPLO:  //fall trough
                    repeticoes++;
                case Aviso::SIRENE_UNICO:  //fall trough
                    for( int n = 0; n < repeticoes; n++ )
                    {
                        insere( 150, 2700, aviso.volume );
                        insere( 150, 3000, aviso.volume );
                    }
                    break;
            }
        }

    private:
        int leitura;
        int escrita;
        unsigned long tocando;

        #define SZ_NOTAS 30
        struct Nota
        {
            int frequencia;
            int duracao;
            char volume;
        } notas[SZ_NOTAS];

        int proximo( int ponteiro )
        {
            ponteiro++;

            if( ponteiro == SZ_NOTAS )
                ponteiro = 0;

            return ponteiro;
        }

        int incrementa( int & ponteiro )
        {
            return ( ponteiro = proximo( ponteiro ) );
        }
};

char trace = TRACE;

CircularStats<double,30> circular1s;        // ultimas 30 leituras ( ~1 segundo )
CircularStats<double,10> circular10;        // medias dos ultimos 10 segundos
CircularStats<double,10> circular10media;   // delay line das medias dos ultimos 10 segundos

TocadorToneAC tocadorToneAC;

Salto salto;

unsigned long tUmSegundo = 0;
unsigned long tUmDecimo = 0;
unsigned long debounceAbertura = 0;
int contadorLoop = 0;
int loopsSeg = 0;

Ponto antes, agora;

#define MAX_CMD 50
#define CMD_EOL '\n'
char comando[MAX_CMD];
unsigned char pos = 0;

double readAltitudeFtBmp()
{
    double temperatura = 0;

    char atraso = barometro.startTemperature();

    if( atraso > 0 )
    {
        delay( atraso );

        barometro.getTemperature( temperatura );

        atraso = barometro.startPressure( 3 ); // oversampling 0-3

        if( atraso > 0 )
        {
            delay( atraso );

            double pressao;

            atraso = barometro.getPressure( pressao, temperatura );

    //        pressao -= 2.9; // offset erro sensor

            if( atraso != 0 )
            {
                delay( atraso );
                return( barometro.altitude( pressao, 1013.25 ) * 3.28084 ); // MSL Mean Sea Level
            }
        }
    }

    return -666;
}

void setup()
{
    toneAC( 3200, 5, 100 );

    SERIALX.begin( 115200 );

    if( ! barometro.begin() )
    {
        Serial.println("Erro inicializando modulo BMP180\n");
        toneAC( 4000, 10, 200 );
        delay(200);
        toneAC( 4000, 10, 200 );
        return;
        while(1);
    }

#ifdef MPL3115A2
    Wire.begin();
    altimetro.begin();
    altimetro.setModeAltimeter();
    altimetro.setOversampleRate(7);
    altimetro.enableEventFlags();
#endif

    for( int z = 0; z < 100; z++ )
    {
        double alti = readAltitudeFtBmp();

        circular1s.insere( alti );
        circular10.insere( circular1s.media() );
        circular10media.insere( circular10.media() );
    }

    salto.saida.altitude = salto.decolagem.altitude = salto.abertura.altitude = agora.altitude = antes.altitude = circular10.media();

    tocadorToneAC.insere( 250, 2700, 10 );

    antes.timestamp = millis();
}

void loop()
{
    bool traceAgora = false;

    // atualiza sensores

    #ifdef BMP180
    double sensor = readAltitudeFtBmp();
    #endif

    #ifdef MPL3115A2
    agora.sensor = altimetro.readAltitudeFt();
    #endif

    agora.timestamp = millis();
    double deltaT = ( (double) agora.timestamp - antes.timestamp ) / 1000.0;

    // predicao

    agora.altitude = antes.altitude + antes.velocidade * deltaT;
    agora.velocidade = antes.velocidade;

    double desvio = sensor - agora.altitude;

    double alpha = 0.1;
    double beta = 0.001;

    agora.altitude += alpha * desvio;
    agora.velocidade += ( beta * desvio ) / deltaT;

//    // Kalman
//
//    static double ganho= 0;
//    static double p = 1;
//    ganho = p + 0.05 / (p + variancia);
//    agora.altitude = antes.altitude + ganho * ( sensor - antes.altitude );
//    p = ( 1 - ganho ) * p;
//    circular1s.insere( agora.altitude );

    circular1s.insere( sensor );

    contadorLoop++;

    if( agora.timestamp >= tUmSegundo )
    {
        tUmSegundo = agora.timestamp + 1000;

        circular10.insere( circular1s.media() );

        circular10media.insere( circular10.media() );

        loopsSeg = contadorLoop;
        contadorLoop = 0;

        traceAgora = (bool)trace;
    }

    if( agora.timestamp >= tUmDecimo )
    {
        tUmDecimo = agora.timestamp + 100;

        #ifdef DEBUG

        // TODO leitura analogicas

        #endif
    }

    // regras de negocio

    switch( salto.estado )
    {
    case ESTADO_DZ:
        salto.decolagem.altitude = *circular10media.topo();

        if( agora.velocidade > THRESHOLD_DECOLAGEM )
        {
            salto.decolagem.timestamp = agora.timestamp;
            salto.decolagem.velocidade = agora.velocidade;

            // reset alarmes
            for( int i = 0; i < nAvisos ; i++ )
                avisos[i].atingido = 0;

            tocadorToneAC.bipe( );

            salto.estado = ESTADO_SUBIDA;
        }
        break;

    case ESTADO_SUBIDA:
        if( agora.velocidade < THRESHOLD_QUEDA ) // inicio da queda livre
        {
            salto.saida.altitude = *circular10media.topo();
            salto.saida.timestamp = agora.timestamp;

            tocadorToneAC.limpa();
            tocadorToneAC.bipe( 2 );

            salto.estado = ESTADO_QUEDA;
        }
        else if( agora.velocidade < -2 ) // abertura subterminal
        {
            if( debounceAbertura && debounceAbertura < agora.timestamp )
            {
                salto.abertura.altitude = circular1s.media();
                salto.abertura.timestamp = agora.timestamp;

                tocadorToneAC.limpa();
                tocadorToneAC.bipe( 3 );

                salto.estado = ESTADO_NAVEGACAO;
            }
        }
        else // subindo ou estavel
        {
            debounceAbertura = agora.timestamp + 10000;
        }
        break;

    case ESTADO_QUEDA:
        if( agora.velocidade > THRESHOLD_ABERTURA )
        {
            salto.abertura.altitude = circular1s.media();
            salto.abertura.timestamp = agora.timestamp;

            tocadorToneAC.limpa();
            tocadorToneAC.bipe( 3 );

            salto.estado = ESTADO_NAVEGACAO;
        }
        break;

    case ESTADO_NAVEGACAO:
        if( agora.velocidade < THRESHOLD_QUEDA )
        {
            tocadorToneAC.limpa();
            tocadorToneAC.bipe( 2 );

            salto.estado = ESTADO_QUEDA;
        }
        break;
    }

    // avisos de altura

    int altura = int( agora.altitude - salto.decolagem.altitude );

    for( int i = 0;  i < nAvisos; i++ )
    {
        if( 0 == avisos[i].atingido )
        {
            if( avisos[i].estado & salto.estado )
            {
                if( ( ( altura > avisos[i].altura ) && ( avisos[i].estado & (ESTADO_SUBIDA) ) )
                 || ( ( altura < avisos[i].altura ) && ( avisos[i].estado & (ESTADO_NAVEGACAO|ESTADO_QUEDA) ) ) )
                {
                    avisos[i].atingido = agora.timestamp;

                    tocadorToneAC.insere( avisos[i] );
                }
            }
        }
    }

    // player

    tocadorToneAC.loop( agora.timestamp );

    // interpretador de comandos via serial
/*
    while( SERIALX.available() > 0 )
    {
        char c = SERIALX.read();

        if ( pos == MAX_CMD )
        {
            pos = 0;
            //printErro( ERRO_TAM_MAX_CMD );
        }
        else if( c == CMD_EOL )
        {
            comando[ pos ] = 0;
            pos = 0;
            //return true;
        }
        else
            comando[ pos++ ] = c;
    }
*/

    if( traceAgora && trace & TRACE_AVISOS )
    {
         for( int i = 0;  i < nAvisos; i++ )
         {
            SERIALX.print("[");
            SERIALX.print(i);
            SERIALX.print("]");
            avisos[i].print();
            SERIALX.println();
         }
    }

    if( ( traceAgora && trace & TRACE_ARDUINO_PLOT ) || ( trace == TRACE_ARDUINO_PLOT ) )
    {
//        Serial.print( deltaT * 1000 );
//        Serial.print( " " );
        Serial.print( altura );
//        Serial.print( agora.altitude, 2 );
//        Serial.print( agora.altitude - salto.decolagem.altitude, 2 );
//        Serial.print( " " );
//        Serial.print( circular10.media() , 2  );
        //Serial.print( circular10.media() - decolagem.altitude, 2  );
        Serial.print( " " );
//        Serial.print( agora.velocidade, 2  );
        Serial.print( sensor - salto.decolagem.altitude, 2  );
        Serial.print( " " );
        Serial.print( *circular10media.topo() - salto.decolagem.altitude, 2 );
        Serial.print( " " );
//        Serial.print( salto.saida.altitude - salto.decolagem.altitude, 2 );
//        Serial.print( " " );
//        Serial.print( circular1s.media() , 2 );
//        Serial.print( " " );
//        Serial.print( sensor, 2  );
        Serial.print( circular1s.media() - salto.decolagem.altitude, 2 );
//        Serial.print( " " );
//        Serial.print( circular1s.desvio(), 2 );
//        Serial.print( " " );
        Serial.println();
    }

    antes = agora;
}
