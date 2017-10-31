
// (c) 2017 MBS - Mauricio Bieze Stefani

#include <math.h>
#include <Wire.h>
#include <EEPROM.h>

#define BMP180
#define DEBUG 1

#include "impressora.hpp"

#include "circular.hpp"

#include "tocador.hpp"
TocadorToneAC tocadorToneAC;

#ifdef MPL3115A2
#include <SparkFunMPL3115A2.h>
MPL3115A2 altimetro;
#endif

#ifdef BMP180
#include <SFE_BMP180.h>
SFE_BMP180 barometro;
#endif

#define ESTADO_DZ           0x00
#define ESTADO_SUBIDA       0x01
#define ESTADO_QUEDA        0x02
#define ESTADO_NAVEGACAO    0x04

#include "protocolo.h"
#include "interpretador.hpp"
Interpretador interpretador;

#ifndef DEBUG

// PRODUCAO !

#define THRESHOLD_DECOLAGEM   6.0 // pes/s
#define THRESHOLD_QUEDA    -150.0 // pes/s
#define THRESHOLD_ABERTURA  -60.0 // pes/s

#define AVISO_SUBIDA_CINTO  1500
#define AVISO_SUBIDA_CHECK 12000

#define AVISO_ALTA_1    6500
#define AVISO_ALTA_2    5500
#define AVISO_ALTA_3    4500
#define ALARME_ALTA     3500

#define AVISO_NAVEGACAO_A      300
#define AVISO_NAVEGACAO_B      600
#define AVISO_NAVEGACAO_C      900
#define AVISO_NAVEGACAO_D     1200

#define TRACE 0

#else

// DEBUG

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

#define TRACE ( TRACE_MASTER_EN | TRACE_SENSOR | TRACE_ALTURA | TRACE_VELOCIDADE )

#endif

class Eeprom
{
    int tamanhoBufSaltos;
public:
    struct Configuracao
    {
        char trace;
        int delayTrace;
        double alpha;
        double beta;
        double gama;
    }
    dados;

    struct Caderneta
    {
        int numero;
        int altitudeDZ;
        int tempoSubida;
        int alturaSaida;
        int tempoQueda;
        int velocidadeMaxQueda;
        int alturaAbertura;
        int tempoNavegacao;
        int velocidadeMaxNavegacao;

        void print()
        {
            SERIALX.print( "#" );
            SERIALX.print( numero );
            SERIALX.print( " alt=" );
            SERIALX.print( altitudeDZ );
            SERIALX.print( " sub=" );
            SERIALX.print( tempoSubida );
            SERIALX.print( " ps=" );
            SERIALX.print( alturaSaida );
            SERIALX.print( " tql=" );
            SERIALX.print( tempoQueda );
            SERIALX.print( " vmq=" );
            SERIALX.print( velocidadeMaxQueda );
            SERIALX.print( " opn=" );
            SERIALX.print( alturaAbertura );
            SERIALX.print( " tnav=" );
            SERIALX.print( tempoNavegacao );
            SERIALX.print( " vmn=" );
            SERIALX.println( velocidadeMaxNavegacao );
        }

        void limpa()
        {
            numero = altitudeDZ = tempoSubida = alturaSaida = tempoQueda = 0;
            velocidadeMaxQueda = alturaAbertura = tempoNavegacao = velocidadeMaxNavegacao = 0;
        }
    };

    int init()
    {
        int carregados = carrega();

        int e2len = EEPROM.length();
        tamanhoBufSaltos = ( e2len - sizeof( Configuracao ) ) / sizeof( Caderneta );

        SERIALX.print( "E2 sz=" );
        SERIALX.print( e2len );
        SERIALX.print( "B cfg=" );
        SERIALX.print( carregados );
        SERIALX.print( "B disp=" );
        SERIALX.println( tamanhoBufSaltos );
    }

    int insere( struct Caderneta *salto )
    {
        struct Caderneta tmp;

        int endereco = sizeof( dados ); // layout: alocado na E2 logo apos a membro dados

        int iter = 0;

        for( ; iter < tamanhoBufSaltos; iter++, endereco += sizeof( Caderneta ) )
        {
            EEPROM.get( endereco, tmp );

            if( salto->numero > 0 )
            {
                // update
                if( salto->numero == tmp.numero )
                {
                    tmp.numero = -salto->numero;
                    EEPROM.put( endereco, *salto );
                    break;
                }
            }
            else if( tmp.numero < 0 )
            {
                // insere em posicao vazia
                salto->numero = -tmp.numero;
                EEPROM.put( endereco, *salto );
                break;
            }
        }

        if( iter >= tamanhoBufSaltos )
        {
            // limpa
            iter = 0;
            endereco = sizeof( dados );

            if( salto->numero == 0 )
                salto->numero = 1;

            tmp.numero = -salto->numero;

            EEPROM.put( endereco, *salto );
        }

        // prepara proximo salto

        tmp.numero--;

        if( iter+1 >= tamanhoBufSaltos )
            endereco = sizeof( dados );
        else
            endereco += sizeof( Caderneta );

        EEPROM.put( endereco, tmp );
    }

