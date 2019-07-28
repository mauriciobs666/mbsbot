
// ==========================================
// (c) 2017-2019 MBS - Mauricio Bieze Stefani
// ==========================================

#include <math.h>
#include <Wire.h>
#include <EEPROM.h>

#include "placa.h"

#include "circular.hpp"

#include "tocador.hpp"
TocadorToneAC tocadorToneAC;

#include "led.hpp"

#include "sensor.hpp"

#ifdef PINO_LED_R
Led ledR;
#endif

#ifdef PINO_LED_G
Led ledG;
#endif

#ifdef PINO_LED_B
Led ledB;
#endif

#include "energia.hpp"

#ifdef BMP180
#include <SFE_BMP180.h>
#endif

#ifdef BMP280
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#endif

#ifdef MPL3115A2
#include <SparkFunMPL3115A2.h>
MPL3115A2 altimetro;
Wire.begin();
altimetro.begin();
altimetro.setModeAltimeter();
altimetro.setOversampleRate(7);
altimetro.enableEventFlags();
#endif

#ifdef CARTAO_SD
#include <SD.h>
#endif

#include "protocolo.h"
#include "interpretador.hpp"
Interpretador interpretador;

class SensorPressao
{
    public:
        SensorPressao() : altitude(0), temperatura(0), pressao(0), inicializado(false)
        {
        }
        double altitude;
        double temperatura;
        double pressao;

        bool setup() { return true; }
        int refresh() { return 0; }
    protected:
        bool inicializado;
};

#ifdef BMP180
class SensorPressaoBMP180 : public SensorPressao
{
private:
    SFE_BMP180 barometro;
public:
    bool setup()
    {
        inicializado = barometro.begin();
        return inicializado;
    }
    int refresh()
    {
        if( ! inicializado )
        {
            altitude = temperatura = pressao = 0;
            return 0;
        }

        temperatura = 0;

        char atraso = barometro.startTemperature();

        if( atraso > 0 )
        {
            delay( atraso );

            barometro.getTemperature( temperatura );

            atraso = barometro.startPressure( 3 ); // oversampling 0-3

            if( atraso > 0 )
            {
                delay( atraso );

                atraso = barometro.getPressure( pressao, temperatura );

        //        pressao -= 2.9; // offset erro sensor

                if( atraso != 0 )
                {
                    delay( atraso );

                    // MSL Mean Sea Level
                    altitude = barometro.altitude( pressao, 1013.25 ) * 3.28084;

                    return( altitude );
                }
            }
        }
        return -666;
    }
};
#endif // BMP180

#ifdef BMP280
class SensorPressaoBMP280 : public SensorPressao
{
private:
    Adafruit_BMP280 barometro;
public:
    bool setup()
    {
        inicializado = barometro.begin( BMP280_ADDRESS_ALT );
        if( inicializado )
        {
            barometro.setSampling( Adafruit_BMP280::MODE_NORMAL,
                                   Adafruit_BMP280::SAMPLING_X16,
                                   Adafruit_BMP280::SAMPLING_X16,
                                   Adafruit_BMP280::FILTER_X16 );
        }
        return inicializado;
    }
    int refresh()
    {
        if( ! inicializado )
        {
            altitude = temperatura = pressao = 0;
            return 0;
        }

        temperatura = barometro.readTemperature();
        pressao = barometro.readPressure();
        altitude = barometro.readAltitude( 1013.25 ) * 3.28084;
//        delay(20);
        return 0;
    }
};
#endif // BMP280

#ifdef BMP280
    SensorPressaoBMP280 sensor;
    #ifdef BMP180
        #define SENSOR2
        SensorPressaoBMP180 sensor2;
    #endif
#elif defined(BMP180)
    SensorPressaoBMP180 sensor;
#endif

#ifdef ARDUINO_ARCH_ESP32
SensorPressao sensor;
#endif // ARDUINO_ARCH_ESP32

