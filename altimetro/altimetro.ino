
// (c) 2017-2018 MBS - Mauricio Bieze Stefani

#include <math.h>
#include <Wire.h>
#include <EEPROM.h>

//#define DEBUG 1

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    // memoria infinita
    #define CARTAO_SD
    #define SERIALX Serial2
#else
    #define SERIALX Serial
#endif

#define BMP180


#define SERIALX_SPD 115200

#define DFT_DELAY_TRACE 100

// mascara
#define TRACE_MASTER_EN             0x01
#define TRACE_SENSOR                0x02
#define TRACE_ALTURA                0x04
#define TRACE_VELOCIDADE            0x08
#define TRACE_ALTITUDE              0x10
#define TRACE_AVISOS                0x20
#define TRACE_ANDROID_GRAPHICS_APP  0x40
#define TRACE_MICROSD               0x80

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

#ifdef CARTAO_SD
//    #include <SPI.h>
    #include <SD.h>
//    Sd2Card card;
//    SdVolume volume;
//    SdFile root;
    bool sdOk = false;
#endif // CARTAO_SD

#define ESTADO_DZ           0x00
#define ESTADO_SUBIDA       0x01
#define ESTADO_QUEDA        0x02
#define ESTADO_NAVEGACAO    0x04

#include "protocolo.h"
#include "interpretador.hpp"
Interpretador interpretador;

#define DEBOUNCE_POUSO       10000
#define DEBOUNCE_SUBTERMINAL 15000
#define DEBOUNCE_VEL_MAX_NAV 10000

#ifndef DEBUG

// PRODUCAO !

#define THRESHOLD_DECOLAGEM    6.0
#define THRESHOLD_QUEDA     -130.0
#define THRESHOLD_ABERTURA   -80.0
#define THRESHOLD_SUBTERMINAL -7.0
#define THRESHOLD_POUSO       -1.0

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

#define THRESHOLD_DECOLAGEM    0.8
#define THRESHOLD_QUEDA       -0.8
#define THRESHOLD_ABERTURA     0.8
#define THRESHOLD_SUBTERMINAL -3.0
#define THRESHOLD_POUSO       -0.5

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
            SERIALX.print( "A: #" );
            SERIALX.print( numero );
            SERIALX.print( " dz=" );
            SERIALX.print( altitudeDZ );
            SERIALX.print( "msl sub=" );
            SERIALX.print( tempoSubida );
            SERIALX.print( "s ps=" );
            SERIALX.print( alturaSaida );
            SERIALX.print( "agl ql=" );
            SERIALX.print( tempoQueda );
            SERIALX.print( "s max=" );
            SERIALX.print( velocidadeMaxQueda );
            SERIALX.print( "ft/s cmd=" );
            SERIALX.print( alturaAbertura );
            SERIALX.print( "agl nav=" );
            SERIALX.print( tempoNavegacao );
            SERIALX.print( "s max=" );
            SERIALX.print( velocidadeMaxNavegacao );
            SERIALX.println( "ft/s" );
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
              timestamp(0.0) {}

    unsigned long timestamp;
    double altitude;
    double velocidade;

    void limpa()
    {
        timestamp = 0;
        altitude = velocidade = 0.0;
    }

    void print()
    {

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
                repeticoes+=4;
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

        anotacao.altitudeDZ = decolagem.altitude;
        anotacao.alturaSaida = saida.altitude - decolagem.altitude;
        anotacao.alturaAbertura = abertura.altitude - decolagem.altitude;

        anotacao.tempoSubida = ( saida.timestamp - decolagem.timestamp ) / 1000;
        anotacao.tempoQueda = ( abertura.timestamp - saida.timestamp ) / 1000;
        anotacao.tempoNavegacao = ( pouso.timestamp - abertura.timestamp ) / 1000;

/**
//      1m = 3.28 ft
//      1 ft/s = 1,09 km/h
//      g = 9.81 m/s = 32.18 ft/s
//      v^2 = v0 + 2 * g * deltaH :. deltaH = 130^2 / ( 2 * 32.18 ) =  262 ft
//      v = v0 + gt :. t = 130 / 32.18 = 4.03 s
//      h = h0 + v0 * t + g * t^2 / 2 :. 262 = 32.18 * 4^2 /2
*/
        eeprom.insere( &anotacao );

        tocadorToneAC.limpa();
        tocadorToneAC.bipe();

        if( eeprom.dados.trace & ( TRACE_AVISOS | TRACE_MASTER_EN ) )
        {
            SERIALX.print("<");
            SERIALX.print( (int) estado );
            SERIALX.print(">");
            anotacao.print();
        }
    }

    void limpa()
    {
        estado = ESTADO_DZ;
        decolagem.limpa();
        saida.limpa();
        abertura.limpa();
        pouso.limpa();
        anotacao.limpa();
    }
}
salto;

