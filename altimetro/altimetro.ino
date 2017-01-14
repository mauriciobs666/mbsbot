
// (c) 2017 MBS - Mauricio Bieze Stefani

#define BMP180

#include <Wire.h>

#include "circular.h"

#ifdef MPL3115A2
#include <SparkFunMPL3115A2.h>
#endif

#ifdef BMP180
#include <SFE_BMP180.h>
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

#define THRESHOLD_DECOLAGEM  1
#define THRESHOLD_QUEDA     -1
#define THRESHOLD_NAVEGACAO -3

#define AVISO_ALTA_1    6000
#define AVISO_ALTA_2    5000
#define AVISO_ALTA_3    4000
#define ALARME_ALTA     2500

#define NAVEGACAO_A      300
#define NAVEGACAO_B      600
#define NAVEGACAO_C      900
#define NAVEGACAO_D     1200

typedef struct Ponto
{
    unsigned long clock;
    float altura;
};

typedef struct Salto
{
    int offset;
    int tSubida;
    int tQueda;
    int tNavegacao;
    int ps;
};

typedef enum Estado
{
    SOLO,
    CLIMB,
    QUEDA,
    NAVEGACAO
};

class TonePlayer
{
    public:
        typedef struct Nota
        {
            int frequencia;
            int duracao;
        } notas[10];

        int notasI;
        int notasO;
};

#ifdef MPL3115A2
MPL3115A2 altimetro;
#endif

SFE_BMP180 barometro;

Estado estado = SOLO;

CircularStats<double,60> circular60s;
CircularStats<double,15> circular15s;

double sensorAgora = 0;
double sensorAntes = 0;
double temperatura = 0;
double deltaT = 0;
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
unsigned long tUmSegundo= 0;

bool trace = true;

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

    atraso = barometro.startPressure( 3 ); // oversampling 0-3

    if( atraso > 0 )
    {
        delay( atraso );

        double pressao;

        atraso = barometro.getPressure( pressao, temperatura );

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
        while(1);
    }

#ifdef MPL3115A2
    Wire.begin();
    altimetro.begin();
    altimetro.setModeAltimeter();
    altimetro.setOversampleRate(7);
    altimetro.enableEventFlags();
#endif

    #define CALIBRAGEM_SZ 60
//    #define CALIBRAGEM_SZ 100
//    double calibragem[CALIBRAGEM_SZ];
    double media = 0;
    double varianciaT0 = 0;

    for( int z = 0; z < CALIBRAGEM_SZ; z++ )
    {
        circular60s.insere( readAltitudeFtBmp( 0 ) );
//        media += ( calibragem[z] = readAltitudeFtBmp() );
//        delay(10);
    }
//    media /= CALIBRAGEM_SZ;
    media = circular60s.media();

//    for( int z = 0; z < CALIBRAGEM_SZ; z++ )
//    {
//        varianciaT0 += pow( calibragem[z] - media, 2 );
//    }
//
//    varianciaT0 /= CALIBRAGEM_SZ;
    varianciaT0 = circular60s.variancia();

    altitudePS = altitudeT0 = altitudeDZ = altitudeAgora = altitudeAntes = media;

    toneAC( 5000, 5, 150 );

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



    //double analogico = analogRead(1);

    deltaT = ( (double) agora - ultimaLeitura ) / 1000.0;

    altitudeAgora = altitudeAntes + velocidadeAntes * deltaT;
    velocidadeAgora = velocidadeAntes;

    double desvio = sensorAgora - altitudeAgora;
//    double alpha = analogico / 1000;
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

    if( agora > tUmSegundo )
    {
        circular60s.insere( altitudeAgora );
        circular15s.insere( altitudeAgora );

        tUmSegundo = agora + 1000;
    }

    switch( estado )
    {
    case SOLO:
        if( velocidadeAgora > THRESHOLD_DECOLAGEM )
        {
            toneAC( 5000, 10, 150 );

            estado = CLIMB;
        }
        else
        {
            altitudeDZ = altitudePS = circular60s.media();
        }
        break;
    case CLIMB:
        if( velocidadeAgora < THRESHOLD_QUEDA )
        {
            toneAC( 5000, 10, 150 );
            delay(100);
            toneAC( 5000, 10, 150 );

            estado = QUEDA;
        }
        else
        {
            altitudePS = circular15s.media();
        }
        break;
    case QUEDA:
        if( velocidadeAgora > THRESHOLD_NAVEGACAO )
        {
            toneAC( 5000, 10, 150 );
            delay(100);
            toneAC( 5000, 10, 150 );
            delay(100);
            toneAC( 5000, 10, 150 );

            estado = NAVEGACAO;
        }
        break;
    case NAVEGACAO:

        break;
    }

    // player



    // trace

    if( trace )
    {
//        Serial.print( deltaT * 1000 );
//        Serial.print( " " );
//        Serial.print( agora - inicio );
//        Serial.print( " " );
        Serial.print( altitudeAgora - altitudeDZ, 2 );
        Serial.print( " " );
        Serial.print( circular15s.media() - altitudeDZ, 2  );
        Serial.print( " " );
        Serial.print( velocidadeAgora, 2  );
        Serial.print( " " );
        Serial.print( sensorAgora - altitudeDZ, 2  );
        Serial.print( " " );
        Serial.print( altitudePS - altitudeDZ, 2 );
        Serial.println();
    }

    sensorAntes = sensorAgora;
    altitudeAntes = altitudeAgora;
    velocidadeAntes = velocidadeAgora;
    ultimaLeitura = agora;

    //delay(100);
}