class Eeprom
{
    int tamanhoBufSaltos;
public:
    struct Configuracao
    {
        int trace;
        int delayTrace;
        double alpha;
        double beta;
        //double gama;
        double bateriaOffset;
        double bateriaSlope;
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
            SERIALX.print( " dz=" );
            SERIALX.print( altitudeDZ );
            SERIALX.print( "msl up=" );
            SERIALX.print( tempoSubida );
            SERIALX.print( "s ps=" );
            SERIALX.print( alturaSaida );
            SERIALX.print( "agl ql=" );
            SERIALX.print( tempoQueda );
            SERIALX.print( "s max=" );
            SERIALX.print( (int) (velocidadeMaxQueda * 1.09728) );
            SERIALX.print( "km/h cmd=" );
            SERIALX.print( alturaAbertura );
            SERIALX.print( "agl nav=" );
            SERIALX.print( tempoNavegacao );
            SERIALX.print( "s max=" );
            SERIALX.print( (int) ( velocidadeMaxNavegacao * 1.09728 ) );
            SERIALX.println( "km/h" );
        }

        void limpa()
        {
            numero = altitudeDZ = tempoSubida = alturaSaida = tempoQueda = 0;
            velocidadeMaxQueda = alturaAbertura = tempoNavegacao = velocidadeMaxNavegacao = 0;
        }
    };

    int setup()
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

        return 0;
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

        return 0;
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

        return 0;
    }

    int limpa( int nProximoSalto )
    {
        struct Caderneta tmp;

        tmp.limpa();
        tmp.numero = -nProximoSalto;

        EEPROM.put( sizeof(dados), tmp );

        return 0;
    }

    int carrega()
    {
        unsigned int addr = 0;
        #ifndef ARDUINO_ARCH_ESP32
        char * dest = (char*) &dados;
        for( ; addr < sizeof(dados); addr++, dest++ )
            *dest = eeprom_read_byte(( unsigned char * ) addr );
        #endif // ARDUINO_ARCH_ESP32
        return addr;
    }

    void salva()
    {
        #ifndef ARDUINO_ARCH_ESP32
        char * dest = (char*) &dados;
        for( unsigned int addr = 0; addr < sizeof(dados); addr++, dest++ )
            eeprom_write_byte(( unsigned char  *) addr, *dest);
        #endif
    }

    void defaults()
    {
        dados.trace = TRACE;
        dados.delayTrace = DFT_DELAY_TRACE;
        dados.alpha = 0.1;
        dados.beta = 0.001;
        dados.bateriaOffset = 0;
        dados.bateriaSlope = 1;
    }
}
eeprom;

class Ponto
{
public:
    Ponto(  unsigned long t = 0,
            double a = 0,
            double v = 0 )
            : timestamp( t ),
              altitude( a ),
              velocidade( v )
    {
    }

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
};

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
//    int altura,           char estado,        enum tipo,              char volume,        ul atingido
    { AVISO_SUBIDA_CINTO,   ESTADO_SUBIDA,      Aviso::BIP_UNICO,       VOLUME_NAVEGACAO,   0 },
    { AVISO_SUBIDA_CHECK,   ESTADO_SUBIDA,      Aviso::BIP_DUPLO,       VOLUME_NAVEGACAO,   0 },
    { AVISO_ALTA_1,         ESTADO_QUEDA,       Aviso::SIRENE_UNICO,    VOLUME_QUEDA,       0 },
    { AVISO_ALTA_2,         ESTADO_QUEDA,       Aviso::SIRENE_DUPLO,    VOLUME_QUEDA,       0 },
    { AVISO_ALTA_3,         ESTADO_QUEDA,       Aviso::SIRENE_TRIPLO,   VOLUME_QUEDA,       0 },
    { ALARME_ALTA,          ESTADO_QUEDA,       Aviso::SIRENE,          VOLUME_QUEDA,       0 },
    { AVISO_NAVEGACAO_A,    ESTADO_NAVEGACAO,   Aviso::BIP_TRIPLO,      VOLUME_NAVEGACAO,   0 },
    { AVISO_NAVEGACAO_B,    ESTADO_NAVEGACAO,   Aviso::BIP_DUPLO,       VOLUME_NAVEGACAO,   0 },
    { AVISO_NAVEGACAO_C,    ESTADO_NAVEGACAO,   Aviso::BIP_UNICO,       VOLUME_NAVEGACAO,   0 },
    { AVISO_NAVEGACAO_D,    ESTADO_NAVEGACAO,   Aviso::BIP_UNICO,       VOLUME_NAVEGACAO,   0 }
};

