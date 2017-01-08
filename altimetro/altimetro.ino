
// (c) 2016 MBS - Mauricio Bieze Stefani

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

template <class tipo, int tamanho> class Circular
{
    tipo circular[tamanho];
    int leitura;
    int escrita;

    int proximo( int ponteiro )
    {
        ponteiro++;

        if( ponteiro == tamanho )
            ponteiro = 0;

        return ponteiro;
    }

    int incrementa( int & ponteiro )
    {
        return ( ponteiro = proximo( ponteiro ) );
    }

public:
    Circular()
    {
        limpa();
    }

    void limpa()
    {
        leitura = escrita = 0;
    }

    bool vazio()
    {
        return ( escrita == leitura );
    }

    void insere( const tipo & elemento )
    {
        circular[escrita] = elemento;

        incrementa(escrita);

        if( escrita == leitura )
            incrementa( leitura );
    }

    void retira()
    {
        if( escrita != leitura )
            incrementa( leitura );
    }

    tipo * topo()
    {
        return vazio() ? NULL : &circular[leitura];
    }
};

typedef struct Ponto
{
    int clock;
    int altura;
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

Circular<double,30> circular;
//Circular<Ponto,15> datalog;

double sensor = 0;
double temperatura = 0;
double altitudeAgora = 0;
double altitudeAntes = 0;
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

        proximaLeituraT += 100;
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
    toneAC( 60, 3, 100 );

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
    }
    media /= CALIBRAGEM_SZ;

    for( int z = 0; z < CALIBRAGEM_SZ; z++ )
    {
        variancia += pow( calibragem[z] - media, 2 );
    }

    variancia /= CALIBRAGEM_SZ;

    altitudeDZ = altitudeAgora = altitudeAntes = media;

    toneAC( 4000, 10, 150 );
}

void loop()
{
    long inicio = millis();

    // leitura sensores

    #ifdef BMP180
    sensor = readAltitudeFt();
    #endif

    #ifdef MPL3115A2
    sensor = altimetro.readAltitudeFt();
    #endif

    // etapa predicao Kalman

    ganho = p + 0.05 / (p + variancia);

    altitudeAgora = altitudeAntes + ganho * ( sensor - altitudeAntes );

    // etapa update Kalman

    p = ( 1 - ganho ) * p;

    altitudeDelta = altitudeAgora - altitudeAntes;

    agora = millis();

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
//    Serial.print( " " );
//    Serial.print( temperatura );
//    Serial.print( " " );
//    Serial.print( p );
    Serial.println();

    altitudeAntes = altitudeAgora;
}
