
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

#ifndef DEBUG

// PRODUCAO !

#define THRESHOLD_QUEDA    -100.0 // pes/s
#define THRESHOLD_ABERTURA  -60.0 // pes/s

#define AVISO_SUBIDA_CHECK 11000

#define AVISO_ALTA_1    6000
#define AVISO_ALTA_2    5000
#define AVISO_ALTA_3    4000
#define ALARME_ALTA     2500

#define AVISO_NAVEGACAO_A      300
#define AVISO_NAVEGACAO_B      600
#define AVISO_NAVEGACAO_C      900
#define AVISO_NAVEGACAO_D     1200

#define TRACE          false

#else

// DEBUG

#define NUM_VARS 10
#define TAM_TOKEN 10
#define TAM_NOME 5

#include "interpretador.hpp"

#define THRESHOLD_DECOLAGEM  0.8
#define THRESHOLD_QUEDA     -0.8
#define THRESHOLD_ABERTURA   0.8

#define AVISO_SUBIDA_CINTO 12
#define AVISO_SUBIDA_CHECK 24

#define AVISO_ALTA_1      20
#define AVISO_ALTA_2      15
#define AVISO_ALTA_3      10
#define ALARME_ALTA        5

#define AVISO_NAVEGACAO_A        6
#define AVISO_NAVEGACAO_B       12
#define AVISO_NAVEGACAO_C       24
#define AVISO_NAVEGACAO_D       36

#define TRACE           true

#endif

#define ESTAGIO_SOLO        0x00
#define ESTAGIO_SUBIDA      0x01
#define ESTAGIO_QUEDA       0x02
#define ESTAGIO_NAVEGACAO   0x04

//typedef struct
//{
//    unsigned long clock;
//    double altura;
//}
//Ponto;

typedef struct
{
    double altitudeDZ;  // area de pouso
    double altitudePS;  // ponto de saida
    double altitudeCMD; // abertura pqd

    unsigned long clockDecolagem;
    unsigned long clockPS;
    unsigned long clockAbertura;

    void limpa()
    {
        altitudeDZ = altitudePS = 0;
        clockDecolagem = clockPS = clockAbertura = 0;
    }
}
Salto;

class Estado
{
public:
    Estado() :  sensor(0),
                altitude(0),
                velocidade(0),
                relogio(0),
                estagio( ESTAGIO_SOLO ) {}

    double sensor;
    double altitude;
    double velocidade;
    unsigned long relogio;
    char estagio;
};

typedef struct
{
    int altura;

    char estagio;

    enum TipoAviso
    {
        BIP_UNICO = 0,
        BIP_DUPLO,
        BIP_TRIPLO,
        SIRENE
    } tipo;

    char volume;

    unsigned long atingido;
}
Aviso;

Aviso avisos[] =
{
    { AVISO_SUBIDA_CINTO, ESTAGIO_SUBIDA, Aviso::BIP_UNICO, 10, 0 },
    { AVISO_SUBIDA_CHECK, ESTAGIO_SUBIDA, Aviso::BIP_DUPLO, 10, 0 },
    { AVISO_ALTA_1, ESTAGIO_QUEDA, Aviso::BIP_UNICO, 10, 0 },
    { AVISO_ALTA_2, ESTAGIO_QUEDA, Aviso::BIP_DUPLO, 10, 0 },
    { AVISO_ALTA_3, ESTAGIO_QUEDA, Aviso::BIP_TRIPLO, 10, 0 },
    { ALARME_ALTA, ESTAGIO_QUEDA, Aviso::SIRENE, 10, 0 },
    { AVISO_NAVEGACAO_A, ESTAGIO_NAVEGACAO, Aviso::BIP_TRIPLO, 10, 0 },
    { AVISO_NAVEGACAO_B, ESTAGIO_NAVEGACAO, Aviso::BIP_DUPLO, 10, 0 },
    { AVISO_NAVEGACAO_C, ESTAGIO_NAVEGACAO, Aviso::BIP_UNICO, 10, 0 },
    { AVISO_NAVEGACAO_D, ESTAGIO_NAVEGACAO, Aviso::BIP_UNICO, 10, 0 }
};

const unsigned int nAvisos = ( sizeof(avisos)/sizeof(avisos[0]) );