const unsigned int nAvisos = ( sizeof(avisos)/sizeof(avisos[0]) );

CircularStats<int,10> circularAltitude;        // medias dos ultimos 10 segundos

unsigned long tUmSegundo = 0;
unsigned long tTrace = 0;
int contadorLoop = 0;
int loopsSeg = 0;

class Salto
{
public:
    char estado;

    Ponto decolagem;    // area de pouso, DZ
    Ponto saida;
    Ponto abertura;
    Ponto pouso;

    Eeprom::Caderneta anotacao;

    int processaPonto( Ponto &agora )
    {
        if( ESTADO_DZ == estado )
            processaDropzone( agora );
        else if( ESTADO_SUBIDA == estado )
            processaSubida( agora );
        else if( ESTADO_QUEDA == estado )
            processaQueda( agora );
        else if( ESTADO_NAVEGACAO == estado )
            processaNavegacao( agora );
        return 0;
    }

    int processaDropzone( Ponto &agora )
    {
        decolagem.altitude = *circularAltitude.topo();

        if( agora.velocidade > THRESHOLD_DECOLAGEM )
        {
            decolagem.timestamp = agora.timestamp;

            saida = abertura = pouso = decolagem;

            // reset alarmes
            for( unsigned int i = 0; i < nAvisos ; i++ )
                avisos[i].atingido = 0;

            trocaEstado( ESTADO_SUBIDA );
        }
        else if( agora.velocidade < THRESHOLD_QUEDA )
        {
            // gambiarra caso estado esteja cagado (resiliencia)
            saida = agora;
            trocaEstado( ESTADO_QUEDA );
        }

        return 0;
    }

    int processaSubida( Ponto &agora)
    {
        if( agora.velocidade < THRESHOLD_QUEDA ) // inicio da queda livre
        {
            saida = agora;
            saida.altitude = *circularAltitude.topo(); // despreza variacao durante aceleracao e usa alt de 10s atras

            trocaEstado( ESTADO_QUEDA );
            debounceAbertura = 0;
        }
        else if( agora.velocidade < THRESHOLD_SUBTERMINAL ) // abertura subterminal
        {
            if( debounceAbertura == 0 )
            {
                debounceAbertura = agora.timestamp + DEBOUNCE_SUBTERMINAL;

                saida = agora;
                saida.altitude = *circularAltitude.topo(); // despreza variacao durante aceleracao e usa alt de 10s atras

                abertura = saida;
            }
            else if( agora.timestamp > debounceAbertura )
            {
                debounceVelMaxNav = agora.timestamp;

                trocaEstado( ESTADO_NAVEGACAO );
                debounceAbertura = 0;
            }
        }
        else // subindo ou estavel
        {
            debounceAbertura = 0;
        }

        return 0;
    }

    int processaQueda( Ponto &agora )
    {
        if( agora.velocidade < anotacao.velocidadeMaxQueda )
        {
            anotacao.velocidadeMaxQueda = agora.velocidade;
        }

        if( agora.velocidade > THRESHOLD_ABERTURA )
        {
            debounceVelMaxNav = agora.timestamp + DEBOUNCE_VEL_MAX_NAV;

            abertura = agora;
            trocaEstado( ESTADO_NAVEGACAO );
        }
        return 0;
    }

