/**	Copyright (C) 2010-2013 - Mauricio Bieze Stefani
 *	This file is part of the MBSBOT project.
 *
 *	MBSBOT is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	MBSBOT is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with MBSBOT.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
ATMEGA1280 - placa_v4.h

Sketch uses 24,804 bytes (19%) of program storage space. Maximum is 126,976 bytes.
Global variables use 1,878 bytes (22%) of dynamic memory, leaving 6,314 bytes for local variables. Maximum is 8,192 bytes.

ATMEGA328 - placa_v942.h

Sketch uses 23.490 bytes (72%) of program storage space. Maximum is 32.256 bytes.
Global variables use 1.412 bytes (68%) of dynamic memory, leaving 636 bytes for local variables. Maximum is 2.048 bytes.
*/

// ******************************************************************************
//		INCLUDEs e CONFIGURACAO
// ******************************************************************************

#include "protocolo.h"
#include "placa.h"

// ANSI
#include <stdio.h>
#include <ctype.h>
#include <string.h>

// AVR / Arduino
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <Servo.h>

// I2C / wiichuck
#ifdef WIICHUCK
    #include <Wire.h>
    #include "nunchuck_funcs.h"
#endif

// Pin Change interrupt ( http://code.google.com/p/arduino-pinchangeint/ )
#include "PinChangeInt.h"

// speaker
#ifdef PINO_BIP
#define BIP(freq, dur) tone(PINO_BIP,freq,dur)
#else
#define BIP(freq, dur)
#endif

// ******************************************************************************
//		Plin-plin
// ******************************************************************************

bool traceLF = false;

#ifdef PINO_SERVO_PAN
Servo pan;
#endif
#ifdef PINO_SERVO_TILT
Servo tilt;
#endif
#ifdef PINO_SERVO_ROLL
Servo roll;
#endif

unsigned long agora = 0;

// timeout de envios pela serial de status e sensores
unsigned long ultimoSensores = 0;
int delaySensores = -1;

unsigned long ultimoStatus = 0;
int delayStatus = -1;

bool trc = false;

enum Erros primeiroErro = SUCESSO;

char strBuffer[20];

const char ErroSucesso[]    PROGMEM = "SUCESSO";
const char ErroTamMaxCmd[]  PROGMEM = "ERRO_TAM_MAX_CMD";
const char ErroPrgInval[]   PROGMEM = "ERRO_PRG_INVALIDO";
const char ErroVarInval[]   PROGMEM = "ERRO_VAR_INVALIDA";
const char ErroInpretador[] PROGMEM = "ERRO_INTERPRETADOR";

const char* const tabErros[] PROGMEM =
{
    ErroSucesso,
    ErroTamMaxCmd,
    ErroPrgInval,
    ErroVarInval,
    ErroInpretador
};

void printErro( enum Erros err )
{
    strcpy_P( strBuffer, (char*) pgm_read_word( &tabErros[err] ) );
    SERIALX.println( strBuffer );

    if( ! primeiroErro )
        primeiroErro = err;
}

// ******************************************************************************
//		EEPROM
// ******************************************************************************

typedef struct
{
    enum eTipoSensor { SENSOR_VIRTUAL, SENSOR_ANALOGICO, SENSOR_PING, SENSOR_RC, SENSOR_DIGITAL };
    char tipo;
    char pino;
    bool invertido;
    volatile unsigned short minimo, maximo, centro;
    volatile bool autoMinMax;

    void init(  eTipoSensor tipo_ = SENSOR_VIRTUAL,
                char pino_ = -1,
                bool invertido_ = false )
    {
        tipo = tipo_;
        pino = pino_;
        invertido = invertido_;

        if(SENSOR_RC == tipo)
        {
            minimo = 1200;
            centro = 1500;
            maximo = 1800;
        }
        else if(SENSOR_ANALOGICO == tipo)
        {
            minimo = 0;
            centro = 512;
            maximo = 1023;
        }
        else
        {
            minimo = 0;
            centro = 32767;
            maximo = 65535;
        }
        autoMinMax = false;
    }
}
ConfigSensor;

typedef struct
{
    enum eTipoGamepad { TIPO_RC, TIPO_PC, TIPO_WII };
    char tipo;
    ConfigSensor X, Y, Z, R;

    void init(  eTipoGamepad t,
                char pinoX = -1,
                char pinoY = -1,
                char pinoZ = -1,
                char pinoR = -1 )
    {
        tipo = t;

        if( tipo == TIPO_RC )
        {
            X.init( ConfigSensor::SENSOR_RC, pinoX );
            Y.init( ConfigSensor::SENSOR_RC, pinoY );
            Z.init( ConfigSensor::SENSOR_RC, pinoZ );
            R.init( ConfigSensor::SENSOR_RC, pinoR );
        }
        else
        {
            X.init( ConfigSensor::SENSOR_VIRTUAL );
            Y.init( ConfigSensor::SENSOR_VIRTUAL );
            Z.init( ConfigSensor::SENSOR_VIRTUAL );
            R.init( ConfigSensor::SENSOR_VIRTUAL );
        }
    }
}
ConfigGamepad;

typedef struct
{
    enum eTipoMotor { MOTOR_SERVO, MOTOR_DC };
    char tipo;
    char pino;
    bool invertido;
    char pinoDir;
    char pinoDirN;
    int centro;      // zero
    int aceleracao;  // variacao abs de potencia aplicada => dp / eeprom.delays.motores

    void initServo( char pino_, int centro_=1500, bool inverso=false )
    {
        tipo = MOTOR_SERVO;
        pino = pino_;
        centro = centro_;
        invertido = inverso;
    }

    void initDC( char pinoPWM, char pinoDIR, char pinoDIRN=-1, int offsetZero=0, int acel=255, bool inverso=false )
    {
        tipo = MOTOR_DC;
        pino = pinoPWM;
        pinoDir = pinoDIR;
        pinoDirN = pinoDIRN;
        centro = offsetZero;
        invertido = inverso;
        aceleracao = acel;
    }
}
ConfigMotor;

typedef struct
{
    int Kp;
    int Ki;
    int Kd;
    int limiteP;
    int limiteI;
    int limiteD;
    int maxMV;
    int maxDT;
    bool zeraAcc;
}
ConfigPID;

class Eeprom
{
public:
    struct sConfiguracao
    {
        char programa;  // enum Programas
        char balanco;   // % balanco motor esq / dir
        char velMax;    // %
        char velEscala; // %
        char handBrake;

        ConfigMotor motorEsq;
        ConfigMotor motorDir;

        #ifdef RODAS_PWM_x4
            ConfigMotor motorEsqT;
            ConfigMotor motorDirT;
        #endif

        struct sDelays  // duracao (ms) de movimentos pra animacao
        {
            int ES; // intervalo de entrada/saida, leitura de sensores etc
            int motores; // intervalo de execucao entre os refresh de motores

            int debounce; // debounce de cruzamento / marcaEsq / marcaDir
        } delays;

        ConfigPID pid[PID_N];

        ConfigGamepad joyRC, joyPC;

        ConfigSensor sensores[NUM_SENSORES];
    }
    dados;
    void load()
    {
        char * dest = (char*) &dados;
        for( unsigned int addr = 0; addr < sizeof(dados); addr++, dest++ )
            *dest = eeprom_read_byte(( unsigned char * ) addr );
    }
    void save()
    {
        char * dest = (char*) &dados;
        for( unsigned int addr = 0; addr < sizeof(dados); addr++, dest++ )
            eeprom_write_byte(( unsigned char  *) addr, *dest);
    }
    void defaults()
    {
        dados.programa = DFT_PROGRAMA;
        dados.handBrake = DFT_FREIO_MAO;
        dados.velMax = DFT_VEL_MAX;
        dados.velEscala = DFT_VEL_ESCALA;
        dados.balanco = DFT_BALANCO;

        #ifndef RODAS_PWM
            dados.motorEsq.initServo( PINO_MOTOR_ESQ, MOTOR_CENTRO, MOTOR_ESQ_INV);
            dados.motorDir.initServo( PINO_MOTOR_DIR, MOTOR_CENTRO, MOTOR_DIR_INV);
        #else
            #ifdef PINO_MOTOR_ESQ_N
                dados.motorEsq.initDC( PINO_MOTOR_ESQ_PWM, PINO_MOTOR_ESQ, PINO_MOTOR_ESQ_N, MOTOR_CENTRO, MOTOR_ACEL, MOTOR_ESQ_INV );
            #else
                dados.motorEsq.initDC( PINO_MOTOR_ESQ_PWM, PINO_MOTOR_ESQ, MOTOR_CENTRO, MOTOR_ACEL, MOTOR_ESQ_INV );
            #endif

            #ifdef PINO_MOTOR_DIR_N
                dados.motorDir.initDC( PINO_MOTOR_DIR_PWM, PINO_MOTOR_DIR, PINO_MOTOR_DIR_N, MOTOR_CENTRO, MOTOR_ACEL, MOTOR_DIR_INV );
            #else
                dados.motorDir.initDC( PINO_MOTOR_DIR_PWM, PINO_MOTOR_DIR, MOTOR_CENTRO, MOTOR_ACEL, MOTOR_DIR_INV );
            #endif

            #ifdef RODAS_PWM_x4
                dados.motorEsqT.initDC( PINO_MOTOR_ESQ_T_PWM, PINO_MOTOR_ESQ_T, MOTOR_CENTRO, MOTOR_ACEL, MOTOR_E_T_INV );
                dados.motorDirT.initDC( PINO_MOTOR_DIR_T_PWM, PINO_MOTOR_DIR_T, MOTOR_CENTRO, MOTOR_ACEL, MOTOR_D_T_INV );
            #endif
        #endif

        dados.delays.ES = DFT_DELAY_ES;
        dados.delays.motores = DFT_VEL_REFRESH;
        dados.delays.debounce = DFT_PID_DEBOUNCE;

        dados.pid[ PID_CALIBRA ].Kp      =     5;
        dados.pid[ PID_CALIBRA ].Ki      =   100;
        dados.pid[ PID_CALIBRA ].Kd      =   300;
        dados.pid[ PID_CALIBRA ].limiteP =    50;
        dados.pid[ PID_CALIBRA ].limiteI =   500;
        dados.pid[ PID_CALIBRA ].limiteD =   100;
        dados.pid[ PID_CALIBRA ].maxMV   =   100;
        dados.pid[ PID_CALIBRA ].maxDT   =    30;
        dados.pid[ PID_CALIBRA ].zeraAcc =  true;

        dados.pid[ PID_CORRIDA ].Kp      = DFT_PID_P;
        dados.pid[ PID_CORRIDA ].Ki      = DFT_PID_I;
        dados.pid[ PID_CORRIDA ].Kd      = DFT_PID_D;
        dados.pid[ PID_CORRIDA ].limiteP = DFT_PID_LIM_P;
        dados.pid[ PID_CORRIDA ].limiteI = DFT_PID_LIM_I;
        dados.pid[ PID_CORRIDA ].limiteD = DFT_PID_LIM_D;
        dados.pid[ PID_CORRIDA ].maxMV   = DFT_PID_MAX_MV;
        dados.pid[ PID_CORRIDA ].maxDT   = DFT_PID_MAX_DT;
        dados.pid[ PID_CORRIDA ].zeraAcc = DFT_PID_ZACC;

        // TODO (mbs#1#): remover config de sensores hard-coded e permitir config serial

#if VERSAO_PLACA == 4
        for( int p = 0; p < NUM_SENSORES; p++ )
            dados.sensores[p].init( ConfigSensor::SENSOR_ANALOGICO, p );
#endif

#if VERSAO_PLACA == 22
        dados.sensores[0].init( ConfigSensor::SENSOR_ANALOGICO, 0 ); // bateria

        dados.sensores[1].init( ConfigSensor::SENSOR_PING, 15 );
        dados.sensores[1].minimo = 1000;
        dados.sensores[1].centro = 4000;
        dados.sensores[1].maximo = 4000;

        dados.sensores[2].init( ConfigSensor::SENSOR_PING, 16 );
        dados.sensores[2].minimo = 1000;
        dados.sensores[2].centro = 4000;
        dados.sensores[2].maximo = 4000;
#endif

#if VERSAO_PLACA == 2
        dados.sensores[1].init( ConfigSensor::SENSOR_ANALOGICO, 1, true );
        dados.sensores[1].minimo = 100;
        dados.sensores[1].maximo = 630;

        dados.sensores[2].init( ConfigSensor::SENSOR_ANALOGICO, 2, true );
        dados.sensores[2].minimo = 100;
        dados.sensores[2].maximo = 630;

        dados.sensores[3].init( ConfigSensor::SENSOR_ANALOGICO, 3, true );
        dados.sensores[3].minimo = 100;
        dados.sensores[3].maximo = 630;
#endif

        dados.joyPC.init( ConfigGamepad::TIPO_PC );

        #ifdef PINO_JOY_X
            #ifdef PINO_JOY_Y
                dados.joyRC.init( ConfigGamepad::TIPO_RC, PINO_JOY_X, PINO_JOY_Y );
                #ifdef PINO_JOY_Z
                    dados.joyRC.Z.init( ConfigSensor::SENSOR_RC, PINO_JOY_Z );
                #endif
                #ifdef PINO_JOY_R
                    dados.joyRC.R.init( ConfigSensor::SENSOR_RC, PINO_JOY_R );
                #endif
            #endif
        #endif
    }
}
eeprom;

