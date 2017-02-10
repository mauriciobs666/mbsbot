
// (c) 2017 MBS - Mauricio Bieze Stefani

#include <Wire.h>

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

/*
    pinagem BMP180 X Arduino Nano

    vermelho - 3v3
    verde - a4
    amarelo - a5
    preto - gnd
    azul - d9 e d10
*/

#include <toneAC.h>

#ifdef PRODUCAO

#define THRESHOLD_DECOLAGEM  3.0 // pes/s
#define THRESHOLD_QUEDA    -10.0 // pes/s
#define THRESHOLD_ABERTURA -10.0 // pes/s

#define AVISO_SUBIDA_CINTO 1500
#define AVISO_SUBIDA_CHECK 9000

#define AVISO_ALTA_1    6000
#define AVISO_ALTA_2    5000
#define AVISO_ALTA_3    4000
#define ALARME_ALTA     2500

#define NAVEGACAO_A      300
#define NAVEGACAO_B      600
#define NAVEGACAO_C      900
#define NAVEGACAO_D     1200

#define TRACE          false

#else

#define THRESHOLD_DECOLAGEM  0.8
#define THRESHOLD_QUEDA     -0.8
#define THRESHOLD_ABERTURA   0.8

#define AVISO_SUBIDA_CINTO 12
#define AVISO_SUBIDA_CHECK 24

#define AVISO_ALTA_1      36
#define AVISO_ALTA_2      24
#define AVISO_ALTA_3      12
#define ALARME_ALTA        6

#define NAVEGACAO_A        6
#define NAVEGACAO_B       12
#define NAVEGACAO_C       24
#define NAVEGACAO_D       36

#define TRACE           true

#endif

typedef struct
{
    unsigned long clock;
    double altura;
} Ponto;

typedef struct
{
    int offset;
    int tSubida;
    int tQueda;
    int tNavegacao;
    int ps;
} Salto;

typedef enum
{
    SOLO,
    CLIMB,
    QUEDA,
    NAVEGACAO
} Estado;

typedef struct
{
    int altura;

    enum TipoAviso
    {
        BIP_UNICO = 0,
        BIP_DUPLO,
        BIP_TRIPLO,
        SIRENE
    } tipo;

    char volume;

    unsigned long atingido;
} Aviso;

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

        void bipe( int nbips = 1, int vol = 10, int freq = 5000, int dur = 250, int inter = 250 )
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

Estado estado = SOLO;

CircularStats<double,10> circular10;
CircularStats<double,50> circular1s;

Aviso avisosAlta[4], avisosBaixa[4], avisosSubida[2];

TonePlayer tonePlayer;

double sensorAgora = 0;
double sensorAntes = 0;
double temperatura = 0;
double varianciaT0 = 0;

double altitudeAgora = 0;
double altitudeAntes = 0;
double altitudeDelta = 0;
double altitudeT0 = 0;
double velocidadeAgora = 0;
double velocidadeAntes = 0;
double altitudePS = 0;      // ponto de saida
double altitudeDZ = 0;      // area de pouso

unsigned long agora = 1;
unsigned long ultimaLeitura = 0;
unsigned long tUmSegundo = 0;
unsigned long tUmDecimo = 0;
double deltaT = 0;

bool trace = TRACE;

double readAltitudeFtBmp( long delayTemp = 0 )
{
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
    toneAC( 60, 5, 100 );

//    for( int x = 200; x < 10000; x += 200 )
//    {
//        toneAC( x, 10, 150 );
//        delay( 300 );
//    }

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
        circular10.insere( alti );
    }

    varianciaT0 = circular1s.variancia();

    altitudePS = altitudeT0 = altitudeDZ = altitudeAgora = altitudeAntes = circular1s.media();

    tonePlayer.insere( 250, 5000, 10 );

    for( int a = 0; a < sizeof( avisosAlta[a] ); a++ )
        avisosAlta[a].atingido = 0;

    avisosAlta[0].altura = AVISO_ALTA_1;
    avisosAlta[0].tipo = Aviso::BIP_UNICO;
    avisosAlta[0].volume = 10;
    avisosAlta[1].altura = AVISO_ALTA_2;
    avisosAlta[1].tipo = Aviso::BIP_DUPLO;
    avisosAlta[1].volume = 10;
    avisosAlta[2].altura = AVISO_ALTA_3;
    avisosAlta[2].tipo = Aviso::BIP_TRIPLO;
    avisosAlta[2].volume = 10;
    avisosAlta[3].altura = ALARME_ALTA;
    avisosAlta[3].tipo = Aviso::SIRENE;
    avisosAlta[3].volume = 10;

    for( int a = 0; a < sizeof( avisosBaixa[a] ); a++ )
        avisosBaixa[a].atingido = 0;

    avisosBaixa[0].altura = NAVEGACAO_A;
    avisosBaixa[0].tipo = Aviso::BIP_TRIPLO;
    avisosBaixa[0].volume = 10;
    avisosBaixa[1].altura = NAVEGACAO_B;
    avisosBaixa[1].tipo = Aviso::BIP_DUPLO;
    avisosBaixa[1].volume = 10;
    avisosBaixa[2].altura = NAVEGACAO_C;
    avisosBaixa[2].tipo = Aviso::BIP_UNICO;
    avisosBaixa[2].volume = 10;
    avisosBaixa[3].altura = NAVEGACAO_D;
    avisosBaixa[3].tipo = Aviso::BIP_UNICO;
    avisosBaixa[3].volume = 10;

    for( int a = 0; a < sizeof( avisosSubida[a] ); a++ )
        avisosSubida[a].atingido = 0;

    avisosSubida[0].altura = AVISO_SUBIDA_CINTO;
    avisosSubida[0].tipo = Aviso::BIP_UNICO;
    avisosSubida[0].volume = 5;
    avisosSubida[1].altura = AVISO_SUBIDA_CHECK;
    avisosSubida[1].tipo = Aviso::BIP_DUPLO;
    avisosSubida[1].volume = 5;

    ultimaLeitura = millis();
}