    int processaNavegacao( Ponto &agora )
    {
        if( agora.velocidade < anotacao.velocidadeMaxNavegacao && debounceVelMaxNav < agora.timestamp )
        {
            anotacao.velocidadeMaxNavegacao = agora.velocidade;
        }

        if( agora.velocidade < THRESHOLD_QUEDA ) // inicio da queda livre
        {
            saida = agora;
            trocaEstado( ESTADO_QUEDA );
            debouncePouso = 0;
        }
        else if( agora.velocidade > THRESHOLD_POUSO ) // pouso
        {
            if( ( agora.altitude - decolagem.altitude ) < 1000 ) // gambi: mil pes de tolerancia pra nao ter mais alarme falso de pouso
            {
                if( debouncePouso == 0 )
                {
                    debouncePouso = agora.timestamp + DEBOUNCE_POUSO;
                    pouso = agora;
                }
                else if ( debouncePouso <= agora.timestamp )
                {
                    trocaEstado( ESTADO_DZ );
                    // salto.limpa();
                    debouncePouso = 0;
                }
            }
        }
        else
        {
            debouncePouso = 0;
        }
        return 0;
    }

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

    void limpa( Ponto *ponto = NULL )
    {
        estado = ESTADO_DZ;

        decolagem.limpa();
        saida.limpa();
        abertura.limpa();
        pouso.limpa();

        if( ponto )
        {
            saida = decolagem = abertura = pouso = *ponto;
        }

        anotacao.limpa();
    }
private:
    unsigned long debounceVelMaxNav;
    unsigned long debounceAbertura;
    unsigned long debouncePouso;
}
salto;

#ifdef CARTAO_SD
class CartaoSD
{
public:
    CartaoSD() : sdOk(false)
        {}
    bool setup( int pinoCS )
    {
        return ( sdOk = SD.begin( pinoCS ) );
    }

    bool ls()
    {
        File dir = SD.open("/");

        for( File entry = dir.openNextFile() ;
                entry ;
                entry = dir.openNextFile() )
        {
            SERIALX.print(entry.name());

            if (entry.isDirectory())
            {
                SERIALX.println("/");
            }
            else
            {
                // files have sizes, directories do not
                Serial.print("\t\t");
                Serial.println(entry.size(), DEC);
            }
            entry.close();
        }
        return true;
    }

    bool cat( char *nomeArquivo )
    {
        head( nomeArquivo, -1 );
    }

    bool head( char *nomeArquivo, int linhas )
    {
        File arquivo = SD.open( nomeArquivo );

        if( arquivo )
        {
            while( arquivo.available() )
            {
                SERIALX.write( arquivo.read() );
            }
            arquivo.close();
        }
    }

    bool criarJmp( int numero, bool trace )
    {
        char nome[13]; // 8.3z
        const char cabecalho[] = "timestamp;sensor;altitude;altura;velocidade;pressao;temperatura;bateria;sensor2;temp2";

        bool existe = false;

        do
        {
            sprintf( nome, "jmp%05d.csv", numero );

            existe = SD.exists( nome );

            if( existe )
                numero++;
        } while( existe );

        arquivoRaw = SD.open( nome, FILE_WRITE);

        if( arquivoRaw )
        {
            sdOk = true;
            arquivoRaw.println( cabecalho );
        }
        else
        {
            sdOk = false;
        }

        if( trace )
        {
            SERIALX.print( sdOk ? "Arquivo criado: " : "Erro ao criar arquivo: " );
            SERIALX.println( nome );

            if( sdOk )
                SERIALX.println( cabecalho );
        }

        return sdOk;
    }
    bool loop( Ponto &ponto, SensorPressao &sensor, int altura, int bateria, SensorPressao &sensor2, bool trace )
    {
        char linha[50];

        if( sdOk && arquivoRaw )
        {
            snprintf( linha, 50, "%ld;%d;%d;%d;%d;%d;%d;%d;%d;%d",
                     ponto.timestamp,
                     (int)sensor.altitude,
                     (int)ponto.altitude,
                     altura,
                     (int)ponto.velocidade,
                     (int)sensor.pressao,
                     (int)sensor.temperatura,
                     (int)bateria,
                     (int)sensor2.altitude,
                     (int)sensor2.temperatura );
            arquivoRaw.println( linha );
        }
        else
        {
//            SERIALX.println( "Erro no arquivo" );
            return false;
        }

        if( trace )
            SERIALX.println( linha );

        return true;
    }