class TonePlayer
{
    public:
        void limpa()
        {
            leitura = escrita = 0;
            tocando = 0;
        }

        TonePlayer()
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
            for( int n = 0; n < nbips; n++ )
            {
                insere( dur, freq, vol );
                insere( inter );
            }
        }

        void insere( Aviso & aviso )
        {
            switch( aviso.tipo )
            {
                BIP_UNICO:
                    bipe( 1, aviso.volume );
                    break;
                BIP_DUPLO:
                    bipe( 2, aviso.volume );
                    break;
                BIP_TRIPLO:
                    bipe( 3, aviso.volume );
                    break;
                SIRENE:
                    for( int n = 0; n < 3; n++ )
                    {
                        insere( 250, 5000, aviso.volume );
                        insere( 250, 1000, aviso.volume );
                    }
                    break;
            }
        }

    private:
        int leitura;
        int escrita;
        unsigned long tocando;

        #define SZ_NOTAS 10
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

bool trace = TRACE;

CircularStats<double,50> circular1s;        // ultimas 50 leituras ( ~1 segundo )
CircularStats<double,10> circular10;        // medias dos ultimos 10 segundos
CircularStats<double,10> circular10media;   // delay line das medias dos ultimos 10 segundos

TonePlayer tonePlayer;

Salto salto;

unsigned long tUmSegundo = 0;
unsigned long tUmDecimo = 0;

Estado antes, agora;

#define MAX_CMD 50
#define CMD_EOL '\n'
char comando[MAX_CMD];
unsigned char pos = 0;

double readAltitudeFtBmp( int delayTemp = 0 )
{
    static double temperatura = 0;
    static unsigned long proximaLeituraT = 0;

    char atraso = 0;

    // refresh temperatura apenas a cada segundo

    if( ! delayTemp || millis() > proximaLeituraT )
    {
        atraso = barometro.startTemperature();

        if( atraso > 0 )
        {
            delay( atraso );

            barometro.getTemperature( temperatura );
        }

        proximaLeituraT += delayTemp;
    }

//    Serial.print( temperatura );
//    Serial.print( " " );

    atraso = barometro.startPressure( 3 ); // oversampling 0-3

    if( atraso > 0 )
    {
        delay( atraso );

        double pressao;

        atraso = barometro.getPressure( pressao, temperatura );

//        pressao -= 2.9; // offset erro sensor
/*
        Serial.print( pressao );
        Serial.print( " " );
*/
        if( atraso != 0 )
        {
            return( barometro.altitude( pressao, 1013.25 ) * 3.28084 ); // MSL Mean Sea Level
        }
    }

    return -666;
}

void setup()
{
    toneAC( 3200, 5, 100 );
/*
    for(int x=2700; x<3200; x+=100)
    {
        toneAC( x, 10, 250 );
        delay(500);
    }
*/
    Serial.begin(115200);

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
        double alti = readAltitudeFtBmp( 0 );

        circular1s.insere( alti );
        circular10.insere( circular1s.media() );
        circular10media.insere( circular10.media() );
    }

    //double varianciaT0 = circular1s.variancia();

    salto.altitudePS = salto.altitudeDZ = salto.altitudeCMD = agora.altitude = antes.altitude = circular10.media();

    tonePlayer.insere( 250, 2700, 10 );

    antes.relogio = millis();
}