// ******************************************************************************
//		DELAY SEM BLOCK (nao cumulativo)
// ******************************************************************************
bool delaySemBlock(unsigned long *ultimaVez, long ms)
{
    if( ms >= 0 && agora >= ( *ultimaVez + ms ) )
    {
        *ultimaVez = agora; // += ms;
        return true;
    }
    return false;
}

// ******************************************************************************
//      Vetor 2D int
// ******************************************************************************
class Vetor2i
{
public:
	int x, y;
	Vetor2i( int xx=0, int yy=0 ) : x(xx), y(yy)
        {}

	bool operator==(const Vetor2i& v) const
        { return ( ( x == v.x ) && ( y == v.y ) ); }
	bool operator!=(const Vetor2i& v) const
        { return ( ( x != v.x ) || ( y != v.y ) ); }

	void Constrain(int min_xy = -100, int max_xy = 100)
	{
	    x = constrain( x, min_xy, max_xy );
	    y = constrain( y, min_xy, max_xy );
	}
	int norma()
    {
        return (int) sqrt( x*x + y*y );
    }
    void normalizar()
    {
        int n = norma();
        *this *= 100;
        *this /= n;
    }

    Vetor2i operator+(const Vetor2i& v) const
    {
        Vetor2i n=*this;
        n.x+=v.x;
        n.y+=v.y;
        return n;
    }
    Vetor2i operator-(const Vetor2i& v) const
    {
        Vetor2i n=*this;
        n.x-=v.x;
        n.y-=v.y;
        return n;
    }
    Vetor2i operator*(int i) const
    {
        Vetor2i n=*this;
        n.x*=i;
        n.y*=i;
        return n;
    }
    Vetor2i operator/(int i) const
    {
        Vetor2i n=*this;
        n.x/=i;
        n.y/=i;
        return n;
    }

    Vetor2i& operator+=(const Vetor2i& v)
    {
        x+=v.x;
        y+=v.y;
        return *this;
    }
    Vetor2i& operator-=(const Vetor2i& v)
    {
        x-=v.x;
        y-=v.y;
        return *this;
    }
    Vetor2i& operator*=(int i)
    {
        x*=i;
        y*=i;
        return *this;
    }
    Vetor2i& operator/=(int i)
    {
        x/=i;
        y/=i;
        return *this;
    }

    void print( )
    {
        SERIALX.print( "(" );
        SERIALX.print( x );
        SERIALX.print( "," );
        SERIALX.print( y );
        SERIALX.print( ")" );
    }
};

// ******************************************************************************
//		SENSOR UNIVERSAL
// ******************************************************************************
class Sensor
{
public:
    volatile unsigned short valor, anterior;
    ConfigSensor *cfg;

    Sensor() : valor(0), anterior(0), cfg(NULL)
        {}
    void setConfig(ConfigSensor *c)
    {
        cfg=c;
        valor = anterior = cfg->centro;
    }
    void print()
    {
        SERIALX.print((int)cfg->pino);
        SERIALX.print(" ");
        SERIALX.print((int)cfg->tipo);
        SERIALX.print(" ");
        SERIALX.print((int)cfg->invertido);
        SERIALX.print(" ");
        SERIALX.print(cfg->minimo);
        SERIALX.print(" ");
        SERIALX.print(cfg->maximo);
        SERIALX.print(" ");
        SERIALX.print(cfg->centro);
        SERIALX.println("");
    }
    unsigned short getValor()
        { return valor; }
    void setValor(unsigned short v)
    {
        if( cfg )
        {
            anterior = valor;
            if( cfg->autoMinMax )
            {
                if (v < cfg->minimo) cfg->minimo = v;
                if (v > cfg->maximo) cfg->maximo = v;
            }
        }
        valor = v;
    }
    unsigned short calibrar()
    {
        refresh();
        if( cfg )
        {
            cfg->minimo = cfg->maximo = cfg->centro = valor;
            cfg->autoMinMax = true;
        }
        return valor;
    }
    bool calibrado()
        { return ( (cfg->maximo - cfg->minimo ) > 200 ); }
    void centrar()
        { if( cfg ) cfg->centro = valor; }
    Sensor& refresh()
    {
        if( cfg )
        {
            switch(cfg->tipo)
            {
                case ConfigSensor::SENSOR_ANALOGICO:
                    setValor( analogRead( cfg->pino ) );
                break;
                case ConfigSensor::SENSOR_PING:
                    // manda pulso de 2ms pro ping))) pra acionar leitura
                    pinMode( cfg->pino, OUTPUT );
                    digitalWrite( cfg->pino, LOW );
                    delayMicroseconds( 2 );
                    digitalWrite( cfg->pino, HIGH );
                    delayMicroseconds( 5 );
                    digitalWrite( cfg->pino, LOW );

                    // duracao do pulso = distancia
                    pinMode( cfg->pino, INPUT );
                    setValor( pulseIn( cfg->pino, HIGH ) );
                break;
                case ConfigSensor::SENSOR_DIGITAL:
                    setValor( digitalRead( cfg->pino ) );
                break;
                default:
                break;
            }
        }
        return *this;
    }
    bool ehMinimo(unsigned short margem = 0)
        { return ( cfg->invertido ? (cfg->maximo - valor) <= margem : (valor - cfg->minimo) <= margem ); }
    bool ehMaximo(unsigned short margem = 0)
        { return ( cfg->invertido ? (valor - cfg->minimo) <= margem : (cfg->maximo - valor) <= margem ); }
    int getPorcentoAprox(int grude=10)
    {
        long x = (long)valor - (long)cfg->centro;

        // calcula o range de 0 a +/-extremo
        long r = ( x > 0 ) ? (cfg->maximo - cfg->centro) : (cfg->centro - cfg->minimo);

        // x%
        if(r)
            x = constrain( ((x * 100) / r ) , -100, 100 );
        else
            x = 0;

        // arredonda no centro e pontas
        if( 100 + x < grude) x = -100;
        if(  abs(x) < grude) x = 0;
        if( 100 - x < grude) x = 100;

        return x;
    }
    int delta()
        { return valor - anterior; }
    bool getBool()
    {
        switch( cfg->tipo )
        {
            case ConfigSensor::SENSOR_ANALOGICO:
                if( calibrado() )
                {
                    unsigned short meio = ( ( cfg->maximo - cfg->minimo ) >> 1 ) + cfg->minimo;
                    return ( ( valor > meio ) ^ cfg->invertido );
                }
            break;

            case ConfigSensor::SENSOR_DIGITAL:
                return ( valor ^ cfg->invertido );
            break;

            default:
            break;
        }
        return false;
    }
}
sensores[NUM_SENSORES];

class Botao
{
public:
    Botao( unsigned char pino, bool invertido = true ) : debounce(0), antes(false), estado(false), trocou(false)
    {
         init( pino, invertido );
    }
    void init( unsigned char pino, bool invertido = true )
    {
        pinMode( pino, invertido ? INPUT_PULLUP : INPUT );
        cfg.init( ConfigSensor::SENSOR_DIGITAL, pino, invertido );
        s.setConfig( &cfg );
    }
    bool refresh()
    {
        s.refresh();

        bool atual = s.getBool();

        if( atual != antes )
            debounce = agora;

        if( ( agora - debounce ) > 50 )
        {
            if( estado != atual )
            {
                estado = atual;
                trocou = true;
            }
        }
        antes = atual;
        return estado;
    }
    bool getEstado()
    {
        return estado;
    }
    bool trocouEstado()
    {
        if( trocou )
        {
            trocou = false;
            return true;
        }
        return false;
    }
private:
    Sensor s;
    ConfigSensor cfg;
    unsigned long debounce;
    bool antes, estado, trocou;
}
botaoCal(37), botaoPrg(39);

// ******************************************************************************
//		GAMEPAD E R/C
// ******************************************************************************
class MbsGamePad
{
public:
    char tipo;
    Sensor x, y, z, r;
    volatile unsigned int botoesAntes, botoesAgora, botoesEdgeF, botoesEdgeR;
    MbsGamePad() : botoesAntes(0), botoesAgora(0), botoesEdgeF(0), botoesEdgeR(0)
        {}
    unsigned int refreshBotoes(unsigned int novo)
    {
        botoesAntes = botoesAgora;
        botoesEdgeR = (novo ^ botoesAntes) & novo;
        botoesEdgeF = (novo ^ botoesAntes) & ~novo;
        return botoesAgora = novo;
    }
    void setConfig( ConfigGamepad *cfg )
    {
        tipo = cfg->tipo;
        x.setConfig( &cfg->X );
        y.setConfig( &cfg->Y );
        z.setConfig( &cfg->Z );
        r.setConfig( &cfg->R );
    }
    void calibrar()
    {
        x.calibrar();
        y.calibrar();
        z.calibrar();
        r.calibrar();
    }
    void centrar()
    {
        x.centrar();
        y.centrar();
        z.centrar();
        r.centrar();
    }
};
MbsGamePad gamepad;

// ******************************************************************************
//		CONTROLADOR DE MOTORES
// ******************************************************************************
class Motor
{
public:
    ConfigMotor *cfg;

    Motor() : cfg(NULL), atual(0), ultimoAcel(0), meta(0) { }

    void init( ConfigMotor *cfgm )
    {
        cfg = cfgm;

        if( cfg->tipo == ConfigMotor::MOTOR_DC )
        {
            pinMode( cfg->pino, OUTPUT );
            analogWrite( cfg->pino, 0 );

            pinMode( cfg->pinoDir, OUTPUT );
            digitalWrite( cfg->pinoDir, 0 );

            if ( cfg->pinoDirN >= 0 )
            {
                pinMode( cfg->pinoDirN, OUTPUT);
                digitalWrite( cfg->pinoDirN, 0);
            }
        }
        else if( cfg->tipo == ConfigMotor::MOTOR_SERVO )
        {
            #ifndef RODAS_PWM
                servo.attach( cfg->pino );
            #endif
        }
        parar();
    }

    void parar()
    {
        write( ( cfg->tipo == ConfigMotor::MOTOR_SERVO ) ? cfg->centro : 0 );
    }

