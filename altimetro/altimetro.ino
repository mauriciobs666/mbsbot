
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


double readAltitudeFt()
{
    char atraso = barometro.startTemperature();

    if( atraso != 0 )
    {
        delay( atraso );

        double T;

        atraso = barometro.getTemperature( T );

        if( atraso != 0 )
        {

            atraso = barometro.startPressure( 3 ); // oversampling 0-3

            if( atraso != 0 )
            {
                delay( atraso );

                double P;

                atraso = barometro.getPressure( P, T );

                if( atraso != 0 )
                {
                    return( barometro.altitude( P, 1013.25 ) * 3.28084 ); // MSL Mean Sea Level
                }
            }
        }
    }

    return -666;
}

//Circular<int,30> circular;
//Circular<Ponto,15> datalog;

double altitudeAgora = 0;
double altitudeAntes = 0;
double altitudeOffset = 0;
double altitudeDelta = 0;
double altitudePS = 0;

double ganho= 0;
double p = 1;

double media = 0;
double variancia = 0;

void setup()
{
    Serial.begin(115200);

    if( ! barometro.begin() )
    {
        Serial.println("Erro inicializando modulo BMP180\n");
        toneAC(200);
        delay(300);
        toneAC();
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

    for( int z = 0, freq = 200; z < CALIBRAGEM_SZ; z++, freq +=50 )
    {
        media += ( calibragem[z] = readAltitudeFt() );
        toneAC( freq );
    }

    toneAC();

    media /= CALIBRAGEM_SZ;

    for( int z = 0; z < CALIBRAGEM_SZ; z++ )
    {
        variancia += pow( calibragem[z] - media, 2 );
    }

    variancia /= CALIBRAGEM_SZ;

    altitudeAgora = altitudeAntes = media;
}

void loop()
{
    long inicio = millis();

    altitudeAntes = altitudeAgora;

    //altitudeAgora = altimetro.readAltitudeFt();

    ganho = p + 0.1 / (p + variancia);

    double sensor = readAltitudeFt();

    altitudeAgora = altitudeAntes + ganho * ( sensor - altitudeAntes );

    p = ( 1 - ganho ) * p;

    altitudeDelta = altitudeAgora - altitudeAntes;

    long agora = millis();

    Serial.print( sensor );
//    Serial.print( agora );
    Serial.print( " " );
    Serial.print( altitudeAgora, 2 );
//    Serial.print( " " );
//    Serial.print( ganho );
//    Serial.print( " " );
//    Serial.print( p );
    Serial.println();

    if( ! altitudeAntes )
    {
        return;
    }

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
}