    void fechaJmp()
    {
        if( arquivoRaw )
        {
            arquivoRaw.close();
            SERIALX.println( "Arquivo fechado" );
        }
    }
private:
    bool sdOk;
    File arquivoRaw;
}
cartao;
#endif // CARTAO_SD

class Altimetro
{
public:
    Ponto agora;

    int setup( Ponto &pontoSetup )
    {
        agora = antes = pontoSetup;
        return 0;
    }

    int refresh( unsigned long timestamp, double altitudeSensor )
    {
        antes = agora;

        agora.timestamp = timestamp;
        double deltaT = ( (double) ( agora.timestamp - antes.timestamp ) ) / 1000.0;

        // predicao

        agora.altitude = antes.altitude + antes.velocidade * deltaT;
        agora.velocidade = antes.velocidade;

        double desvio = altitudeSensor - agora.altitude;

        agora.altitude += eeprom.dados.alpha * desvio;
        agora.velocidade += ( eeprom.dados.beta * desvio ) / deltaT;

    //    // Kalman
    //
    //    static double ganho= 0;
    //    static double p = 1;
    //    ganho = p + 0.05 / (p + variancia);
    //    altimetro.agora.altitude = altimetro.antes.altitude + ganho * ( altitude - altimetro.antes.altitude );
    //    p = ( 1 - ganho ) * p;
    //    circular1s.insere( altimetro.agora.altitude );

        return 0;
    }

    unsigned long getTimestamp()
    {
        return agora.timestamp;
    }

    double getAltitude()
    {
        return agora.altitude;
    }

    double getVelocidade()
    {
        return agora.velocidade;
    }

    int getAltura()
    {
        return int( agora.altitude - salto.decolagem.altitude );
    }
private:
    Ponto antes;
}
altimetro;

class GerenciadorEnergia
{
public:
    Voltimetro bateria;

    GerenciadorEnergia() : pinoEnergiaBT(-1)
    {
        #ifdef PINO_ENERGIA_BLUETOOTH
            pinoEnergiaBT = PINO_ENERGIA_BLUETOOTH;
            pinMode( pinoEnergiaBT, OUTPUT );
        #endif

        ligaBluetooth();

        #ifdef PINO_MONITOR_BATERIA
             bateria.setPino( PINO_MONITOR_BATERIA );
        #endif
    }
    void refresh()
    {
        bateria.refresh();
    }
    void ligaBluetooth()
    {
        energiaBluetooth = true;
        if( pinoEnergiaBT > 0 )
        {
            digitalWrite( pinoEnergiaBT, !energiaBluetooth );
        }
    }
    void desligaBluetooth()
    {
        energiaBluetooth = false;
        if( pinoEnergiaBT > 0 )
        {
            digitalWrite( pinoEnergiaBT, !energiaBluetooth );
        }
    }
    bool isBluetoothEnergizado()
    {
        return energiaBluetooth;
    }
private:
    int pinoEnergiaBT;
    bool energiaBluetooth;
}
energia;

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
    {
        SERIALX.print("vbat = ");
        SERIALX.print( energia.bateria.getVolts() );
        SERIALX.println(" V");
    }
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
        for( unsigned int i = 0;  i < nAvisos; i++ )
        {
            SERIALX.print("[");
            SERIALX.print(i);
            SERIALX.print("]");
            avisos[i].print();
            SERIALX.println();
        }
    }