    void move( char potencia100 )
    {
        #ifndef RODAS_PWM
        if ( cfg->tipo == ConfigMotor::MOTOR_SERVO )
        {
            // TODO (mbs#1#): usar aceleracao e escala pro servo tb
            // servos tipicos aceitam pulsos entre 1000us e 2000us, ou seja, centro(1500us) +/- 500us
            write( cfg->invertido ? ( cfg->centro - potencia100*5 ) : ( cfg->centro + potencia100*5 ) );
            return;
        }
        #endif
        /* onde:
            potencia100 = +/- 0-100 %
            centro = pwm a partir do qual o motor comeca a se mover
        */
        if ( potencia100 )
        {
            short c = potencia100 > 0 ? cfg->centro : -cfg->centro; // c = "centro" com sinal
            short fator = 255 - cfg->centro;                        // faixa de controle (linear?)
            meta = c + ( potencia100 * fator ) / 100;               // converte % de potencia pra PWM 8 b
            meta = constrain( meta, -255, 255 );                    // range de saida: +/- centro ... 255
        }
        else
            meta = 0;
    }

    short read()
    {
        return atual;
    }

    void write(int valor)
    {
        meta = atual = valor;
    }

    void refresh( bool imediato = false )
    {
        // acelerador: v = v0 + at

        if( delaySemBlock( &ultimoAcel, eeprom.dados.delays.motores ) || imediato )
        {
            if ( meta > atual)
            {
                if( atual == 0 && cfg->centro ) // estava parado
                    atual = cfg->centro;
                else if( cfg->aceleracao )
                    atual += cfg->aceleracao;
                else
                    atual = meta;

                if( meta < atual) // passou do ponto
                    atual = meta;
            }
            else if ( meta < atual)
            {
                if( atual == 0 && cfg->centro ) // estava parado
                    atual = -cfg->centro;
                else if( cfg->aceleracao )
                    atual -= cfg->aceleracao;
                else
                    atual = meta;

                if ( meta > atual)
                    atual = meta;
            }

            if ( -cfg->centro < atual && atual < cfg->centro )
                atual = 0;
        }

        // I/O

        if( cfg->tipo == ConfigMotor::MOTOR_DC )
        {
            atual = constrain( atual, -255, 255 ); // protecao de range

            // uma ultima olhada no freio de mao
            if( eeprom.dados.handBrake ) meta = atual = 0;

            digitalWrite( cfg->pinoDir, (atual < 0) ^ cfg->invertido ? HIGH : LOW); // 1/2 ponte H

            if( cfg->pinoDirN >= 0 ) // pino de direcao invertido
            {
                if( atual ) // movendo, dirN = !dir
                    digitalWrite( cfg->pinoDirN, (atual < 0) ^ cfg->invertido ? LOW : HIGH); // outra 1/2 ponte H
                else        // freio, dirN = dir
                    digitalWrite( cfg->pinoDirN, (atual < 0) ^ cfg->invertido ? HIGH : LOW);
            }

            if( atual == 0 && cfg->pinoDirN >= 0 )  // conduz 100% pra freiar
                analogWrite( cfg->pino, 255 );
            else                                   // operacao normal
                analogWrite( cfg->pino, abs( atual ) );
        }
        #ifndef RODAS_PWM
        else // tipo == MOTOR_SERVO
        {
            if( eeprom.dados.handBrake ) meta = atual = cfg->centro;

            atual = constrain( atual, 1000, 2000 );
            servo.writeMicroseconds( atual );
        }
        #endif
    }
protected:
    #ifndef RODAS_PWM
        Servo servo;
    #endif
    short atual;
    unsigned long ultimoAcel;
    short meta;
    //char prioMeta; // TODO (Mauricio#1#): prioridade de quem setou a meta
};

class Drive
{
public:
    Sensor *sensorEsq, *sensorFre, *sensorDir;
    Motor motorEsq, motorDir;

    void parar()
    {
        motorEsq.parar();
        motorDir.parar();
    }

    void refresh( bool imediato = false )
    {
        motorEsq.refresh( imediato );
        motorDir.refresh( imediato );
    }

    void move( char esq100, char dir100 )
    {
        esq100 = map( esq100, -100, 100, -eeprom.dados.velEscala, eeprom.dados.velEscala);
        esq100 = constrain( esq100, -eeprom.dados.velMax, eeprom.dados.velMax );
        esq100 += eeprom.dados.balanco;
        motorEsq.move( esq100 );

        dir100 = map( dir100, -100, 100, -eeprom.dados.velEscala, eeprom.dados.velEscala);
        dir100 = constrain( dir100, -eeprom.dados.velMax, eeprom.dados.velMax );
        dir100 -= eeprom.dados.balanco;
        motorDir.move( dir100 );
    }

    void vetorial(Vetor2i direcao)
    {
        direcao.Constrain( /* -100, 100 */ ); // entrada em %

        /*
            as equacoes de interpolacao dos movimentos de giro a esquerda
               e a direita estao divididas em 4 quadrantes:

                      y
                      |
             Q2 (-,+) | Q1 (+,+)
            __________|__________ x
                      |
             Q3 (-,-) | Q4 (+,-)
                      |

            a ideia eh controlar os motores como se uma reta a 45o dividisse cada quadrante em 2:

            y
            | *1
            |  /
            | / *2
            |/____x

            * 1 - curva suave, ex: roda direita > 0
            * 2 - rotacao, ex: roda direita < 0
        */

        if( direcao.y >= 0)
        {
            if( direcao.x >= 0)   // Q1 (+,+)
            {
                motorEsq.move( max( direcao.x, direcao.y ) );
                motorDir.move( direcao.y - direcao.x );
            }
            else        // Q2 (-,+)
            {
                motorEsq.move( direcao.x + direcao.y );
                motorDir.move( max( -direcao.x, direcao.y ) );
            }
        }
        else // (y < 0)
        {
            if( direcao.x < 0)    // Q3(-,-)
            {

                motorEsq.move( min( direcao.x, direcao.y ) );
                motorDir.move( direcao.y - direcao.x );
            }
            else        // Q4 (+,-)
            {
                motorEsq.move( direcao.x + direcao.y );
                motorDir.move( min( -direcao.x, direcao.y ) );
            }
        }
    }

    void vetorialSensor(Vetor2i direcao)
    {
        direcao.Constrain();

        //#define TRACE
        #ifdef TRACE
            SERIALX.print(" dir");
            direcao.print();
        #endif

        Vetor2i resultante = direcao;

        if( direcao.y > 0 ) // ainda nao ha sensores atras :. soh trata sensores se estiver indo pra frente
        {
            // int angulo = 45;    // em relacao ao eixo Y / direcao "frente", sen=cos=0,707
            int seno = 70;      // %
            int cosseno = 70;   // %

            // pra evitar interferencias, atualiza um sensor de cada vez
            static char turno = 0;
            ( turno ^= 1 ) ? sensorEsq->refresh() : sensorDir->refresh() ;

            int s_esq = sensorEsq->getPorcentoAprox();
            int s_dir = sensorDir->getPorcentoAprox();;

            #ifdef TRACE
                SERIALX.print(" se=");
                SERIALX.print(s_esq);
                SERIALX.print(" sd=");
                SERIALX.print(s_dir);
            #endif

            /*        y
                ______|______ x
                 dir /|\ esq
                    / | \
                   V  |  V
                      V
                  obstaculos
            */

            Vetor2i esq( -( ( seno * s_esq ) / 100 ) , ( ( cosseno * s_esq ) / 100 ) );

            #ifdef TRACE
                SERIALX.print(" esq");
                esq.print();
            #endif

            Vetor2i dir( ( ( seno * s_dir ) / 100 ) , ( ( cosseno * s_dir ) / 100 ) );

            #ifdef TRACE
                SERIALX.print(" dir");
                dir.print();
            #endif

            Vetor2i obstaculos = esq + dir;

            #ifdef TRACE
                SERIALX.print(" obs" );
                obstaculos.print();
                SERIALX.print(" |obs| ");
                SERIALX.print(obstaculos.norma());
            #endif

            //obstaculos.normalizar();
            obstaculos.Constrain();

            #ifdef TRACE
                SERIALX.print(" obs");
                obstaculos.print();
            #endif

            resultante += obstaculos ;

            #ifdef TRACE
                SERIALX.print(" |res| ");
                SERIALX.print(resultante.norma());
            #endif

            //resultante.normalizar();

            //resultante.y = constrain( resultante.y, 0, 100 ); // never come back !
        }

        #ifdef TRACE
            SERIALX.print(" res");
            resultante.print();
        #endif

        vetorial( resultante );

        #ifdef TRACE
            SERIALX.println("\n");
        #endif

        #ifdef TRACE
            printRodas();
        #endif
        //#undef TRACE
    }

    void printRodas()
    {
        SERIALX.print( NOME_RODA_ESQ );
        SERIALX.print(" ");
        SERIALX.println( motorEsq.read() );

        SERIALX.print( NOME_RODA_DIR );
        SERIALX.print(" ");
        SERIALX.println( motorDir.read() );
    }

    void gira( char porc = 0 )
    {
        move( porc, -porc );
    }

    void giraEsq( char porc = 100 )
    {
        gira( -porc );
    }

    void giraDir( char porc = 100 )
    {
        gira( porc );
    }

    void praFrente( char porc = 100 )
    {
        move( porc, porc );
    }
}
drive;

#ifdef RODAS_PWM_x4
	Drive drive2;
#endif

// ******************************************************************************
//   LINE FOLLOWER
// ******************************************************************************

//#ifdef LINE_FOLLOWER
#define TRACE_LF

class PID
{
public:
    ConfigPID *cfg;

    long Proporcional;
    long Integral;
    long Derivada;
    long MV;
    long erro;
    long erroAnterior;
    long acumulador;
    long dE;
    long dT;
    unsigned long tEanterior;
    unsigned long tEatual;
    unsigned long tUltimoLoop;

    void zera()
    {
        Proporcional = Integral = Derivada = 0;
        MV = erro = erroAnterior = acumulador = dE = dT = 0;
        tEanterior = tEatual = tUltimoLoop = 0;
    }

    int executa()
    {
        // P

        Proporcional = constrain( ( erro * cfg->Kp ), -cfg->limiteP, cfg->limiteP );

        /* Dia 1: Kp = 10, limiteP = 200

        Proporcional = erro * cfg->Kp;

        if( Proporcional > cfg->limiteP )
            Proporcional = cfg->limiteP;
        else if( Proporcional < -cfg->limiteP )
            Proporcional = -cfg->limiteP;
        */

        // I

        if( cfg->Ki ) // zero desativa
        {
            if( erro && tUltimoLoop )
                acumulador += erro * (agora - tUltimoLoop);
            else if( cfg->zeraAcc )
                acumulador = 0;

            /* Dia 1: Ki = 15000, limiteI = 32000

            if( acumulador > cfg->limiteI )
                acumulador = cfg->limiteI;
            else if( acumulador < -cfg->limiteI )
                acumulador = -cfg->limiteI;

            Integral = acumulador / cfg->Ki;
            */

            Integral = constrain( ( acumulador / cfg->Ki ), -cfg->limiteI, cfg->limiteI );
        }
        else
        {
            Integral = 0;
            acumulador = 0;
        }

        // D

        bool erroMudou = ( erro != erroAnterior );

        if( erroMudou )
        {
            tEanterior = tEatual;
            tEatual = agora;
            dE = erro - erroAnterior;
            erroAnterior = erro;
        }

        dT = tEatual - tEanterior;

        if( ( agora - tEatual ) > cfg->maxDT )
            dT = 0;

        //if( ( agora - tEatual ) > dT )
        //    dT = agora - tEanterior;  // = agora - tEatual;

        long derivadaAntigo = Derivada;

        if( dT )
        {
            /* Dia 1: Kd = 3000 limiteD = 15000

            Derivada = constrain( ( cfg->Kd * dE ), -cfg->limiteD, cfg->limiteD );
            Derivada /= dT;
            */
            Derivada = constrain( ( ( cfg->Kd * dE ) / dT ), -cfg->limiteD, cfg->limiteD );
        }
        else
            Derivada = 0;

        long MVantigo = MV;

        MV = constrain( ( Proporcional + Integral + Derivada ), -cfg->maxMV , cfg->maxMV );

        //if( MV != MVantigo )
        if( erroMudou || derivadaAntigo != Derivada )
            print();

        tUltimoLoop = agora;
        return MV;
    }

