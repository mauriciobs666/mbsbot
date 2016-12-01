
// (c) 2016 MBS - Mauricio Bieze Stefani

#include <Wire.h>
#include <SparkFunMPL3115A2.h>

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

    void incrementa( tipo & ponteiro )
    {
        ponteiro++;

        if( ponteiro == tamanho )
            ponteiro = 0;
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
        return ( escrita != leitura ) ? &circular[leitura] : NULL;
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

MPL3115A2 altimetro;

Estado estado = SOLO;

Circular<int,30> circular;
Circular<Ponto,300> datalog;

int altitudeAgora = 0;
int altitudeAntes = 0;
int altitudeOffset = 0;
int altitudeDelta = 0;
int altitudePS = 0;

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    altimetro.begin();
    altimetro.setModeAltimeter();
    altimetro.setOversampleRate(7);
    altimetro.enableEventFlags();
}

void loop()
{
    long inicio = millis();

    altitudeAntes = altitudeAgora;
    altitudeAgora = altimetro.readAltitudeFt();
    altitudeDelta = altitudeAgora - altitudeAntes;

    long agora = millis();

    Serial.print( "Altitude(ft): " );
    Serial.print( altitudeAgora, 2 );
    Serial.print( " " );
    Serial.print( agora - inicio );
    Serial.println( "ms" );

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
            circular.insere( altitudeAntes );
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