void loop()
{
//    unsigned long inicio = millis();

    // atualiza sensores

    #ifdef BMP180
    agora.sensor = readAltitudeFtBmp( 1000 );
    #endif

    #ifdef MPL3115A2
    agora.sensor = altimetro.readAltitudeFt();
    #endif

    agora.relogio = millis();

    // etapa predicao

    double deltaT = ( (double) agora.relogio - antes.relogio ) / 1000.0;

    agora.altitude = antes.altitude + antes.velocidade * deltaT;
    agora.velocidade = antes.velocidade;

    double desvio = agora.sensor - agora.altitude;

    double alpha = 0.1;
    double beta = 0.001;

    agora.altitude += alpha * desvio;
    agora.velocidade += ( beta * desvio ) / deltaT;

//    // Kalman
//
//    static double ganho= 0;
//    static double p = 1;
//    ganho = p + 0.05 / (p + variancia);
//    agora.altitude = antes.altitude + ganho * ( agora.sensor - antes.altitude );
//    p = ( 1 - ganho ) * p;

    circular1s.insere( agora.altitude );

    if( agora.relogio >= tUmSegundo )
    {
        tUmSegundo = agora.relogio + 1000;

        circular10.insere( circular1s.media() );

        circular10media.insere( circular10.media() );
    }

        if( trace )
        {
    //        Serial.print( deltaT * 1000 );
    //        Serial.print( " " );
    //        Serial.print( agora.relogio - inicio );
    //        Serial.print( " " );
            Serial.print( agora.altitude, 2 );
//            Serial.print( agora.altitude - salto.altitudeDZ, 2 );
//            Serial.print( " " );
//            Serial.print( circular10.media() , 2  );
            //Serial.print( circular10.media() - altitudeDZ, 2  );
            Serial.print( " " );
    //        Serial.print( agora.velocidade, 2  );
    //        Serial.print( " " );
//            Serial.print( agora.sensor - salto.altitudeDZ, 2  );
            Serial.print( *circular10media.topo() , 2 );
            Serial.print( " " );
    //        Serial.print( salto.altitudePS - salto.altitudeDZ, 2 );
    //        Serial.print( " " );
            Serial.print( circular1s.media() , 2 );
            Serial.print( " " );
            Serial.print( agora.sensor, 2  );
            //Serial.print( circular1s.media() - salto.altitudeDZ, 2 );
     //       Serial.print( " " );
     //       Serial.print( circular1s.desvio(), 2 );
            Serial.println();
        }

    if( agora.relogio >= tUmDecimo )
    {
        tUmDecimo = agora.relogio + 100;

/*
        // variometro

        if( ! tonePlayer.tocando() )
        {
            if( agora.velocidade > 0.5 )
            {
                int freq = 1000 + agora.velocidade * 1000;
                toneAC( freq, 3, 200, true );
            }
            else if( agora.velocidade < -0.5 )
            {
                int freq = 1000 - agora.velocidade * 500;
                toneAC( freq, 3, 200, true );
            }
            else
            {
                noToneAC();
            }
        }
*/
    }

    switch( agora.estagio )
    {
    case ESTAGIO_SOLO:
        if( agora.velocidade > THRESHOLD_DECOLAGEM )
        {
            salto.altitudeDZ = *circular10media.topo();

            // reset alarmes
            for( int i = 0; i < nAvisos ; i++ )
                avisos[i].atingido = 0;

            tonePlayer.bipe( );

            agora.estagio = ESTAGIO_SUBIDA;
        }
        break;

    case ESTAGIO_SUBIDA:
        if( agora.velocidade < THRESHOLD_QUEDA )
        {
            salto.altitudePS = *circular10media.topo();

            tonePlayer.bipe( 2 );

            agora.estagio = ESTAGIO_QUEDA;
        }
        break;

    case ESTAGIO_QUEDA:
        if( agora.velocidade > THRESHOLD_ABERTURA )
        {
            salto.altitudeCMD = circular1s.media();

            tonePlayer.limpa();
            tonePlayer.bipe( 3 );

            agora.estagio = ESTAGIO_NAVEGACAO;
        }
        break;

    case ESTAGIO_NAVEGACAO:
        if( agora.velocidade < THRESHOLD_QUEDA )
        {
            tonePlayer.limpa();
            tonePlayer.bipe( 2 );

            agora.estagio = ESTAGIO_QUEDA;
        }
        break;
    }

    // avisos de altura

    register int iAltitudeAgora = int( agora.altitude - salto.altitudeDZ );

    for( int i = 0;  i < nAvisos; i++ )
    {
        if( 0 == avisos[i].atingido )
        {
            if( avisos[i].estagio & agora.estagio )
            {
                if( ( ( iAltitudeAgora < avisos[i].altura ) && ( avisos[i].estagio & (ESTAGIO_NAVEGACAO|ESTAGIO_QUEDA) ) )
                 || ( ( iAltitudeAgora > avisos[i].altura ) && ( avisos[i].estagio & (ESTAGIO_SUBIDA) ) ) )
                {
                    avisos[i].atingido = agora.relogio;

                    tonePlayer.insere( avisos[i] );
                }
            }
        }
    }

    // player

    tonePlayer.loop( agora.relogio );

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
    antes = agora;
}
