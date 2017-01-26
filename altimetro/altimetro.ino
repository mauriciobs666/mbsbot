
// (c) 2017 MBS - Mauricio Bieze Stefani

#include <Wire.h>

#include "circular.h"

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

#define THRESHOLD_DECOLAGEM  1.0 // pes/s
#define THRESHOLD_QUEDA     -1.0 // pes/s
#define THRESHOLD_ABERTURA  -0.5 // pes/s

#define AVISO_SUBIDA_CINTO 1500

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

/*
typedef struct Ponto
{
    unsigned long clock;
    double altura;
};

typedef struct Salto
{
    int offset;
    int tSubida;
    int tQueda;
    int tNavegacao;
    int ps;
};
*/

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
        TonePlayer()
        {
            limpa();
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

        void limpa()
        {
            leitura = escrita = 0;
            tocando = 0;
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

Aviso avisosAlta[4], avisosBaixa[4], avisosSubida;

Estado estado = SOLO;

CircularStats<double,15> circular15;
CircularStats<double,50> circular1s;

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
        circular15.insere( alti );
    }

    varianciaT0 = circular1s.variancia();

    altitudePS = altitudeT0 = altitudeDZ = altitudeAgora = altitudeAntes = circular1s.media();

    tonePlayer.insere( 250, 5000, 10 );

    avisosAlta[0].altura = AVISO_ALTA_1;
    avisosAlta[1].altura = AVISO_ALTA_2;
    avisosAlta[2].altura = AVISO_ALTA_3;
    avisosAlta[3].altura = ALARME_ALTA;

    avisosAlta[0].tipo = Aviso::BIP_UNICO;
    avisosAlta[1].tipo = Aviso::BIP_DUPLO;
    avisosAlta[2].tipo = Aviso::BIP_TRIPLO;
    avisosAlta[3].tipo = Aviso::SIRENE;

    avisosAlta[0].volume = 10;
    avisosAlta[1].volume = 10;
    avisosAlta[2].volume = 10;
    avisosAlta[3].volume = 10;

    avisosBaixa[0].altura = NAVEGACAO_A;
    avisosBaixa[1].altura = NAVEGACAO_B;
    avisosBaixa[2].altura = NAVEGACAO_C;
    avisosBaixa[3].altura = NAVEGACAO_D;

    avisosBaixa[0].tipo = Aviso::BIP_TRIPLO;
    avisosBaixa[1].tipo = Aviso::BIP_DUPLO;
    avisosBaixa[2].tipo = Aviso::BIP_UNICO;
    avisosBaixa[3].tipo = Aviso::BIP_UNICO;

    avisosBaixa[0].volume = 10;
    avisosBaixa[1].volume = 10;
    avisosBaixa[2].volume = 10;
    avisosBaixa[3].volume = 10;

    avisosSubida.altura = AVISO_SUBIDA_CINTO;
    avisosSubida.tipo = Aviso::BIP_UNICO;
    avisosSubida.volume = 10;

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
        circular15.insere( circular1s.media() );

        tUmSegundo = agora + 1000;
    }

    if( agora >= tUmDecimo )
    {
/*
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
*/
        tUmDecimo = agora + 100;
    }

    switch( estado )
    {
    case SOLO:
        if( velocidadeAgora > THRESHOLD_DECOLAGEM )
        {
            tonePlayer.insere( 250, 5000, 10 );

            estado = CLIMB;
        }
        else
        {
            altitudeDZ = altitudePS = *circular15.topo();
        }
        break;
    case CLIMB:
        if( velocidadeAgora < THRESHOLD_QUEDA )
        {
            tonePlayer.insere( 250, 5000, 10 );
            tonePlayer.insere( 250 );
            tonePlayer.insere( 250, 5000, 10 );

            estado = QUEDA;
        }
        else
        {
            altitudePS = *circular15.topo();
        }
        break;
    case QUEDA:
        if( velocidadeAgora > THRESHOLD_ABERTURA )
        {
            tonePlayer.insere( 250 );
            tonePlayer.insere( 250, 5000, 10 );
            tonePlayer.insere( 250 );
            tonePlayer.insere( 250, 5000, 10 );
            tonePlayer.insere( 250 );
            tonePlayer.insere( 250, 5000, 10 );

            estado = NAVEGACAO;
        }
        break;
    case NAVEGACAO:
        break;
    }

    // player

    tonePlayer.loop( agora );

    // trace

    if( trace )
    {
//        Serial.print( deltaT * 1000 );
//        Serial.print( " " );
//        Serial.print( agora - inicio );
//        Serial.print( " " );
        Serial.print( altitudeAgora - altitudeDZ, 2 );
        Serial.print( " " );
        Serial.print( circular15.media() - altitudeDZ, 2  );
        Serial.print( " " );
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