#ifdef CARTAO_SD
    else if( 0 == strncmp( token, CMD_LST, TAM_TOKEN )  )
    {
        cartao.ls();
    }
    else if( 0 == strncmp( token, CMD_REC, TAM_TOKEN )  )
    {
        int temp = 1;

        // segundo token eh o numero do proximo salto
        if( getToken() == NUMERO )
        {
            getInt( &temp );
        }

        cartao.criarJmp( temp, true );
    }
    else if( 0 == strncmp( token, CMD_STOP, TAM_TOKEN )  )
    {
        cartao.fechaJmp();
    }
    else if( 0 == strncmp( token, CMD_CAT, TAM_TOKEN )  )
    {
        if( getToken() == NOME )
        {
            cartao.cat( token );
        }
        else
        {
            SERIALX.print( "cat: argumento invalido tipo=" );
            SERIALX.println( (int) tipoToken );
        }
    }
#endif // CARTAO_SD
    else if( 0 == strncmp( token, CMD_WHO, TAM_TOKEN ) )
    {
        for( int i = 0 ; i < nvars ; i++ )
        {
            SERIALX.println( var[i].nome );
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

Botao botaoPwr;
Botao botaoProx;
Botao botaoOk;

void setup()
{
    #ifdef PINO_BOTAO_POWER
        botaoPwr.setup( PINO_BOTAO_POWER );
    #endif

    #ifdef PINO_BOTAO_PROX
        botaoProx.setup( PINO_BOTAO_PROX );
    #endif

    #ifdef PINO_BOTAO_OK
        botaoOk.setup( PINO_BOTAO_OK );
    #endif

    tocadorToneAC.bipeBlk( 3200, VOLUME_NAVEGACAO, 100 );

    SERIALX.begin( SERIALX_SPD );

    eeprom.setup();

    energia.bateria.setFuncao( eeprom.dados.bateriaSlope, eeprom.dados.bateriaOffset );

    interpretador.declaraVar( VAR_INT, NOME_TRACE, &eeprom.dados.trace );
    interpretador.declaraVar( VAR_INT, NOME_T_TRC, &eeprom.dados.delayTrace );
    interpretador.declaraVar( VAR_INT, NOME_LOOPS, &loopsSeg );
    interpretador.declaraVar( VAR_INT, NOME_BAT, &energia.bateria.valorMedido );
    interpretador.declaraVar( VAR_DOUBLE, NOME_ALPHA, &eeprom.dados.alpha );
    interpretador.declaraVar( VAR_DOUBLE, NOME_BETA, &eeprom.dados.beta );
    interpretador.declaraVar( VAR_DOUBLE, NOME_BAT_OFF, &eeprom.dados.bateriaOffset );
    interpretador.declaraVar( VAR_DOUBLE, NOME_BAT_SLP, &eeprom.dados.bateriaSlope );
    interpretador.declaraVar( VAR_LONG, NOME_TIMESTAMP,  &altimetro.agora.timestamp );

    if( ! sensor.setup() )
    {
        Serial.println("Erro sensor barometrico");

        for(int x=0; x<5; x++)
        {
            #ifndef DEBUG
                tocadorToneAC.bipeBlk( 4000, 10, 200 );
                delay( 300 );
            #endif
        }
    }

    #ifdef SENSOR2
    if( ! sensor2.setup() )
    {
        Serial.println("Erro sensor 2");
    }
    #endif // SENSOR2

    for( int z = 0; z < 30; z++ )
    {
        sensor.refresh();
        circularAltitude.insere( sensor.altitude );

        #ifdef SENSOR2
        sensor2.refresh();
        #endif // SENSOR2

    }

    Ponto pontoSetup( millis(), circularAltitude.media(), 0.0 );

    salto.limpa( &pontoSetup );

    tUmSegundo = tTrace = pontoSetup.timestamp;

    tocadorToneAC.insere( 250, 2700, VOLUME_QUEDA );

    #ifdef CARTAO_SD
        cartao.setup( CARTAO_SD_PINO_SS );
    #endif

    #ifdef PINO_LED_R
        ledR.setup( PINO_LED_R );
        ledR.start( pontoSetup.timestamp, 1500, 500 );
    #endif

    #ifdef PINO_LED_G
        ledG.setup( PINO_LED_G );
        ledG.start( pontoSetup.timestamp, 666, 333 );
    #endif

    #ifdef PINO_LED_B
        ledB.setup( PINO_LED_B );
        ledB.start( pontoSetup.timestamp, 250, 250 );
    #endif

    altimetro.setup( pontoSetup );
}

int tom = 2000;

void loop()
{
    energia.refresh();

    sensor.refresh();

    #ifdef SENSOR2
    sensor2.refresh();
    #endif // SENSOR2

    unsigned long timestamp = millis();

    altimetro.refresh( timestamp, sensor.altitude );

    #ifdef PINO_BOTAO_POWER
        botaoPwr.refresh( timestamp );

        if( botaoPwr.isTrocaEstado() )
        {
            if( botaoPwr.getEstado() )
            {
                if( energia.isBluetoothEnergizado() )
                    energia.desligaBluetooth();
/*
                tom -= 100;
                if( tom < 50 )
                    tom = 50;
                tocadorToneAC.insere( 100, tom, 5 );
*/
            }
            else
                energia.ligaBluetooth();
        }
    #endif

    #ifdef PINO_BOTAO_PROX
        botaoProx.refresh( timestamp );

        if( botaoProx.isTrocaEstado() )
        {
            if( botaoProx.getEstado() )
            {
                tocadorToneAC.insere( 250, tom, 10 );
            }
        }
    #endif

    #ifdef PINO_BOTAO_OK
        botaoOk.refresh( timestamp );

        if( botaoOk.isTrocaEstado() )
        {
            if( botaoOk.getEstado() )
            {
                tom += 100;
                if( tom > 10000 )
                    tom = 10000;
                tocadorToneAC.insere( 100, tom, 5 );
            }
        }
    #endif

    contadorLoop++;

    if( timestamp >= tUmSegundo )
    {
        tUmSegundo += 1000;

        circularAltitude.insere( altimetro.getAltitude() );

        loopsSeg = contadorLoop;
        contadorLoop = 0;
    }

    salto.processaPonto( altimetro.agora );

    // avisos de altura

    int altura = altimetro.getAltura();

    for( unsigned int i = 0;  i < nAvisos; i++ )
    {
        if( 0 == avisos[i].atingido )
        {
            if( avisos[i].estado & salto.estado )
            {
                if( ( ( altura > avisos[i].altura ) && ( avisos[i].estado & (ESTADO_SUBIDA) ) )
                 || ( ( altura < avisos[i].altura ) && ( avisos[i].estado & (ESTADO_NAVEGACAO|ESTADO_QUEDA) ) ) )
                {
                    avisos[i].atingido = timestamp;
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

    tocadorToneAC.loop( timestamp );

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
            else if( c == CMD_EOL || c == '\n' )
            {
                comando[ pos ] = 0;
                pos = 0;

                interpretador.eval( comando );
            }
            else
                comando[ pos++ ] = c;
        }
    }

    if( timestamp >= tTrace )
    {
        tTrace += eeprom.dados.delayTrace;

        if( eeprom.dados.trace & TRACE_MASTER_EN )
        {
            #define TRACE_ANDROID_PREFIXO "E "
            #define TRACE_ANDROID_SEPARADOR ","
            #define TRACE_SEPARADOR " "
            #define TRACE_PRECISAO 2

            bool androidPlotterApp = ( eeprom.dados.trace & TRACE_ANDROID_PLOTTER );

            if( androidPlotterApp )
            {
                SERIALX.print( TRACE_ANDROID_PREFIXO );
            }

            bool qquerCoisa = false;

            if( eeprom.dados.trace & TRACE_ALTURA )
            {
                if( qquerCoisa ) SERIALX.print( androidPlotterApp ? TRACE_ANDROID_SEPARADOR : TRACE_SEPARADOR );
                SERIALX.print( altimetro.getAltura() );
                qquerCoisa = true;
            }

            if( eeprom.dados.trace & TRACE_ALTITUDE )
            {
                if( qquerCoisa ) SERIALX.print( androidPlotterApp ? TRACE_ANDROID_SEPARADOR : TRACE_SEPARADOR );
                SERIALX.print( altimetro.getAltitude(), TRACE_PRECISAO );
                qquerCoisa = true;
            }

            if( eeprom.dados.trace & TRACE_VELOCIDADE )
            {
                if( qquerCoisa ) SERIALX.print( androidPlotterApp ? TRACE_ANDROID_SEPARADOR : TRACE_SEPARADOR );
                SERIALX.print( altimetro.getVelocidade(), TRACE_PRECISAO  );
                qquerCoisa = true;
            }

            if( eeprom.dados.trace & TRACE_SENSOR )
            {
                if( qquerCoisa ) SERIALX.print( androidPlotterApp ? TRACE_ANDROID_SEPARADOR : TRACE_SEPARADOR );
                SERIALX.print( sensor.altitude /*- salto.decolagem.altitude*/, TRACE_PRECISAO  );
                qquerCoisa = true;
            }

            if( eeprom.dados.trace & TRACE_SENSOR2 )
            {
                if( qquerCoisa ) SERIALX.print( androidPlotterApp ? TRACE_ANDROID_SEPARADOR : TRACE_SEPARADOR );
                SERIALX.print( sensor2.altitude /*- salto.decolagem.altitude*/, TRACE_PRECISAO  );
                qquerCoisa = true;
            }

            if( eeprom.dados.trace & TRACE_BATERIA )
            {
                if( qquerCoisa ) SERIALX.print( androidPlotterApp ? TRACE_ANDROID_SEPARADOR : TRACE_SEPARADOR );
                SERIALX.print( energia.bateria.valorMedido );
                qquerCoisa = true;
            }

            if( eeprom.dados.trace & TRACE_TEMPERATURA )
            {
                if( qquerCoisa ) SERIALX.print( androidPlotterApp ? TRACE_ANDROID_SEPARADOR : TRACE_SEPARADOR );
                SERIALX.print( sensor.temperatura );
                qquerCoisa = true;
            }

        //        SERIALX.print( deltaT * 1000 );
        //        SERIALX.print( circularAltitude.media() , 2  );
        //        SERIALX.print( circularAltitude.media() - decolagem.altitude, 2  );
        //        SERIALX.print( *circularAltitudemedia.topo() - salto.decolagem.altitude, 2 );
        //        SERIALX.print( salto.saida.altitude - salto.decolagem.altitude, 2 );
        //        SERIALX.print( circular1s.media() , 2 );
        //        SERIALX.print( circular1s.media() - salto.decolagem.altitude, 2 );
        //        SERIALX.print( circular1s.desvio(), 2 );

            if( qquerCoisa )
            {
                SERIALX.print( CMD_EOL );
            }
        }
    }

    #ifdef CARTAO_SD
        cartao.loop( altimetro.agora, sensor, altura, energia.bateria.getVolts(), sensor2,
                      ( eeprom.dados.trace & TRACE_MASTER_EN ) && ( eeprom.dados.trace & TRACE_CARTAO ) );
    #endif

#ifdef PINO_LED_R
    ledR.loop( timestamp );
#endif

#ifdef PINO_LED_G
    ledG.loop( timestamp );
#endif

#ifdef PINO_LED_B
    ledB.loop( timestamp );
#endif
}