    int dump()
    {
        struct Caderneta tmp;

        int endereco = sizeof( dados ); // layout: alocado logo apos a membro dados

        for( int iter = 0; iter < tamanhoBufSaltos; iter++, endereco += sizeof( Caderneta ) )
        {
            EEPROM.get( endereco, tmp );
            tmp.print();
        }
    }

    int limpa( int nProximoSalto )
    {
        struct Caderneta tmp;

        tmp.limpa();
        tmp.numero = -nProximoSalto;

        EEPROM.put( sizeof(dados), tmp );
    }

    int carrega()
    {
        unsigned int addr = 0;
        char * dest = (char*) &dados;
        for( ; addr < sizeof(dados); addr++, dest++ )
            *dest = eeprom_read_byte(( unsigned char * ) addr );
        return addr;
    }

    void salva()
    {
        char * dest = (char*) &dados;
        for( unsigned int addr = 0; addr < sizeof(dados); addr++, dest++ )
            eeprom_write_byte(( unsigned char  *) addr, *dest);
    }

    void defaults()
    {
        dados.trace = TRACE;
        dados.delayTrace = DFT_DELAY_TRACE;
        dados.alpha = 0.1;
        dados.beta = 0.001;
    }
}
eeprom;

class Ponto
{
public:
    Ponto() : altitude(0),
              velocidade(0.0),
              timestamp(0.0),
              aceleracao(0.0) {}

    unsigned long timestamp;
    double altitude;
    double velocidade;
    double aceleracao;