    void print()
    {
        #ifdef TRACE_LF
        if( trc )
        {
            SERIALX.print( "E " );
            SERIALX.print( erro );
            SERIALX.print( " P " );
            SERIALX.print( Proporcional );
            SERIALX.print( " I " );
            SERIALX.print( Integral );
            SERIALX.print( " D " );
            SERIALX.print( Derivada );
            SERIALX.print( " dT " );
            SERIALX.print( dT );
            SERIALX.print( " MV " );
            SERIALX.println( MV );
        }
        #endif // TRACE_LF
    }
};

class LineFollower
{
public:

    LineFollower()
    {
        pid.cfg = NULL;
        zeraTudo();
    }

    void calibrar();

    bool calibrado()
    {
        for(int sb = 0; sb < NUM_IR_TRACK; sb++)
            if( ! sensores[ PINO_TRACK_0 + sb ].calibrado() )
                return false;
        return true;
    }

    void loop();

    void zeraTudo()
    {
        pid.zera();

        nGrupos = 0;
        tInicio = tFim = debounce = 0;
        rodaEsq = rodaDir = 0;
        marcaEsq = marcaDir = estadoLed = false;
        buscaInicioVolta = true;

        for(int sb = 0; sb < NUM_IR_TRACK; sb++)
            sensoresBool[sb] = debounceArray[sb] = false;
    }

    void iniciarCorrida()
    {
        zeraTudo();

        if( ! calibrado() )
        {
            calibrar();
            if( ! calibrado() )
            {
                SERIALX.println("Erro, nao foi possivel calibrar");
                return;
            }
        }

        pid.cfg = &eeprom.dados.pid[ PID_CORRIDA ];

        refresh();

        if( nGrupos )
        {
            trilho = grupos[0];
            eeprom.dados.programa = PRG_LINE_FOLLOW;
        }
        else
        {
            SERIALX.println("Erro, trilho nao encontrado");
            eeprom.dados.programa = DFT_PROGRAMA;
        }
    }

    PID pid;
    int rodaEsq;
    int rodaDir;
    unsigned long tInicio;
    unsigned long tFim;
    unsigned long debounce;
    bool sensoresBool[ NUM_IR_TRACK ];
    bool debounceArray[ NUM_IR_TRACK ];
    bool marcaEsq;
    bool marcaDir;
    bool buscaInicioVolta;
    bool estadoLed;

    void print()
    {
        #ifdef TRACE_LF
        if( trc )
        {
            for( int x = 0; x < NUM_IR_TRACK; x++ )
                SERIALX.print( sensoresBool[x] ? "A" : "_" );
            SERIALX.println();

            for( int ig = 0 ; ig < nGrupos ; ig++ )
                grupos[ig].print();

            pid.print();
        }
        #endif // TRACE_LF
    }

    int nGrupos;

    class Grupo
    {
    public:
        int pontoMedio;
        int pontoMin;
        int pontoMax;
        int tamanho;
        Grupo()
        {}
        bool intersecciona( Grupo &b )
        {
            // expande um sensor ( 2*s+1 ) pra cada lado
            int pmin = pontoMin > 1 ? pontoMin - 2 : pontoMin;
            int pmax = pontoMax < 2 * NUM_IR_TRACK - 1 ? pontoMax + 2 : pontoMax;

            // se minimo ou maximo de b estiverem dentro do range
            if( ( b.pontoMin >= pmin && b.pontoMin <= pmax ) || ( b.pontoMax >= pmin && b.pontoMax <= pmax ) )
                return true;

            return false;
        }
        void print()
        {
            #ifdef TRACE_LF
            if( trc )
            {
                SERIALX.print("grp m ");
                SERIALX.print((int)pontoMedio);
                SERIALX.print(" i ");
                SERIALX.print((int)pontoMin);
                SERIALX.print(" x ");
                SERIALX.print((int)pontoMax);
                SERIALX.print(" t ");
                SERIALX.print((int)tamanho);
                SERIALX.println("");
            }
            #endif
        }
    }
    grupos[NUM_IR_TRACK/2], trilho;

    void refresh()
    {
        nGrupos = 0;

        for(int sb = 0; sb < NUM_IR_TRACK; sb++)
            sensoresBool[sb] = sensores[ PINO_TRACK_0 + sb ].refresh().getBool();

        for(int s = 0; s < NUM_IR_TRACK; s++)
        {
            if( sensoresBool[s] )
            {
                Grupo grp;

                // e.g. os pesos p/ 6 sensores seriam { 1, 3, 5, 7, 9, 11 }
                grp.pontoMin = grp.pontoMax = 2*s + 1;
                grp.tamanho = 1;

                // agrupa enquanto o proximo for true
                while( ( s < NUM_IR_TRACK-1 ) && sensoresBool[ s + 1 ] )
                {
                    s++;
                    grp.pontoMax = 2*s + 1;
                    grp.tamanho++;
                }
/*
                // suaviza centro barra sensores
                if( grp.pontoMin >= 15 && grp.pontoMax <= 17 )
                {
                    grp.pontoMin = 15;
                    grp.pontoMax = 17;
                }
*/
                grp.pontoMedio = ( grp.pontoMin + grp.pontoMax ) / 2;

                grupos[ nGrupos ] = grp;
                nGrupos++;
            }
        }
    }

    int giraP( int setPoint = NUM_IR_TRACK )
    {
        refresh();

        if( nGrupos )
            trilho = grupos[0];

        pid.erro = trilho.pontoMedio - setPoint;

        pid.executa();

        drive.gira( constrain( pid.MV, -100 , 100 ) );

        drive.refresh();

        return pid.erro;
    }
}
lineFollower;

void LineFollower::loop()
{
    bool traceLF = false;

    if( tFim && agora > tFim )
    {
        tFim = 0;
        drive.parar();
        drive.refresh( true );

        SERIALX.print("Lap: ");
        SERIALX.print(int((agora-tInicio)/1000));
        SERIALX.println("s");

        // pisca LED por 2,25s
        for( int l = 0; l < 15; l ++ )
        {
            delay(50);
            digitalWrite( PINO_LED, false );
            delay(100);
            digitalWrite( PINO_LED, true );
        }

        // enganei um bobo da casca do ovo :-P
        iniciarCorrida();
        buscaInicioVolta = false;
    }

    refresh();

    if( nGrupos )
    {
        int eleito = -1;
        int distEleito = 3; // distancia maxima do trilho anterior

        for( int ig = 0 ; ig < nGrupos ; ig++ )
        {
            if( grupos[ig].tamanho < (NUM_IR_TRACK/3) ) // ignora cruzamentos e marcacoes
            {
                if( trilho.intersecciona( grupos[ig] ) ) // apenas se houver intersecao
                {
                    int distancia = abs( grupos[ig].pontoMedio - trilho.pontoMedio );

                    if( distancia < distEleito )
                    {
                        // close enough :-)
                        eleito = ig;
                        distEleito = distancia;
                    }
                }
            }
        }

        if( eleito >= 0 )
        {
            trilho = grupos[ eleito ];

            for( int ig = 0 ; ig < nGrupos ; ig++ )
            {
                if( ig != eleito )
                {
                    // minimo de 2 sensores de distancia i.e 2 * peso
                    if( ( grupos[ig].pontoMax + 2*2 ) < trilho.pontoMin )
                        marcaEsq = true;

                    if( ( trilho.pontoMax + 2*2 ) < grupos[ig].pontoMin )
                        marcaDir = true;
                }
            }
        }

//        if( nGrupos == 1 && grupos[0].tamanho < (NUM_IR_TRACK/3) )
        if( nGrupos == 1 && eleito >= 0 )
        {
            if( debounce && agora > debounce )
            {
                debounce = 0;

                int conta1s = 0;
                for( int s = 0; s < NUM_IR_TRACK; s++ )
                {
                    if( debounceArray[s] )
                    {
                        conta1s++;
                        debounceArray[s] = false;
                    }
                }

                if( ( marcaEsq && marcaDir ) || ( conta1s > ( ( NUM_IR_TRACK * 7 ) / 10 ) ) )
                {
                    #ifdef TRACE_LF
                    if( trc )
                        SERIALX.println("C");
                    #endif
                }
                else if( marcaEsq )
                {
                    #ifdef TRACE_LF
                    if( trc )
                        SERIALX.println("E");
                    #endif
                }
                else if( marcaDir )
                {
                    #ifdef TRACE_LF
                    if( trc )
                        SERIALX.println("D");
                    #endif
                    if( buscaInicioVolta )
                    {
                        tInicio = agora;
                        buscaInicioVolta = false;
                    }
                    else
                    {
                        tFim = agora + 500;
                    }
                }
                marcaEsq = marcaDir = false;

                traceLF = true;
            }
        }
        else // ( nGrupos > 1 ) || grupos[0].tamanho < (NUM_IR_TRACK/3)
        {
            traceLF = ! debounce;

            debounce = agora + eeprom.dados.delays.debounce;

            for( int s = 0; s < NUM_IR_TRACK; s++ )
                debounceArray[s] |= sensoresBool[s];
        }
    }
    else
    {
        static unsigned long piscaLed=0;
        if( delaySemBlock( &piscaLed, 500 ) )
        {
            traceLF = true;
            estadoLed = ! estadoLed;
        }
    }

    pid.erro = trilho.pontoMedio - NUM_IR_TRACK;

    pid.executa();

    rodaEsq = (pid.MV < 0) ? (100 + pid.MV) : 100;
    rodaDir = (pid.MV > 0) ? (100 - pid.MV) : 100;

    drive.move( rodaEsq , rodaDir );
    drive.refresh();

    digitalWrite( PINO_LED, estadoLed );

    #ifdef TRACE_LF
    if( trc )
    {
        if( traceLF )
        {
            traceLF = false;
            print();
        }
    }
    #endif
}

void LineFollower::calibrar()
{
    unsigned short valores[NUM_IR_TRACK];
    int num0s = 0;
    int num1s = 0;
    unsigned short minimo = 1023;
    unsigned short maximo = 0;

    pid.cfg = &eeprom.dados.pid[ PID_CALIBRA ];

    drive.parar();
    drive.refresh( true );

    // avisa que vai entrar em modo auto

    for( int l = 0; l < 15; l ++ )
    {
        delay(50);
        digitalWrite( PINO_LED, false );
        delay(100);
        digitalWrite( PINO_LED, true );
    }

    // primeira rodada de leitura

    for(int x = 0; x < NUM_IR_TRACK; x++)
    {
        valores[x] = sensores[ PINO_TRACK_0 + x ].calibrar();

        if( valores[x] < minimo ) minimo = valores[x];
        if( valores[x] > maximo ) maximo = valores[x];
    }

    // chuta threshold medio e classifica

    unsigned short meio = ( ( maximo - minimo ) >> 1 ) + minimo;

    for( int x = 0; x < NUM_IR_TRACK; x++ )
    {
        if( valores[x] > meio )
            num1s++;
        else
            num0s++;
    }

    // se tem mais trilho que brita inverte td

    for( int x = 0; x < NUM_IR_TRACK; x++ )
        sensores[PINO_TRACK_0 + x].cfg->invertido = ( num1s > num0s );

    unsigned long timeout = millis() + 3000;

    zeraTudo();

    trilho.pontoMedio = NUM_IR_TRACK; // supoe que robo ta centrado na linha

    // gira tudo pra esquerda
    do
    {
        agora = millis();
    }
    while( agora < timeout && giraP( ( 2 * (NUM_IR_TRACK-1) ) + 1 ) );

    delay( 100 );

    trilho.print();

    // gira tudo pra direita
    do
    {
        agora = millis();
    }
    while( agora < timeout && giraP( 1 ) );

    delay( 100 );

    trilho.print();

    // centra
    do
    {
        agora = millis();
        giraP( NUM_IR_TRACK );
    }
    while( agora < timeout );

    drive.parar();
    drive.refresh( true );

    digitalWrite( PINO_LED, false );
}
//#endif

