
// (c) 2017 MBS - Mauricio Bieze Stefani

#define BMP180

#include <Wire.h>

#ifdef MPL3115A2
#include <SparkFunMPL3115A2.h>
#endif

#ifdef BMP180
#include <SFE_BMP180.h>
#endif

#include <toneAC.h>

#define THRESHOLD_SUBIDA     5
#define THRESHOLD_QUEDA     -5
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
    long clock;
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

enum Estado
{
    SOLO,
    SUBIDA,
    QUEDA,
    NAVEGACAO
};

#ifdef MPL3115A2
MPL3115A2 altimetro;
#endif

SFE_BMP180 barometro;

Estado estado = SOLO;

//Circular<double,30> circular;
//Circular<Ponto,15> datalog;

long ultimaLeitura = 0;

double sensor = 0;
double temperatura = 0;
double deltaT = 0;
double altitudeAgora = 0;
double altitudeAntes = 0;
double velocidadeAgora = 0;
double velocidadeAntes = 0;
double altitudeOffset = 0;
double altitudeDelta = 0;
double altitudePS = 0;      // ponto de saida
double altitudeDZ = 0;      // area de pouso

double ganho= 0;
double p = 1;

double media = 0;
double variancia = 0;

long agora = 1;

double readAltitudeFt()
{
    static long proximaLeituraT = 0;

    char atraso;

    // refresh temperatura apenas a cada segundo

    if( millis() > proximaLeituraT || proximaLeituraT == 0 )
    {
        atraso = barometro.startTemperature();

        if( atraso > 0 )
        {
            delay( atraso );

            barometro.getTemperature( temperatura );
        }

        proximaLeituraT += 1;
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

    #define CALIBRAGEM_SZ 100
    double calibragem[CALIBRAGEM_SZ];

    for( int z = 0; z < CALIBRAGEM_SZ; z++ )
    {
        media += ( calibragem[z] = readAltitudeFt() );
        delay(10);
    }
    media /= CALIBRAGEM_SZ;

    for( int z = 0; z < CALIBRAGEM_SZ; z++ )
    {
        variancia += pow( calibragem[z] - media, 2 );
    }

    variancia /= CALIBRAGEM_SZ;

    altitudeDZ = altitudeAgora = altitudeAntes = media;

    toneAC( 5000, 10, 150 );
}

void loop()
{
    long inicio = millis();

    // leitura sensores ( delay variavel )

    #ifdef BMP180
    sensor = readAltitudeFt();
    #endif

    #ifdef MPL3115A2
    sensor = altimetro.readAltitudeFt();
    #endif

    // etapa predicao

    agora = millis();

    deltaT = ( (double) agora - ultimaLeitura ) / 1000.0;

    altitudeAgora = altitudeAntes + velocidadeAntes * deltaT;
    velocidadeAgora = velocidadeAntes;

    double desvio = sensor - altitudeAgora;
    double alpha = 0.1, beta = 0.0005;

    altitudeAgora += alpha * desvio;
    velocidadeAgora += ( beta * desvio ) / deltaT;

//    // Kalman
//    ganho = p + 0.05 / (p + variancia);
//    altitudeAgora = altitudeAntes + ganho * ( sensor - altitudeAntes );
//    p = ( 1 - ganho ) * p;

    altitudeDelta = altitudeAgora - altitudeAntes;

    switch( estado )
    {
    case SOLO:
        if( altitudeDelta > THRESHOLD_SUBIDA )
        {
            //altitudeOffset = circular.media();
            estado = SUBIDA;
        }
        else
        {
//            circular.insere( altitudeAntes );
        }
        break;
    case SUBIDA:
        if( altitudeDelta < THRESHOLD_QUEDA )
        {
            estado = QUEDA;
        }
        break;
    case QUEDA:
        if( altitudeDelta > THRESHOLD_NAVEGACAO )
        {
            estado = QUEDA;
        }
        break;
    case NAVEGACAO:
        break;
    }

    // trace

    Serial.print( sensor - altitudeDZ );
//    Serial.print( agora );
    Serial.print( " " );
    Serial.print( altitudeAgora - altitudeDZ, 2 );
    Serial.print( " " );
    Serial.print( velocidadeAgora );
//    Serial.print( " " );
//    Serial.print( p );
    Serial.println();

    altitudeAntes = altitudeAgora;
    velocidadeAntes = velocidadeAgora;
    ultimaLeitura = agora;
}