void loop()
{
    unsigned long inicio = millis();

    // atualiza sensores

    #ifdef BMP180
    sensorAgora = readAltitudeFtBmp( 1000 );
    #endif

    #ifdef MPL3115A2
    sensorAgora = altimetro.readAltitudeFt();
    #endif

    agora = millis();

    // etapa predicao

    deltaT = ( (double) agora - ultimaLeitura ) / 1000.0;

    altitudeAgora = altitudeAntes + velocidadeAntes * deltaT;
    velocidadeAgora = velocidadeAntes;

    double desvio = sensorAgora - altitudeAgora;

    double alpha = 0.1;
    double beta = 0.001;

    altitudeAgora += alpha * desvio;
    velocidadeAgora += ( beta * desvio ) / deltaT;

//    // Kalman
//
//    static double ganho= 0;
//    static double p = 1;
//    ganho = p + 0.05 / (p + variancia);
//    altitudeAgora = altitudeAntes + ganho * ( sensorAgora - altitudeAntes );
//    p = ( 1 - ganho ) * p;

    circular1s.insere( altitudeAgora );

    if( agora >= tUmSegundo )
    {
        tUmSegundo = agora + 1000;

        circular10.insere( circular1s.media() );
    }

    if( agora >= tUmDecimo )
    {
        tUmDecimo = agora + 100;
/*
        // variometro

        if( ! tonePlayer.tocando() )
        {
            if( velocidadeAgora > 0.5 )
            {
                int freq = 1000 + velocidadeAgora * 1000;
                toneAC( freq, 3, 200, true );
            }
            else if( velocidadeAgora < -0.5 )
            {
                int freq = 1000 - velocidadeAgora * 500;
                toneAC( freq, 3, 200, true );
            }
            else
            {
                noToneAC();
            }
        }
*/
    }

    switch( estado )
    {
    case SOLO:
        if( velocidadeAgora > THRESHOLD_DECOLAGEM )
        {
            tonePlayer.bipe( );

            estado = CLIMB;
        }
        else
        {
            altitudeDZ = altitudePS = *circular10.topo();
        }
        break;

    case CLIMB:
        if( velocidadeAgora < THRESHOLD_QUEDA )
        {
            tonePlayer.bipe( 2 );

            estado = QUEDA;
        }
        else
        {
            altitudePS = *circular10.topo();
        }

        for( int i = 0;  i < sizeof( avisosSubida ); i++ )
        {
            if( ! avisosSubida[i].atingido && altitudeAgora > avisosSubida[i].altura )
            {
                avisosSubida[i].atingido = agora;

                tonePlayer.insere( avisosSubida[i] );
            }
        }

        break;

    case QUEDA:
        if( velocidadeAgora > THRESHOLD_ABERTURA )
        {
            tonePlayer.limpa();
            tonePlayer.bipe( 3 );

            estado = NAVEGACAO;
        }

        for( int i = 0;  i < sizeof( avisosAlta ); i++ )
        {
            if( ! avisosAlta[i].atingido && altitudeAgora < avisosAlta[i].altura )
            {
                avisosAlta[i].atingido = agora;

                tonePlayer.insere( avisosAlta[i] );
            }
        }

        break;

    case NAVEGACAO:
        if( velocidadeAgora < THRESHOLD_QUEDA )
        {
            tonePlayer.bipe( 2 );

            estado = QUEDA;
        }

        for( int i = 0;  i < sizeof( avisosBaixa ); i++ )
        {
            if( ! avisosBaixa[i].atingido && altitudeAgora < avisosBaixa[i].altura )
            {
                avisosBaixa[i].atingido = agora;

                tonePlayer.insere( avisosBaixa[i] );
            }
        }

        break;
    }

    // player

    tonePlayer.loop( agora );

    if( trace )
    {
//        Serial.print( deltaT * 1000 );
//        Serial.print( " " );
//        Serial.print( agora - inicio );
//        Serial.print( " " );
//        Serial.print( altitudeAgora, 2 );
        Serial.print( altitudeAgora - altitudeDZ, 2 );
        Serial.print( " " );
//        Serial.print( circular10.media() - altitudeDZ, 2  );
//        Serial.print( " " );
        Serial.print( velocidadeAgora, 2  );
        Serial.print( " " );
        Serial.print( sensorAgora - altitudeDZ, 2  );
        Serial.print( " " );
        Serial.print( altitudePS - altitudeDZ, 2 );
        Serial.print( " " );
        Serial.print( circular1s.desvio(), 2 );
        Serial.println();
    }

    sensorAntes = sensorAgora;
    altitudeAntes = altitudeAgora;
    velocidadeAntes = velocidadeAgora;
    ultimaLeitura = agora;

    //delay(100);
}