// ******************************************************************************
//		Scanner IR 1D
// ******************************************************************************
#ifdef SCANNER
class Scanner
{
    #define SCANNER_STEPS 30
    #define SCANNER_ANG ( 180 / SCANNER_STEPS )
public:
    Scanner() : stepAtual(0), stepDir(1)
        {}
    bool stepUp()
    {
        if(upperBound())
            stepAtual = (SCANNER_STEPS-1);
        else
            stepAtual++;
        return upperBound();
    }
    bool stepDown()
    {
        if(lowerBound())
            stepAtual = 0;
        else
            stepAtual--;
        return lowerBound();
    }
    void refreshServo()
        {
            #ifdef PINO_SERVO_PAN
            pan.write( (SCANNER_ANG/2) + stepAtual * SCANNER_ANG );
            #endif
        }
    void chase()
    {
        if( s->refresh() > 300 ) // threshold
        {
            if(stepDir < 0)
                stepDown();
            else
                stepUp();
        }
        else // nada a vista, apenas continua na mesma direcao
            step();

        if(upperBound() || lowerBound())
            reverseDir();

        refreshServo();
    }
    void fillArray()
    {
        dataArray[stepAtual] = s->refresh(); // le sensor

        if( step() ) // calcula proxima posicao, retorna true se encheu array
        {
            reverseDir();
            /*
            SERIALX.print("RFA ");
            for(int x = 0; x < SCANNER_STEPS; x++)
            {
                SERIALX.print(dataArray[x]);
                SERIALX.print(" ");
            }
            SERIALX.println("");
            */
        }
        refreshServo();
    }
    bool step()
        { return (stepDir < 0) ? stepDown() : stepUp(); }
    void reverseDir()
        { stepDir = -stepDir; }
    bool lowerBound()
        { return stepAtual <= 0; }
    bool upperBound()
        { return stepAtual >= (SCANNER_STEPS-1); }
    void setSensor(Sensor *ss)
        { s = ss; }
private:
    Sensor *s;
    short stepAtual;
    char stepDir;
    unsigned short dataArray[SCANNER_STEPS];
}
scanner;
#endif

// ******************************************************************************
//		SERVIDOR SERIAL
// ******************************************************************************

typedef enum
{
    VAR_NULO = 0,
    VAR_CHAR,
    VAR_INT,
    VAR_LONG,
    VAR_BOOL,
    VAR_PID,
    VAR_SENSOR,
    VAR_MOTOR
}
TipoVariavel;

class Variavel
{
public:
    char nome[TAM_NOME];
    char tipo;
    char tam;
    void *dados;

    Variavel( TipoVariavel tipo_=VAR_NULO, const char *nome_=NULL, void *dados_=NULL, char tam_=1 )
    {
        declara( tipo_, nome_, dados_, tam_ );
    }

    void declara( TipoVariavel tipo_, const char *nome_, void *dados_, char tam_=1 )
    {
        tipo = tipo_;
        strncpy( nome, nome_, TAM_NOME );
        dados = dados_;
        tam = tam_;
    }
};

int compVar( const void *a, const void *b )
{
    return strncmp( ((Variavel*)a)->nome, ((Variavel*)b)->nome, TAM_NOME );
}

//#define TRACE_INTERPRETADOR
class Interpretador
{
public:
    Variavel var[NUM_VARS];
    char nvars;

    Variavel* buscaVar( const char *nome )
    {
        Variavel v( VAR_NULO, nome );
        return buscaVar( &v );
    }

    Variavel* buscaVar( const Variavel* chave )
    {
        return nvars ? ( Variavel* ) bsearch( chave, var, nvars, sizeof( Variavel ), compVar ) : NULL ;
    }

    Variavel* declaraVar( TipoVariavel tipo, char *nome, void *dados, char tam=1 )
    {
        Variavel nova( tipo, nome, dados, tam );

        #ifdef TRACE_INTERPRETADOR
        SERIALX.print( "declaraVar: " );
        SERIALX.print( nome );
        #endif // TRACE_INTERPRETADOR

        if( buscaVar( nome ) )
        {
            //#ifdef TRACE_INTERPRETADOR
            SERIALX.print( "declaraVar: ja existe " );
            SERIALX.println( nome );
            //#endif // TRACE_INTERPRETADOR
            return NULL;
        }

        if( ! ( nvars < NUM_VARS-1 ) )
        {
            //#ifdef TRACE_INTERPRETADOR
            SERIALX.println( "declaraVar: array cheio" );
            //#endif // TRACE_INTERPRETADOR

            return NULL;
        }

        int elem = 0;
        if( nvars )
        {
            // insere ordenado no array
            for( elem = nvars; elem > 0 ; elem-- )
                if( compVar( &nova, &var[elem-1] ) < 0 )
                    var[elem] = var[elem-1];
                else
                    break;
        }
        var[elem] = nova;
        nvars++;

        #ifdef TRACE_INTERPRETADOR
        SERIALX.print( "declaraVar: " );
        SERIALX.println( nome );
        SERIALX.print( " criada pos " );
        SERIALX.println( elem );
        #endif // TRACE_INTERPRETADOR

        return &var[elem];
    }

    Interpretador() : nvars(0), linha(NULL)
    {
    }

    void eval( char *lnh )
    {
        #ifdef TRACE_INTERPRETADOR
        SERIALX.print( "eval( " );
        SERIALX.print( lnh );
        SERIALX.println( " )" );
        #endif // TRACE_INTERPRETADOR

        linha = lnh;
        getToken();

        if( tipoToken == NOME && ( 0 == strncmp( token, CMD_GET, TAM_TOKEN) || 0 == strncmp( token, CMD_SET, TAM_TOKEN ) ) )
            getToken();

        long resultado = 0;
        enum Erros rc = evalAtribuicao( &resultado );

        #ifdef TRACE_INTERPRETADOR
        if( rc )
        {
            SERIALX.print( "eval( " );
            SERIALX.print( lnh );
            SERIALX.print( " ) => " );
            printErro( rc );
        }
        SERIALX.print("resultado = ");
        SERIALX.println( resultado );
        #endif // TRACE_INTERPRETADOR
    }
private:
    char *linha;
    char token[TAM_TOKEN];

    enum TipoToken
    {
        NULO=0,
        NUMERO,
        NOME,
        DELIMIT,
        STRING,
        CHAVE,
        BLOCO
    } tipoToken;

    Erros evalAtribuicao( long *resultado )
    {
        enum Erros rc = SUCESSO;

        #ifdef TRACE_INTERPRETADOR
        SERIALX.println( "evalAtribuicao" );
        #endif // TRACE_INTERPRETADOR

        if( tipoToken == NOME )
        {
            char bkpToken[TAM_TOKEN];
            strncpy( bkpToken, token, TAM_TOKEN );

            Variavel *v = buscaVar( token );

            getToken();

            if( v )
            {
                if( tipoToken == NULO )  // imprime valor da variavel
                {
                    SERIALX.print( bkpToken );
                    SERIALX.print( " " );

                    switch( v->tipo )
                    {
                    case VAR_CHAR:
                        SERIALX.println( (int) *( (char*) v->dados ) );
                        return SUCESSO;
                    case VAR_INT:
                        SERIALX.println( (int) *( (int* ) v->dados ) );
                        return SUCESSO;
                    case VAR_LONG:
                        SERIALX.println( (long) *( (long*) v->dados ) );
                        return SUCESSO;
                    case VAR_BOOL:
                        SERIALX.println( (bool) *( (bool*) v->dados ) );
                        return SUCESSO;
                    default:
                        return ERRO_INTERPRETADOR;
                    }
                }
                else if( token[0] == '=' )  // atribuicao
                {
                    getToken();
                    rc = evalExpressao( resultado );
                    if( rc )
                        return rc;

                    if( v )
                    {
                        switch( v->tipo )
                        {
                        case VAR_CHAR:
                            *( (char*) v->dados ) = *resultado;
                            return SUCESSO;
                        case VAR_INT:
                            *( (int* ) v->dados ) = *resultado;
                            return SUCESSO;
                        case VAR_LONG:
                            *( (long*) v->dados ) = *resultado;
                            return SUCESSO;
                        case VAR_BOOL:
                            *( (bool*) v->dados ) = *resultado;
                            return SUCESSO;
                        default:
                            return ERRO_INTERPRETADOR;
                        }
                    }
                    else
                        return ERRO_VAR_INVALIDA;
                }
            }
            else
            {
                /*
                devolve();
                strncpy( token, bkpToken, TAM_TOKEN );
                tipoToken = NOME;
                return evalExpressao( resultado );
                */
                return ERRO_VAR_INVALIDA;
            }
        }
        else
            return evalExpressao( resultado );

        return ERRO_INTERPRETADOR;
    }

    Erros evalExpressao( long *resultado )
    {
        #ifdef TRACE_INTERPRETADOR
        SERIALX.println( "evalExpressao" );
        #endif // TRACE_INTERPRETADOR

        long temp;
        char op;
        enum Erros rc = SUCESSO;

        rc = evalTermo( resultado );

        while( rc == SUCESSO && ( ( op = token[0] ) == '+' || op == '-' ) )
        {
            getToken();
            rc = evalTermo( &temp );
            switch( op )
            {
            case '-':
                *resultado -= temp;
                break;
            case '+':
                *resultado += temp;
                break;
            }
        }
        return rc;
    }

    Erros evalTermo( long *resultado )
    {
        // Termo -> Fator [ * Fator ] [ / Fator ]

        long temp;
        char op;
        enum Erros rc = SUCESSO;

        #ifdef TRACE_INTERPRETADOR
            SERIALX.println( "evalTermo" );
        #endif // TRACE_INTERPRETADOR

        rc = evalFator( resultado );

        while( rc == SUCESSO && ( ( op = token[0] ) == '*' || op == '/' ) )
        {
            getToken();
            rc = evalFator( &temp );
            switch( op )
            {
            case '*':
                *resultado *= temp;
                break;
            case '/':
                *resultado /= temp;
                break;
            }
        }
        return rc;
    }

    Erros evalFator( long *resultado )
    {
// Nome, Numero,
        char op = 0;
        enum Erros rc = SUCESSO;

        #ifdef TRACE_INTERPRETADOR
            SERIALX.println( "evalFator" );
        #endif // TRACE_INTERPRETADOR

        if( tipoToken == DELIMIT && token[0] == '+' || token[0] == '-' )
        {
            op = token[0];
            getToken();
        }

        // parenteses
        if( token[0] == '(' )
        {
            getToken();
            rc = evalAtribuicao( resultado );
            if( token[0] != ')' )
                return ERRO_INTERPRETADOR;
            getToken();
        }
        else
            rc = evalAtomo( resultado );

        if( op == '-' )
            *resultado = -( *resultado );

        return rc;
    }