    void limpa()
    {
        timestamp = 0;
        altitude = velocidade = aceleracao = 0.0;
    }
}
antes, agora;

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

    void tocar()
    {
//            SERIALX.print("insere(");
//            aviso.print();
//            SERIALX.println(")");
        int repeticoes = 1;

        switch( tipo )
        {
            case TipoAviso::BIP_TRIPLO:
                repeticoes++;
            case TipoAviso::BIP_DUPLO: //fall trough
                repeticoes++;
            case TipoAviso::BIP_UNICO: //fall trough
                tocadorToneAC.bipe( repeticoes, volume );
                break;

            case TipoAviso::SIRENE:
                repeticoes+=7;
            case TipoAviso::SIRENE_TRIPLO: //fall trough
                repeticoes++;
            case TipoAviso::SIRENE_DUPLO:  //fall trough
                repeticoes++;
            case TipoAviso::SIRENE_UNICO:  //fall trough
                for( int n = 0; n < repeticoes; n++ )
                {
                    tocadorToneAC.insere( 150, 2700, volume );
                    tocadorToneAC.insere( 150, 3000, volume );
                }
                break;
        }
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

class Salto
{
public:
    char estado;

    Ponto decolagem;    // area de pouso, DZ
    Ponto saida;
    Ponto abertura;
    Ponto pouso;
    Eeprom::Caderneta anotacao;

    void trocaEstado( char novoEstado )
    {
        estado = novoEstado;

        tocadorToneAC.limpa();
        tocadorToneAC.bipe( );

        switch( estado )
        {
        case ESTADO_SUBIDA:
            anotacao.altitudeDZ = decolagem.altitude;
            break;
        case ESTADO_QUEDA:
            anotacao.alturaSaida = saida.altitude - decolagem.altitude;
            anotacao.tempoSubida = ( saida.timestamp - decolagem.timestamp ) / 1000;
            break;
        case ESTADO_NAVEGACAO:
            anotacao.alturaAbertura = abertura.altitude - decolagem.altitude;
            anotacao.tempoQueda = ( abertura.timestamp - saida.timestamp ) / 1000;
            break;
        case ESTADO_DZ:
            anotacao.tempoNavegacao = ( pouso.timestamp - abertura.timestamp ) / 1000;
            break;
        }

        anotacao.print();

        eeprom.insere( &anotacao );
    }

    void limpa()
    {
        estado = ESTADO_DZ;
        decolagem.limpa();
        saida.limpa();
        abertura.limpa();
        pouso.limpa();
    }
}
salto;

CircularStats<double,30> circular1s;        // ultimas 30 leituras ( ~1 segundo )
CircularStats<double,10> circular10;        // medias dos ultimos 10 segundos
CircularStats<double,10> circular10media;   // delay line das medias dos ultimos 10 segundos

unsigned long tUmSegundo = 0;
unsigned long tTrace = 0;
unsigned long debounceAbertura = 0;
unsigned long debouncePouso = 0;
int contadorLoop = 0;
int loopsSeg = 0;

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

Erros Interpretador::evalHardCoded( Variavel* resultado )
{
    #ifdef TRACE_INTERPRETADOR
        SERIALX.print( "evalHardCoded( " );
        SERIALX.print( token );
        SERIALX.println( " )" );
    #endif

    Erros rc = SUCESSO;

    char dest[TAM_TOKEN];
    strcpy( dest, token );

    eco=false;

    if( 0 == strncmp( token, CMD_GRAVA, TAM_TOKEN )  )	        // salva EEPROM
        eeprom.salva();
    else if( 0 == strncmp( token, CMD_CARREGA, TAM_TOKEN ) )    // descarta mudancas e recarrega da EEPROM
        eeprom.carrega();
    else if( 0 == strncmp( token, CMD_DEFAULT, TAM_TOKEN ) )    // hard-coded
        eeprom.defaults();
    else if( 0 == strncmp( token, CMD_DUMP, TAM_TOKEN )  )
        eeprom.dump();
    else if( 0 == strncmp( token, CMD_RESET, TAM_TOKEN )  )
    {
        salto.estado = ESTADO_DZ;
    }
    else if( 0 == strncmp( token, CMD_CLEAR, TAM_TOKEN )  )
    {
        int temp = 1;

        // segundo token eh o numero do proximo salto
        if( getToken() == NUMERO )
        {
            getInt( &temp );
        }

        eeprom.limpa( temp );
    }
    else
    {
        eco = true;
        rc = SKIP;
    }

    #ifdef TRACE_INTERPRETADOR
//        if( SUCESSO == rc )
//            SERIALX.println( " exec ok" );
//        else
//            SERIALX.println( " nope" );
    #endif

    return rc;
}

void setup()
{
    toneAC( 3200, 5, 100 );

    SERIALX.begin( SERIALX_SPD );

    eeprom.init();

    interpretador.declaraVar( VAR_CHAR, NOME_TRACE, &eeprom.dados.trace );
    interpretador.declaraVar( VAR_INT,  NOME_T_TRC, &eeprom.dados.delayTrace );
    interpretador.declaraVar( VAR_INT,  NOME_LOOPS, &loopsSeg );
    interpretador.declaraVar( VAR_DOUBLE, NOME_ALPHA, &eeprom.dados.alpha );
    interpretador.declaraVar( VAR_DOUBLE, NOME_BETA, &eeprom.dados.beta );
    interpretador.declaraVar( VAR_LONG, NOME_TIMESTAMP,  &agora.timestamp );

    if( ! barometro.begin() )
    {
        Serial.println("Erro inicializando modulo BMP180\n");
        toneAC( 4000, 10, 200 );
        delay( 200 );
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

    salto.limpa();
    salto.saida.altitude = salto.decolagem.altitude = salto.abertura.altitude = agora.altitude = antes.altitude = circular10.media();

    tocadorToneAC.insere( 250, 2700, 10 );

    antes.timestamp = millis();
}

void loop()
{
    // atualiza sensores

    #ifdef BMP180
    double sensor = readAltitudeFtBmp();
    #endif

    #ifdef MPL3115A2
    agora.sensor = altimetro.readAltitudeFt();
    #endif

    agora.timestamp = millis();
    double deltaT = ( (double) ( agora.timestamp - antes.timestamp ) ) / 1000.0;

    // predicao

    agora.altitude = antes.altitude + antes.velocidade * deltaT;
    agora.velocidade = antes.velocidade;

    double desvio = sensor - agora.altitude;

    agora.altitude += eeprom.dados.alpha * desvio;
    agora.velocidade += ( eeprom.dados.beta * desvio ) / deltaT;

//    // Kalman
//
//    static double ganho= 0;
//    static double p = 1;
//    ganho = p + 0.05 / (p + variancia);
//    agora.altitude = antes.altitude + ganho * ( sensor - antes.altitude );
//    p = ( 1 - ganho ) * p;
//    circular1s.insere( agora.altitude );

    circular1s.insere( agora.altitude );

    contadorLoop++;

    if( agora.timestamp >= tUmSegundo )
    {
        tUmSegundo = agora.timestamp + 1000;

        circular10.insere( circular1s.media() );

        circular10media.insere( circular10.media() );

        loopsSeg = contadorLoop;
        contadorLoop = 0;
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

            salto.anotacao.limpa();

            // reset alarmes
            for( int i = 0; i < nAvisos ; i++ )
                avisos[i].atingido = 0;

            salto.trocaEstado( ESTADO_SUBIDA );
        }
        break;

    case ESTADO_SUBIDA:
        if( agora.velocidade < THRESHOLD_QUEDA ) // inicio da queda livre
        {
            debounceAbertura = 0;

            salto.saida.altitude = *circular10media.topo();
            salto.saida.timestamp = agora.timestamp;

            salto.trocaEstado( ESTADO_QUEDA );
        }
        else if( agora.velocidade < -3 ) // abertura subterminal
        {
            if( debounceAbertura == 0 )
            {
                debounceAbertura = agora.timestamp + 10000;
            }
            else if( debounceAbertura < agora.timestamp )
            {
                debounceAbertura = 0;

                salto.saida.altitude = *circular10media.topo();
                salto.saida.timestamp = agora.timestamp - 10000;

                salto.abertura.altitude = circular1s.media();
                salto.abertura.timestamp = agora.timestamp;

                salto.trocaEstado( ESTADO_NAVEGACAO );
            }
        }
        else // subindo ou estavel
        {
            debounceAbertura = 0;
        }
        break;

    case ESTADO_QUEDA:
        if( agora.velocidade > THRESHOLD_ABERTURA )
        {
            salto.abertura.altitude = circular1s.media();
            salto.abertura.timestamp = agora.timestamp;

            salto.trocaEstado( ESTADO_NAVEGACAO );
        }
        break;

    case ESTADO_NAVEGACAO:
        if( agora.velocidade < THRESHOLD_QUEDA )
        {
            salto.trocaEstado( ESTADO_QUEDA );
        }
        //else if( agora.velocidade > -2 && agora.velocidade < 2 ) // pouso
        else if( fabs( agora.velocidade ) < 2 ) // pouso
        {
            if( debouncePouso == 0)
            {
                debouncePouso = agora.timestamp + 10000;
            }
            else if ( debouncePouso < agora.timestamp )
            {
                debouncePouso = 0;

                salto.pouso.altitude = *circular10media.topo();
                salto.pouso.timestamp = agora.timestamp - 10000;

                salto.trocaEstado( ESTADO_DZ );
            }
        }
        else
        {
            debouncePouso = 0;
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
                    avisos[i].tocar();
                }
            }
        }
    }

    // player

    tocadorToneAC.loop( agora.timestamp );

    // interpretador de comandos via serial

    {
        static char comando[MAX_CMD];
        static unsigned char pos = 0;

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

                interpretador.eval( comando );
            }
            else
                comando[ pos++ ] = c;
        }
    }


    if( ( eeprom.dados.trace & TRACE_MASTER_EN ) && ( agora.timestamp >= tTrace ) )
    {
        tTrace = agora.timestamp + eeprom.dados.delayTrace;

        if( eeprom.dados.trace & TRACE_AVISOS )
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

        bool androidGraphicsApp = ( eeprom.dados.trace & TRACE_ANDROID_GRAPHICS_APP );

        if( androidGraphicsApp )
        {
            Serial.print( "E" );
        }

        if( eeprom.dados.trace & TRACE_ALTURA )
        {
            Serial.print( agora.altitude - salto.decolagem.altitude, 2 ); // Serial.print( altura );
            Serial.print( androidGraphicsApp ? "," : " " );
        }

        if( eeprom.dados.trace & TRACE_ALTITUDE )
        {
            Serial.print( agora.altitude, 2 );
            Serial.print( androidGraphicsApp ? "," : " " );
        }

        if( eeprom.dados.trace & TRACE_VELOCIDADE )
        {
            Serial.print( agora.velocidade, 2  );
            Serial.print( androidGraphicsApp ? "," : " " );
        }

        if( eeprom.dados.trace & TRACE_SENSOR )
        {
            //Serial.print( sensor, 2  );
            Serial.print( sensor - salto.decolagem.altitude, 2  );
            Serial.print( androidGraphicsApp ? "," : " " );
        }

    //        Serial.print( deltaT * 1000 );
    //        Serial.print( circular10.media() , 2  );
    //        Serial.print( circular10.media() - decolagem.altitude, 2  );
    //        Serial.print( *circular10media.topo() - salto.decolagem.altitude, 2 );
    //        Serial.print( salto.saida.altitude - salto.decolagem.altitude, 2 );
    //        Serial.print( circular1s.media() , 2 );
    //        Serial.print( circular1s.media() - salto.decolagem.altitude, 2 );
    //        Serial.print( circular1s.desvio(), 2 );

        Serial.println();
    }

    antes = agora;
}