CircularStats<int,10> circular10;        // medias dos ultimos 10 segundos

unsigned long tUmSegundo = 0;
unsigned long tTrace = 0;
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
    else if( 0 == strncmp( token, CMD_UNAME, TAM_TOKEN )  )
        eeprom.dump();
    else if( 0 == strncmp( token, CMD_BIP, TAM_TOKEN )  )
    {
        int n = 1;

        if( getToken() == NUMERO )
        {
            getInt( &n );
        }

        tocadorToneAC.bipe( n );
    }
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
    else if( 0 == strncmp( token, CMD_AVISOS, TAM_TOKEN )  )
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

    interpretador.declaraVar( VAR_INT, NOME_TRACE, &eeprom.dados.trace );
    interpretador.declaraVar( VAR_INT, NOME_T_TRC, &eeprom.dados.delayTrace );
    interpretador.declaraVar( VAR_INT, NOME_LOOPS, &loopsSeg );
    interpretador.declaraVar( VAR_DOUBLE, NOME_ALPHA, &eeprom.dados.alpha );
    interpretador.declaraVar( VAR_DOUBLE, NOME_BETA, &eeprom.dados.beta );
    interpretador.declaraVar( VAR_LONG, NOME_TIMESTAMP,  &agora.timestamp );

    if( ! barometro.begin() )
    {
        Serial.println("Err ini BMP180");
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
        circular10.insere( readAltitudeFtBmp() );
    }

    salto.limpa();
    salto.saida.altitude = salto.decolagem.altitude = salto.abertura.altitude = agora.altitude = antes.altitude = *circular10.topo();

    tocadorToneAC.insere( 250, 2700, 10 );

    antes.timestamp = millis();

    #ifdef CARTAO_SD
        sdOk = SD.begin( 53 );
    #endif // CARTAO_SD
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

    int altura = int( agora.altitude - salto.decolagem.altitude );

    contadorLoop++;

    if( agora.timestamp >= tUmSegundo )
    {
        tUmSegundo = agora.timestamp + 1000;

        circular10.insere( agora.altitude );

        loopsSeg = contadorLoop;
        contadorLoop = 0;
    }

    // regras de negocio

    static unsigned long debounceVelMaxNav = 0;

    switch( salto.estado )
    {
    case ESTADO_DZ:
        salto.decolagem.altitude = *circular10.topo();

        if( agora.velocidade > THRESHOLD_DECOLAGEM )
        {
            salto.decolagem.timestamp = agora.timestamp;
            salto.saida = salto.abertura = salto.pouso = salto.decolagem;

            // reset alarmes
            for( int i = 0; i < nAvisos ; i++ )
                avisos[i].atingido = 0;

            salto.trocaEstado( ESTADO_SUBIDA );
        }
        else if( agora.velocidade < THRESHOLD_QUEDA ) // inicio da queda livre
        {
            salto.saida = agora;
            salto.trocaEstado( ESTADO_QUEDA );
        }
        break;

    case ESTADO_SUBIDA:

        static unsigned long debounceAbertura = 0;

        if( agora.velocidade < THRESHOLD_QUEDA ) // inicio da queda livre
        {
            salto.saida = agora;
            salto.saida.altitude = *circular10.topo(); // despreza variacao durante aceleracao e usa alt de 10s atras

            salto.trocaEstado( ESTADO_QUEDA );
            debounceAbertura = 0;
        }
        else if( agora.velocidade < THRESHOLD_SUBTERMINAL ) // abertura subterminal
        {
            if( debounceAbertura == 0 )
            {
                debounceAbertura = agora.timestamp + DEBOUNCE_SUBTERMINAL;

                salto.saida = agora;
                salto.saida.altitude = *circular10.topo(); // despreza variacao durante aceleracao e usa alt de 10s atras

                salto.abertura = salto.saida;
            }
            else if( agora.timestamp > debounceAbertura )
            {
                debounceVelMaxNav = agora.timestamp;

                salto.trocaEstado( ESTADO_NAVEGACAO );
                debounceAbertura = 0;
            }
        }
        else // subindo ou estavel
        {
            debounceAbertura = 0;
        }
        break;

    case ESTADO_QUEDA:
        if( agora.velocidade < salto.anotacao.velocidadeMaxQueda )
        {
            salto.anotacao.velocidadeMaxQueda = agora.velocidade;
        }

        if( agora.velocidade > THRESHOLD_ABERTURA )
        {
            debounceVelMaxNav = agora.timestamp + DEBOUNCE_VEL_MAX_NAV;

            salto.abertura = agora;
            salto.trocaEstado( ESTADO_NAVEGACAO );
        }
        break;

    case ESTADO_NAVEGACAO:

        static unsigned long debouncePouso = 0;

        if( agora.velocidade < salto.anotacao.velocidadeMaxNavegacao && debounceVelMaxNav < agora.timestamp )
        {
            salto.anotacao.velocidadeMaxNavegacao = agora.velocidade;
        }

        if( agora.velocidade < THRESHOLD_QUEDA ) // inicio da queda livre
        {
            salto.saida = agora;
            salto.trocaEstado( ESTADO_QUEDA );
            debouncePouso = 0;
        }
        else if( agora.velocidade > THRESHOLD_POUSO ) // pouso
        {
            if( altura < 1000 ) // gambi: mil pes de tolerancia pra nao ter mais alarme falso de pouso
            {
                if( debouncePouso == 0)
                {
                    debouncePouso = agora.timestamp + DEBOUNCE_POUSO;
                    salto.pouso = agora;
                }
                else if ( debouncePouso <= agora.timestamp )
                {
                    salto.trocaEstado( ESTADO_DZ );
                    // salto.limpa();
                    debouncePouso = 0;
                }
            }
        }
        else
        {
            debouncePouso = 0;
        }

        break;
    }

    // avisos de altura

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

                    if( eeprom.dados.trace & ( TRACE_AVISOS | TRACE_MASTER_EN ) )
                    {
                        SERIALX.print("[");
                        SERIALX.print(i);
                        SERIALX.print("]<");
                        SERIALX.print( (int) salto.estado );
                        SERIALX.print(">");
                        avisos[i].print();
                        SERIALX.println();
                    }
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

        bool androidGraphicsApp = ( eeprom.dados.trace & TRACE_ANDROID_GRAPHICS_APP );
        bool qquerCoisa = false;

        if( androidGraphicsApp )
        {
            SERIALX.print( "E" );
            qquerCoisa = true;
        }

        if( eeprom.dados.trace & TRACE_ALTURA )
        {
            if( qquerCoisa ) SERIALX.print( androidGraphicsApp ? "," : " " );
            SERIALX.print( agora.altitude - salto.decolagem.altitude, 2 ); // Serial.print( altura );
            qquerCoisa = true;
        }

        if( eeprom.dados.trace & TRACE_ALTITUDE )
        {
            if( qquerCoisa ) SERIALX.print( androidGraphicsApp ? "," : " " );
            SERIALX.print( agora.altitude, 2 );
            qquerCoisa = true;
        }

        if( eeprom.dados.trace & TRACE_VELOCIDADE )
        {
            if( qquerCoisa ) SERIALX.print( androidGraphicsApp ? "," : " " );
            SERIALX.print( agora.velocidade, 2  );
            qquerCoisa = true;
        }

        if( eeprom.dados.trace & TRACE_SENSOR )
        {
            if( qquerCoisa ) SERIALX.print( androidGraphicsApp ? "," : " " );
            SERIALX.print( sensor - salto.decolagem.altitude, 2  );
            qquerCoisa = true;
        }

    //        SERIALX.print( deltaT * 1000 );
    //        SERIALX.print( circular10.media() , 2  );
    //        SERIALX.print( circular10.media() - decolagem.altitude, 2  );
    //        SERIALX.print( *circular10media.topo() - salto.decolagem.altitude, 2 );
    //        SERIALX.print( salto.saida.altitude - salto.decolagem.altitude, 2 );
    //        SERIALX.print( circular1s.media() , 2 );
    //        SERIALX.print( circular1s.media() - salto.decolagem.altitude, 2 );
    //        SERIALX.print( circular1s.desvio(), 2 );

        if( qquerCoisa )
            SERIALX.println();

        #ifdef CARTAO_SD
        if( eeprom.dados.trace & TRACE_MICROSD)
        {
            File dataFile = SD.open( "datalog.txt", FILE_WRITE);

            if (dataFile)
            {
                String dataString = "";

                dataString += String( agora.altitude - salto.decolagem.altitude );
                dataString += String( agora.altitude );
                dataString += String( agora.velocidade );
                dataString += String( sensor - salto.decolagem.altitude );

                dataFile.println( dataString );

                dataFile.close();
            }
            else
                SERIALX.println("error datalog.txt");
        }
        #endif
    }

    antes = agora;
}