    Erros evalAtomo( long *resultado )
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.println( "evalAtomo" );
        #endif // TRACE_INTERPRETADOR

        if( tipoToken == NUMERO )
        {
            *resultado = atol( token );
            getToken();
            return SUCESSO;
        }
        else if( tipoToken == NOME )
        {
            Variavel *v = buscaVar( token );

            getToken();

            if( v )
            {
                switch( v->tipo )
                {
                case VAR_CHAR:
                    *resultado = *( (char*) v->dados );
                    break;
                case VAR_INT:
                    *resultado = *( (int* ) v->dados );
                    break;
                case VAR_LONG:
                    *resultado = *( (long*) v->dados );
                    break;
                case VAR_BOOL:
                    *resultado = *( (bool*) v->dados );
                    break;
                default:
                    break;
                }
                return SUCESSO;
            }
        }
        return ERRO_INTERPRETADOR;
    }

    TipoToken getToken()
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "getToken " );
        #endif // TRACE_INTERPRETADOR

        tipoToken = NULO;

        char *tok = token;
        *tok = 0;

        while( *linha == ' ' && *linha ) linha++;

        if( *linha )
        {
            if( strchr("{}", *linha ) )
            {
                tipoToken = BLOCO;
                *tok++ = *linha++;
            }
            else if( strchr("+-*/=()", *linha ) )
            {
                tipoToken = DELIMIT;
                *tok++ = *linha++;
            }
            else if( isalpha( *linha ))
            {
                tipoToken = NOME;
                while( !isdelim( *linha ) ) *tok++ = *linha++;
            }
            else if( isdigit( *linha ))
            {
                tipoToken = NUMERO;
                while( !isdelim( *linha ) ) *tok++ = *linha++;
            }
        }
        *tok = 0;

        #ifdef TRACE_INTERPRETADOR
            switch( tipoToken )
            {
            case NULO:
                SERIALX.print("NULO ");
                break;
            case NUMERO:
                SERIALX.print("NUMERO ");
                break;
            case NOME:
                SERIALX.print("NOME ");
                break;
            case DELIMIT:
                SERIALX.print("DELIMIT ");
                break;
            case STRING:
                SERIALX.print("STRING ");
                break;
            case CHAVE:
                SERIALX.print("CHAVE ");
                break;
            case BLOCO:
                SERIALX.print("BLOCO ");
                break;
            }
            SERIALX.println( token );
        #endif

        return tipoToken;
    }

    void devolve()
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.println( "devolve" );
        #endif // TRACE_INTERPRETADOR
        for( char *t = token ; *t ; t++ )
            linha--;
    }

    bool isdelim( char c )
    {
        if( strchr("+-*/=()", c ) || c == 0 || c == ' ' )
            return true;
        return false;
    }
}
interpretador;

class Telnet
{
public:
    Telnet() : pos(0)
    {}

    bool recebe()
    {
        while( SERIALX.available() > 0 )
        {
            char c = SERIALX.read();

            if ( pos == MAX_CMD )
            {
                pos = 0;
                printErro( ERRO_TAM_MAX_CMD );
            }
            else if( c == CMD_EOL )
            {
                command[ pos ] = 0;
                pos = 0;
                return true;
            }
            else
                command[ pos++ ] = c;
        }
        return false;
    }

    void loop()
    {
        if( recebe() )
        {
            interpretador.eval( command );

            char *tok;

            char *pqp;	// algumas avr-libc linux naum tem strtok():
            #define STRTOK(a, b) strtok_r(a, b, &pqp)

            if((tok = STRTOK(command, " ")))					// primeiro token eh o comando/acao
            {
                if(strcmp(tok, CMD_SET) == 0)					// atribui um valor a uma variavel
                {
                    if((tok = STRTOK(NULL, " =")))				// segundo token eh o nome da variavel
                    {
                        char dest[10];
                        strcpy(dest,tok);

                        if((tok = STRTOK(NULL, " ")))			// terceiro token eh o valor ou indice a ser atribuido
                        {
                            int valor = atoi(tok);

                            if(strcmp(dest, NOME_RODA_ESQ) == 0)
                                drive.motorEsq.move(valor);
                            else if(strcmp(dest, NOME_RODA_DIR) == 0)
                                drive.motorDir.move(valor);
                            else if(strcmp(dest, NOME_PROGRAMA) == 0)
                            {
                                drive.parar();
                                eeprom.dados.programa = valor;
                            }
                            #ifdef PINO_SERVO_PAN
                            else if(strcmp(dest, NOME_SERVO_X) == 0)
                                pan.write(valor);
                            #endif
                            #ifdef PINO_SERVO_TILT
                            else if(strcmp(dest, NOME_SERVO_Y) == 0)
                                tilt.write(valor);
                            #endif
                            #ifdef PINO_SERVO_ROLL
                            else if(strcmp(dest, NOME_SERVO_Z) == 0)
                                roll.write(valor);
                            #endif
                        }
                    }
                }
                else if(strcmp(tok, CMD_GET) == 0)	// le uma variavel
                {
                    if((tok = STRTOK(NULL, " ")))	// segundo token eh o nome da variavel a ser lida
                    {
                        if(strcmp(tok, NOME_RODA_ESQ) == 0)
                        {
                            SERIALX.print(tok);          // ecoa nome da variavel
                            SERIALX.print(" ");
                            SERIALX.println(drive.motorEsq.read());
                        }
                        else if(strcmp(tok, NOME_RODA_DIR) == 0)
                        {
                            SERIALX.print(tok);          // ecoa nome da variavel
                            SERIALX.print(" ");
                            SERIALX.println(drive.motorDir.read());
                        }
                        #ifdef PINO_SERVO_PAN
                        else if(strcmp(tok, NOME_SERVO_X) == 0)
                        {
                            SERIALX.print(tok);          // ecoa nome da variavel
                            SERIALX.print(" ");
                            SERIALX.println(pan.read());
                        }
                        #endif
                        #ifdef PINO_SERVO_TILT
                        else if(strcmp(tok, NOME_SERVO_Y) == 0)
                        {
                            SERIALX.print(tok);          // ecoa nome da variavel
                            SERIALX.print(" ");
                            SERIALX.println(tilt.read());
                        }
                        #endif
                        #ifdef PINO_SERVO_ROLL
                        else if(strcmp(tok, NOME_SERVO_Z) == 0)
                        {
                            SERIALX.print(tok);          // ecoa nome da variavel
                            SERIALX.print(" ");
                            SERIALX.println(roll.read());
                        }
                        #endif
                        else if(strcmp(tok, NOME_AS) == 0)
                            enviaSensores(true);
                    }
                }
                else if(strcmp(tok, CMD_GRAVA) == 0)	// salva temporarios na EEPROM
                    eeprom.save();
                else if(strcmp(tok, CMD_CARREGA) == 0)	// descarta mudancas e recarrega da EEPROM
                    eeprom.load();
                else if(strcmp(tok, CMD_DEFAULT) == 0)  // hard-coded
                    eeprom.defaults();
                else if(strcmp(tok, CMD_CAL) == 0)	    // entra no modo autocalibracao de joystick
                    gamepad.calibrar();
                else if(strcmp(tok, CMD_CENTRO) == 0)	// sai do modo de autocalibracao e centra joystick
                    gamepad.centrar();
                #ifdef LINE_FOLLOWER
                else if(strcmp(tok, CMD_LF_CAL) == 0)	// re-calibra sensores do line follower
                    lineFollower.calibrar();
                #endif
                else if(strcmp(tok, CMD_MV_PARAR) == 0)
                {
                    drive.parar();
					#ifdef RODAS_PWM_x4
						drive2.parar();
					#endif
                    eeprom.dados.programa = PRG_RC_SERIAL;
                }
                else if(strcmp(tok, CMD_MV_RODAS) == 0)
                {
                    tok = STRTOK(NULL, " ");
                    if (tok)			// second token is the left wheel power percent
                    {
                        int lw = atoi(tok);
                        tok = STRTOK(NULL, " ");
                        if (tok)		// third token is the right wheel power percent
                        {
                            drive.motorEsq.move(lw);
                            drive.motorDir.move(atoi(tok));
                        }
                    }
                }
                else if(strcmp(tok, CMD_MV_VET) == 0)
                {
                    eeprom.dados.programa = PRG_RC_SERIAL;
                    Vetor2i direcao;

                    if ((tok = STRTOK(NULL, " ")))			// segundo token eh o percentual de potencia p/ eixo X
                    {
                        direcao.x = atoi(tok);
                        if ((tok = STRTOK(NULL, " ")))		// terceiro token eh o percentual de potencia p/ eixo Y
                        {
                            direcao.y = atoi(tok);
                            drive.vetorial( direcao );
							#ifdef RODAS_PWM_x4
								if ((tok = STRTOK(NULL, " ")))  // quarto token eh o percentual de potencia p/ eixo X
								{
									direcao.x = atoi(tok);
									if ((tok = STRTOK(NULL, " ")))// quinto token eh o percentual de potencia p/ eixo Y
									{
										direcao.y = atoi(tok);
										drive2.vetorial( direcao );
									}
								}
							#endif
						}
                    }
                }
                else if(strcmp(tok, CMD_STATUS) == 0)
                    enviaStatus();
                else if(strcmp(tok, CMD_UNAME) == 0)
                    uname();
                else if(strcmp(tok, CMD_BIP) == 0)
                {
                    #ifdef PINO_BIP
                        tok = STRTOK(NULL, " ");
                        if (tok)			        // frequencia
                        {
                            int hz = atoi(tok);
                            tok = STRTOK(NULL, " ");
                            if (tok)                // duracao
                                BIP(hz, atoi(tok));
                            else
                                BIP(hz, 200);
                        }
                    #endif
                }
                else if(strcmp(tok, CMD_LIMPA_ERRO) == 0)
                {
                    primeiroErro = SUCESSO;
                }
                else if(strcmp(tok, CMD_JOYPAD) == 0)
                {
                    if ((tok = STRTOK(NULL, " ")))			        // segundo token eh o status dos botoes
                    {
                        gamepad.refreshBotoes(atoi(tok));
                        if ((tok = STRTOK(NULL, " ")))		        // terceiro token eh o eixo X
                        {
                            if( gamepad.tipo != ConfigGamepad::TIPO_PC  )
                            {
                                gamepad.setConfig( &eeprom.dados.joyPC );
                            }
                            gamepad.x.setValor(atol(tok));
                            if ((tok = STRTOK(NULL, " ")))	        // quarto token eh o eixo Y
                            {
                                gamepad.y.setValor(atol(tok));
                                if ((tok = STRTOK(NULL, " ")))		// quinto token eh o eixo Z
                                {
                                    gamepad.z.setValor(atol(tok));
                                    if ((tok = STRTOK(NULL, " ")))	// sexto token eh o eixo Rudder
                                    {
                                        gamepad.r.setValor(atol(tok));
                                    }
                                }
                            }
                        }
                    }
                    else
                        enviaJoystick();
                }
            }
        }
    }

    void enviaSensores(bool enviaComando = true)
    {
        if(enviaComando)
        {
            SERIALX.print( NOME_AS );
            SERIALX.print( " " );
        }
        for (int x = 0; x < NUM_SENSORES; x++)
        {
            SERIALX.print( sensores[x].refresh().getValor() );
            SERIALX.print( " " );
        }
        SERIALX.println( "" );
/*
        for( int x = 0; x < NUM_IR_TRACK; x++ )
            SERIALX.print( sensores[PINO_TRACK_0 + x].getBool() ? "|" : "_" );
        SERIALX.println("");
*/
    }

    void enviaStatus(bool enviaComando = true)
    {
        if(enviaComando)
        {
            SERIALX.print(CMD_STATUS);
            SERIALX.print(" ");
        }
        SERIALX.print((int)eeprom.dados.programa);
        SERIALX.print(" ");
        SERIALX.print((int)primeiroErro);
        SERIALX.print(" ");
        SERIALX.print((int)eeprom.dados.handBrake);
        SERIALX.print(" ");
        SERIALX.print(drive.motorEsq.read());
        SERIALX.print(" ");
        SERIALX.print(drive.motorDir.read());
		#ifdef RODAS_PWM_x4
            SERIALX.print(" ");
            SERIALX.print(drive2.motorEsq.read());
            SERIALX.print(" ");
            SERIALX.print(drive2.motorDir.read());
		#endif
        #ifdef PINO_SERVO_PAN
            SERIALX.print(" ");
            SERIALX.print(pan.read());
        #endif
        #ifdef PINO_SERVO_TILT
            SERIALX.print(" ");
            SERIALX.print(tilt.read());
        #endif
        #ifdef PINO_SERVO_ROLL
            SERIALX.print(" ");
            SERIALX.print(roll.read());
        #endif
        SERIALX.println(" ");
    }

    void enviaJoystick()
    {
        SERIALX.print(CMD_JOYPAD);
        SERIALX.print(" X ");
        SERIALX.print(gamepad.x.getPorcentoAprox(0));
        SERIALX.print(" ~ ");
        SERIALX.print(gamepad.x.getPorcentoAprox());

        SERIALX.print(" (");
        SERIALX.print(gamepad.x.cfg->minimo);
        SERIALX.print(",");
        SERIALX.print(gamepad.x.cfg->centro);
        SERIALX.print(",");
        SERIALX.print(gamepad.x.cfg->maximo);
        SERIALX.print(") ");
        SERIALX.println(gamepad.x.valor);

        SERIALX.print(CMD_JOYPAD);
        SERIALX.print(" Y ");
        SERIALX.print(gamepad.y.getPorcentoAprox(0));
        SERIALX.print(" ~ ");
        SERIALX.print(gamepad.y.getPorcentoAprox());

        SERIALX.print(" (");
        SERIALX.print(gamepad.y.cfg->minimo);
        SERIALX.print(",");
        SERIALX.print(gamepad.y.cfg->centro);
        SERIALX.print(",");
        SERIALX.print(gamepad.y.cfg->maximo);
        SERIALX.print(") ");
        SERIALX.println(gamepad.y.valor);
    /*
        SERIALX.print(" Z ");
        SERIALX.print(gamepad.z.getPorcentoAprox(0));
        SERIALX.print(" R ");
        SERIALX.println(gamepad.r.getPorcentoAprox(0));
    */
    }

    void uname()
    {
        SERIALX.print("MBSBOT hw ");
        SERIALX.print(VERSAO_PLACA);
        SERIALX.print(" sw ");
        SERIALX.println(VERSAO_PROTOCOLO);
    }

private:
    char command[MAX_CMD];
    unsigned char pos;
}
telnet;

// ******************************************************************************
//		INTS DE R/C - http://code.google.com/p/arduino-pinchangeint/wiki/Usage
// ******************************************************************************

void isrRadioEixo(class Sensor *s, unsigned long *inicioPulso)
{
    if( !s || !s->cfg || s->cfg->tipo != ConfigSensor::SENSOR_RC )
        return;

    if(PCintPort::pinState == HIGH)
        *inicioPulso = micros();
    else
    {
        if(*inicioPulso)
        {
            unsigned long duracao = micros() - *inicioPulso;
            if( duracao > 1000 && duracao < 2000 )
                s->setValor((unsigned short)duracao);
            *inicioPulso = 0;
        }
    }
}

void isrRadio()
{
    if( eeprom.dados.programa == PRG_RC )
    switch(PCintPort::arduinoPin)
    {
        #ifdef PINO_JOY_X
        case PINO_JOY_X:
        {
            static unsigned long inicioPulsoX = 0;
            isrRadioEixo(&gamepad.x, &inicioPulsoX);
        }
        break;
        #endif
        #ifdef PINO_JOY_Y
        case PINO_JOY_Y:
        {
            static unsigned long inicioPulsoY = 0;
            isrRadioEixo(&gamepad.y, &inicioPulsoY);
        }
        break;
        #endif
        #ifdef PINO_JOY_Z
        case PINO_JOY_Z:
        {
            static unsigned long inicioPulsoZ = 0;
            isrRadioEixo(&gamepad.z, &inicioPulsoZ);
        }
        break;
        #endif
        #ifdef PINO_JOY_R
        case PINO_JOY_R:
        {
            static unsigned long inicioPulsoR = 0;
            isrRadioEixo(&gamepad.r, &inicioPulsoR);
        }
        break;
        #endif
        #ifdef PINO_JOY_SW1
        case PINO_JOY_SW1:
        {
            static unsigned long inicioPulsoSW1 = 0;
            if(PCintPort::pinState == HIGH)
                inicioPulsoSW1 = micros();
            else
            {
                if(inicioPulsoSW1)
                {
                    unsigned long duracao = micros() - inicioPulsoSW1;
                    gamepad.refreshBotoes((duracao < 1500) ? BT_RB : 0);
                    inicioPulsoSW1 = 0;
                }
            }
        }
        break;
        #endif
        default:
        break;
    }
}

void trataJoystick()
{
    if(gamepad.botoesEdgeR & BT_L3)
    {
        gamepad.calibrar();
        eeprom.dados.handBrake = 1;
        eeprom.dados.programa = PRG_RC_SERIAL;
    }

    if( (gamepad.botoesEdgeF & BT_A) || (gamepad.botoesEdgeF & BT_B) )
    {
        drive.parar();
		#ifdef RODAS_PWM_x4
			drive2.parar();
		#endif

        // auto centra joystick
        gamepad.centrar();

        eeprom.dados.handBrake = (gamepad.botoesEdgeF & BT_B);

        // poe no modo RC
        eeprom.dados.programa = PRG_RC_SERIAL;
        //eeprom.dados.velEscala = 60;
    }

    if(gamepad.botoesEdgeF & BT_X)
    {
        eeprom.dados.handBrake = 0;
        lineFollower.calibrar();
    }

    if(gamepad.botoesEdgeF & BT_Y)
    {
        eeprom.dados.handBrake = 0;
        lineFollower.iniciarCorrida();
    }
/*
    if(gamepad.botoesAgora & BT_LT)
    {
        // 4 rodas: controla servos com eixos Z e R
    }

    if(gamepad.botoesAgora & BT_RT)
    {
        eeprom.dados.handBrake = 0;
#ifdef PINO_ARMA
        digitalWrite(PINO_ARMA, HIGH);
    }
    else
    {
        digitalWrite(PINO_ARMA, LOW);
#endif
    }
*/
    if(gamepad.botoesEdgeF & BT_LB)
    {
        if( eeprom.dados.velEscala > 30 )
            eeprom.dados.velEscala -= 10;

        if( eeprom.dados.velEscala < 30 )
            eeprom.dados.velEscala = 30;
    }

    if(gamepad.botoesEdgeF & BT_RB)
    {
        if( eeprom.dados.velEscala < 100 )
            eeprom.dados.velEscala += 10;

        if( eeprom.dados.velEscala > 100 )
            eeprom.dados.velEscala = 100;
    }

    gamepad.botoesEdgeR = 0;
    gamepad.botoesEdgeF = 0;
}

// ******************************************************************************
//		SETUP
// ******************************************************************************
void setup()
{
    SERIALX.begin(115200);
    telnet.uname();

    eeprom.load();

    SERIALX.print("EEPROM ");
    SERIALX.print(sizeof(eeprom.dados));
    SERIALX.println(" B");

    drive.motorEsq.init( &eeprom.dados.motorEsq );
    drive.motorDir.init( &eeprom.dados.motorDir );

#ifdef RODAS_PWM_x4
    drive2.motorEsq.init( &eeprom.dados.motorEsqT );
    drive2.motorDir.init( &eeprom.dados.motorDirT );
#endif

#ifdef PINO_SERVO_PAN
    pan.attach(PINO_SERVO_PAN);
    pan.write(90);
#endif

#ifdef PINO_SERVO_TILT
    tilt.attach(PINO_SERVO_TILT);
    tilt.write(90);
#endif

#ifdef PINO_SERVO_ROLL
    roll.attach(PINO_SERVO_ROLL);
    roll.write(90);
#endif

    pinMode(PINO_LED, OUTPUT);
    digitalWrite(PINO_LED, LOW);

#ifdef PINO_ARMA
    pinMode(PINO_ARMA, OUTPUT);
    digitalWrite(PINO_ARMA, LOW);
#endif

#ifdef WIICHUCK
    #ifdef WIICHUCK_POWER
        nunchuck_setpowerpins();
    #endif
    nunchuck_init();
#endif

    for(int s=0; s<NUM_SENSORES; s++)
        sensores[s].setConfig(&eeprom.dados.sensores[s]);

    drive.sensorEsq = &sensores[1];
    drive.sensorDir = &sensores[2];
    drive.sensorFre = &sensores[3];

    gamepad.setConfig( &eeprom.dados.joyRC );

#ifdef PINO_JOY_X
    PCintPort::attachInterrupt(PINO_JOY_X, &isrRadio, CHANGE);
#endif

#ifdef PINO_JOY_Y
    PCintPort::attachInterrupt(PINO_JOY_Y, &isrRadio, CHANGE);
#endif

#ifdef PINO_JOY_Z
    PCintPort::attachInterrupt(PINO_JOY_Z, &isrRadio, CHANGE);
#endif

#ifdef PINO_JOY_R
    PCintPort::attachInterrupt(PINO_JOY_R, &isrRadio, CHANGE);
#endif

#ifdef PINO_JOY_SW1
    PCintPort::attachInterrupt(PINO_JOY_SW1, &isrRadio, CHANGE);
#endif

    // liga pull-up de pinos livres pra economizar energia
    unsigned char unused[] = PINO_UNUSED_ARRAY;
    for(unsigned char p=0; p < PINO_UNUSED_CNT; p++)
    {
        pinMode(unused[p], INPUT);
        digitalWrite(unused[p], HIGH);
    }

/*    // plaquinha com 8 leds conectada no canto do mega
    for( int i = 39; i <= 53; i += 2 )
        pinMode(i, OUTPUT);
    for( int x = 0; x < NUM_IR_TRACK; x++ )
        digitalWrite( (53 - (2 * x)) , sensoresBool[x] );
*/


/*  Variavel* declaraVar( TipoVariavel tipo, char *nome, void *dados, char tam=1 )
	VAR_NULO = 0,
    VAR_CHAR,
    VAR_INT,
    VAR_LONG,
    VAR_BOOL,
    VAR_STRING
*/

    interpretador.declaraVar( VAR_CHAR, NOME_PROGRAMA,   &eeprom.dados.programa );
    interpretador.declaraVar( VAR_CHAR, NOME_BALANCO,    &eeprom.dados.balanco );
    interpretador.declaraVar( VAR_CHAR, NOME_VEL_MAX,    &eeprom.dados.velMax );
    interpretador.declaraVar( VAR_CHAR, NOME_VEL_ESCALA, &eeprom.dados.velEscala );
    interpretador.declaraVar( VAR_CHAR, NOME_FREIO,      &eeprom.dados.handBrake );

    interpretador.declaraVar( VAR_INT,  NOME_T_SE,    &delaySensores );
    interpretador.declaraVar( VAR_INT,  NOME_T_ST,    &delayStatus );
    interpretador.declaraVar( VAR_BOOL, NOME_TRACE,   &trc );

    interpretador.declaraVar( VAR_INT, NOME_T_RF,    &eeprom.dados.delays.ES );
    interpretador.declaraVar( VAR_INT, NOME_T_MOTOR, &eeprom.dados.delays.motores );
    interpretador.declaraVar( VAR_INT, NOME_T_DEB,   &eeprom.dados.delays.debounce );

    interpretador.declaraVar( VAR_INT, NOME_ZERO_ESQ, &eeprom.dados.motorEsq.centro );
    interpretador.declaraVar( VAR_INT, NOME_ACEL_ESQ, &eeprom.dados.motorEsq.aceleracao );
    interpretador.declaraVar( VAR_INT, NOME_ZERO_DIR, &eeprom.dados.motorDir.centro );
    interpretador.declaraVar( VAR_INT, NOME_ACEL_DIR, &eeprom.dados.motorDir.aceleracao );

    interpretador.declaraVar( VAR_INT, NOME_PID_KP, &eeprom.dados.pid[ PID_CORRIDA ].Kp );
    interpretador.declaraVar( VAR_INT, NOME_PID_KI, &eeprom.dados.pid[ PID_CORRIDA ].Ki );
    interpretador.declaraVar( VAR_INT, NOME_PID_KD, &eeprom.dados.pid[ PID_CORRIDA ].Kd );

    interpretador.declaraVar( VAR_INT, NOME_PID_LIM_P, &eeprom.dados.pid[ PID_CORRIDA ].limiteP );
    interpretador.declaraVar( VAR_INT, NOME_PID_LIM_I, &eeprom.dados.pid[ PID_CORRIDA ].limiteI );
    interpretador.declaraVar( VAR_INT, NOME_PID_LIM_D, &eeprom.dados.pid[ PID_CORRIDA ].limiteD );

    interpretador.declaraVar( VAR_INT,  NOME_PID_MMV, &eeprom.dados.pid[ PID_CORRIDA ].maxMV );
    interpretador.declaraVar( VAR_INT,  NOME_PID_MDT, &eeprom.dados.pid[ PID_CORRIDA ].maxDT );
    interpretador.declaraVar( VAR_BOOL, NOME_PID_ZAC, &eeprom.dados.pid[ PID_CORRIDA ].zeraAcc );

}

// ******************************************************************************
//		for(;;) ...
// ******************************************************************************
void loop()
{
    agora = millis();

    telnet.loop();

    trataJoystick();

    botaoCal.refresh();
    if( botaoCal.trocouEstado() )
    {
        if( botaoCal.getEstado() )
        {
//            SERIALX. println("[CAL] apertado");
        }
        else
        {
//            SERIALX. println("[CAL] solto");
            lineFollower.calibrar();
        }
    }

    botaoPrg.refresh();
    if( botaoPrg.trocouEstado() )
    {
        if( botaoPrg.getEstado() )
        {
//            SERIALX. println("[PRG] apertado");
        }
        else
        {
            if( eeprom.dados.programa == PRG_LINE_FOLLOW )
            {
                drive.parar();
                eeprom.dados.programa = DFT_PROGRAMA;
            }
            else
                lineFollower.iniciarCorrida();

//            SERIALX. println("[PRG] solto");
        }
    }

    if( delaySemBlock(&ultimoStatus, delayStatus) )
    {
        telnet.enviaStatus();
        //digitalWrite(PINO_LED, !digitalRead(PINO_LED));
    }

    if( delaySemBlock(&ultimoSensores, delaySensores) )
    {
        //telnet.enviaJoystick();
        telnet.enviaSensores();
        #ifdef WIICHUCK
            nunchuck_print_data();
        #endif
    }

    //#define TESTE_PERFORMANCE
    #ifdef TESTE_PERFORMANCE
    static unsigned long passagensLoop = 0;
    static unsigned long ultimoLoop = 0;
    if( delaySemBlock(&ultimoLoop, 10000) )
    {
        SERIALX.print( passagensLoop / 10 );
        SERIALX.println(" fps");
        passagensLoop = 0;
    }
    #endif

    static unsigned short msExec = 10;
    static unsigned long ultimaExec = 0;
    if( delaySemBlock(&ultimaExec, msExec) )
    {
        #ifdef TESTE_PERFORMANCE
        passagensLoop++;
        #endif
        switch(eeprom.dados.programa)
        {
        case PRG_RC:
        case PRG_RC_SERIAL:
        {
            static unsigned long piscaLed = 0;
            static int intervalo = 100;

            if( delaySemBlock( &piscaLed, intervalo ) )
            {
                if( intervalo == 100 )
                {
                    digitalWrite( PINO_LED, false );
                    intervalo = 2000;
                }
                else
                {
                    digitalWrite( PINO_LED, true );
                    intervalo = 100;
                }
            }
            #ifdef RODAS_PWM_x4
                drive.vetorial(gamepad.x.getPorcentoAprox() + gamepad.z.getPorcentoAprox(), -gamepad.y.getPorcentoAprox());
                drive2.vetorial(-gamepad.x.getPorcentoAprox() + gamepad.z.getPorcentoAprox(), -gamepad.y.getPorcentoAprox());
            #else
                Vetor2i direcao( gamepad.x.getPorcentoAprox(), -gamepad.y.getPorcentoAprox() );
//                if( gamepad.botoesAgora & BT_RT ) // arma ligada desabilita sensores
                    drive.vetorial( direcao );
//                else
//                    drive.vetorialSensor( direcao );
            #endif
        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_FOTOVORO:
        {
            const int threshold = 25;

            int ldrEsq = analogRead(0);
            int ldrDir = analogRead(1);

            // numeros menores significam mais luz

            if ( (ldrDir - ldrEsq) > threshold )		// vira pra esq
                drive.giraEsq();
            else if ( (ldrEsq - ldrDir) > threshold )	// vira pra direita
                drive.giraDir();
            else
                drive.praFrente();
        }
        msExec = eeprom.dados.delays.ES;
        break;

        #ifdef LINE_FOLLOWER
        case PRG_LINE_FOLLOW:
            lineFollower.loop();
        msExec = eeprom.dados.delays.ES;
        break;
        #endif

        #ifdef SCANNER
        case PRG_SCANNER:
            scanner.fillArray();
        msExec = eeprom.dados.delays.ES;
        break;
        case PRG_CHASE:
            scanner.chase();
        msExec = eeprom.dados.delays.ES;
        break;
        #endif

        case PRG_COLISAO:
        {
            // ignora objetos muito longe
            if( drive.sensorFre->refresh().getValor() > 200 )
            {
                // calc velocidade em unidades sensor / segundo
                int v = ( (long) drive.sensorFre->delta() * 1000 ) / eeprom.dados.delays.ES;

                // s = s0 + v * t;
                int timeToCollision = ( 700 - drive.sensorFre->getValor() ) / v;

                if(timeToCollision < 3 )
                    eeprom.dados.programa = PRG_ALARME;
            }

        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_SENTINELA:
        {
            drive.sensorFre->refresh();
            if( abs(drive.sensorFre->delta() ) > 30 )
                eeprom.dados.programa = PRG_ALARME;
        }
        msExec = eeprom.dados.delays.ES;
        break;

        #ifdef WIICHUCK
        case PRG_WIICHUCK:
        {
            // TODO (mbs#1#): limpar essa merda e usar nova classe joystick p wiichuck tb

            Vetor2i joyCenter(174,174);

            if(nunchuck_get_data() == 0)
                break;

            // nunchuck_print_data();

            if( ! nunchuck_zbutton())
            {
                int x = nunchuck_joyx() - joyCenter.x;
                int y = nunchuck_joyy() - joyCenter.y;

                // TODO: mapear 0-100 direito
                x *= 10; // joga x lah pra pqp
                y *= 10;

                drive.vetorial(x, y);
            }
            else
            {
                drive.parar();

                const int VELOCIDADE_SERVO = 2;

                #ifdef PINO_SERVO_PAN
                if( nunchuck_joyx() < joyCenter.x )
                {
                    int angle = pan.read() + VELOCIDADE_SERVO;
                    if(angle > 170) angle = 170;
                    pan.write(angle);
                }
                else if( nunchuck_joyx() > joyCenter.x )
                {
                    int angle = pan.read() - VELOCIDADE_SERVO;
                    if(angle < 10) angle = 10;
                    pan.write(angle);
                }
                #endif

                #ifdef PINO_SERVO_TILT
                if( nunchuck_joyy() > joyCenter.y )
                {
                    int angle = tilt.read() + VELOCIDADE_SERVO;
                    if(angle > 170) angle = 170;
                    tilt.write(angle);
                }
                else if( nunchuck_joyy() < joyCenter.y )
                {
                    int angle = tilt.read() - VELOCIDADE_SERVO;
                    if(angle < 10) angle = 10;
                    tilt.write(angle);
                }
                #endif
            }

            if(nunchuck_cbutton())
            {
                eeprom.dados.handBrake = 0;
                digitalWrite(PINO_ARMA,HIGH);
            }
            else
                digitalWrite(PINO_ARMA,LOW);
        }
        msExec = 100;
        break;
        #endif

        case PRG_SCOPE:
        {
            // pra conseguir performance melhor descarta os 2 bits menos significativos
            // e envia somente um byte
            SERIALX.write((analogRead(0) >> 2) & 0xFF);
        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_KNOB:
            #ifdef PINO_SERVO_PAN
            pan.write( map( analogRead(0), 0, 1023, 5, 174 ));
            #endif
        msExec = 50;
        break;

        case PRG_NAV_3S:
        {
            drive.sensorFre->refresh();
            drive.sensorEsq->refresh();
            drive.sensorDir->refresh();

            static char palpite = 0; // pra seguir girando pra um lado ateh encontrar um caminho livre

            #define MARGEM_SHARP 200
            #define MARGEM_PING 400

            if( drive.sensorFre->ehMinimo(MARGEM_PING) )
            {
//                int getReta()
//                    { return ( cfg->a * valor + cfg->b ); }
//                if( ! palpite )
//                    palpite = constrain( ( drive.sensorDir->getReta() - drive.sensorEsq->getReta() ), -1, 1 );

                if(palpite < 0)
                    drive.giraEsq( 50 );
                else
                    drive.giraDir( 50 );
            }
            else if( drive.sensorEsq->ehMinimo(MARGEM_SHARP) )
                drive.giraEsq( 50 );
            else if( drive.sensorDir->ehMinimo(MARGEM_SHARP) )
                drive.giraEsq( 50 );
            else
            {
                // ceu de brigadeiro
                drive.praFrente( 100 );
                palpite = 0;
            }

        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_ALARME:
        {
            digitalWrite(PINO_LED, HIGH);
            SERIALX.println("ALARM");

            #define SIRENE_TOM_MIN  1000
            #define SIRENE_TOM_MAX  3000
            #define SIRENE_PASSO    2
            #define SIRENE_COMPASSO 150

            for(int x=0; x<2; x++)
            {
                for(int tom=SIRENE_TOM_MIN;tom<SIRENE_TOM_MAX;tom+=SIRENE_PASSO)
                {
                    BIP(tom,SIRENE_COMPASSO);
                }

                for(int tom=SIRENE_TOM_MAX;tom>SIRENE_TOM_MIN;tom-=SIRENE_PASSO)
                {
                    BIP(tom,SIRENE_COMPASSO/2);
                }
            }
            delay(1000);
            #ifdef PINO_BIP
                // TODO (mbs#1#): noTone() naum funfa na v2
                noTone(PINO_BIP);
                digitalWrite(PINO_BIP, LOW);
            #endif
            digitalWrite(PINO_LED, LOW);
            //delay(1000);
        }
        eeprom.dados.programa = PRG_RC;
        break;

        default:
            printErro( ERRO_PRG_INVALIDO );
            SERIALX.println(eeprom.dados.programa);
            eeprom.dados.programa = DFT_PROGRAMA;
        break;
        }
    }

    drive.refresh();
    #ifdef RODAS_PWM_x4
        drive2.refresh();
    #endif
}
