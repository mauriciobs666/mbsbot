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

// ******************************************************************************
//		INCLUDEs e CONFIGURACAO
// ******************************************************************************

#include "protocolo.h"
#include "placa.h"

// ANSI
#include <stdio.h>
#include <ctype.h>
#include <string.h>

// Arduino
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

#ifdef PINO_SERVO_PAN
Servo pan;
#endif
#ifdef PINO_SERVO_TILT
Servo tilt;
#endif
#ifdef PINO_SERVO_ROLL
Servo roll;
#endif

enum Erros ultimoErro = SUCCESSO;
unsigned long agora = 0;

// ******************************************************************************
//		EEPROM
// ******************************************************************************
#include <avr/eeprom.h>

typedef struct
{
    enum eTipoSensor { SENSOR_VIRTUAL, SENSOR_ANALOGICO, SENSOR_PING, SENSOR_RC, SENSOR_DIGITAL } tipo;
    unsigned char pino;
    bool invertido;
    volatile unsigned short minimo, maximo, centro;
    volatile bool autoMinMax;
    int a, b; // = a*x + b

    void init(eTipoSensor tipo_ = SENSOR_VIRTUAL,
              unsigned char pino_ = -1,
              bool invertido_ = false)
    {
        pino = pino_;
        if( pino_ >= 0 )
            invertido = invertido_;

        tipo = tipo_;
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

        a = invertido ? -1 : 1;
        b = 0;
        autoMinMax = false;
    }

    void print()
    {
        SERIALX.print((int)pino);
        SERIALX.print(" ");
        SERIALX.print((int)tipo);
        SERIALX.print(" ");
        SERIALX.print((int)invertido);
        SERIALX.print(" ");
        SERIALX.print(minimo);
        SERIALX.print(" ");
        SERIALX.print(maximo);
        SERIALX.print(" ");
        SERIALX.print(centro);
        SERIALX.print(" ");
        SERIALX.print(a);
        SERIALX.print(" ");
        SERIALX.print(b);
        SERIALX.println("");
    }
}
ConfigSensor;

typedef struct
{
    enum eTipoGamepad { TIPO_RC, TIPO_PC, TIPO_WII } tipo;
    ConfigSensor X, Y, Z, R;

    void init(  eTipoGamepad t,
                char pinoX = -1,
                char pinoY = -1,
                char pinoZ = -1,
                char pinoR = -1
            )
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
    enum eTipoMotor { MOTOR_SERVO, MOTOR_DC } tipo;
    int pino;
    bool invertido;
    int pinoDir;
    int pinoDirN;
    short centro;
    short aceleracao;   // % de potencia => dv / eeprom.delays.motores

    void initServo(int pino_, short centro_=1500, bool inverso=false)
    {
        tipo = MOTOR_SERVO;
        pino = pino_;
        centro = centro_;
        invertido = inverso;
    }

    void initDC(int pinoPWM, int pinoDIR, int pinoDIRN=0, short offsetZero=0, short acel=255, bool inverso=false)
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

class Eeprom
{
public:
    struct sConfiguracao
    {
        short programa;

        char handBrake;

        unsigned char balanco;   // % balanco motor esq / dir
        unsigned char velMax;    // %
        unsigned char velEscala; // %

        ConfigMotor motorEsq;
        ConfigMotor motorEsqT;
        ConfigMotor motorDir;
        ConfigMotor motorDirT;

        struct sDelays  // duracao (ms) de movimentos pra animacao
        {
            // timeout de envios pela serial de status e sensores
            int sensores;
            int status;

            int ES; // intervalo de entrada/saida, leitura de sensores etc
            int motores; // intervalo de execucao entre os refresh de motores
        } delays;

        // controlador PID

        struct sPID
        {
            int Kp;		// proporcional
            int Ki;		// integral
            int Kd;		// derivativo
            int maxMV;
            int debounce;
            int limiteP;
            int limiteI;
            int limiteD;
            int zeraAcc;
        } pid;

        ConfigGamepad joyRC, joyPC;

        ConfigSensor sensores[NUM_SENSORES];
    }
    dados;
    void load()
    {
        char * dest = (char*) &dados;
        for(unsigned int addr = 0; addr < sizeof(dados); addr++, dest++ )
            *dest = eeprom_read_byte((unsigned char *) addr);

        SERIALX.print("EEPROM ");
        SERIALX.print(sizeof(dados));
        SERIALX.println(" B");
    }
    void save()
    {
        char * dest = (char*) &dados;
        for(unsigned int addr = 0; addr < sizeof(dados); addr++, dest++ )
            eeprom_write_byte((unsigned char *) addr, *dest);
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

        dados.delays.sensores = -1;
        dados.delays.status = -1;
        dados.delays.ES = DFT_DELAY_ES;
        dados.delays.motores = DFT_VEL_REFRESH;

        dados.pid.Kp = DFT_PID_P;
        dados.pid.Ki = DFT_PID_I;
        dados.pid.Kd = DFT_PID_D;
        dados.pid.debounce = DFT_PID_DEBOUNCE;
        dados.pid.maxMV = DFT_PID_MAX_MV;
        dados.pid.limiteP = DFT_PID_LIM_P;
        dados.pid.limiteI = DFT_PID_LIM_I;
        dados.pid.limiteD = DFT_PID_LIM_D;
        dados.pid.zeraAcc = 0;

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
    void centrar()
        { if( cfg ) cfg->centro = valor; }
    Sensor& refresh()
        {
            if( cfg && ( cfg->pino >= 0 ) )
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
    void setReta(int aa, int bb)
        { cfg->a = aa; cfg->b = bb; }
    int getReta()
        { return ( cfg->a * valor + cfg->b ); }
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
                    if( cfg->maximo - cfg->minimo > 200 )
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
    Botao( int pino = -1, bool invertido = true ) : debounce(0), antes(false), estado(false), trocou(false)
    {
         if( pino >= 0 )
            init( pino, invertido );
    }
    void init( int pino, bool invertido = true )
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
    ConfigGamepad::eTipoGamepad tipo;
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

            if ( cfg->pinoDirN )
            {
                pinMode( cfg->pinoDirN, OUTPUT);
                digitalWrite( cfg->pinoDirN, 0);
            }
        }
        else if( cfg->tipo == ConfigMotor::MOTOR_SERVO )
        {
            servo.attach( cfg->pino );
        }
        parar();
    }

    void parar()
    {
        write( ( cfg->tipo == ConfigMotor::MOTOR_SERVO ) ? cfg->centro : 0 );
    }

    void move( char potencia100 )
    {
        if ( cfg->tipo == ConfigMotor::MOTOR_SERVO )
        {
            // TODO (mbs#1#): usar aceleracao e escala pro servo tb
            // servos tipicos aceitam pulsos entre 1000us e 2000us, ou seja, centro(1500us) +/- 500us
            write( cfg->invertido ? ( cfg->centro - potencia100*5 ) : ( cfg->centro + potencia100*5 ) );
            return;
        }
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

    void refresh()
    {
        // acelerador: v = v0 + at

        if( delaySemBlock( &ultimoAcel, eeprom.dados.delays.motores ) )
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
            atual = constrain( atual, -255, 255); // protecao de range

            // uma ultima olhada no freio de mao
            if( eeprom.dados.handBrake ) meta = atual = 0;

            digitalWrite( cfg->pinoDir, (atual < 0) ^ cfg->invertido ? HIGH : LOW); // 1/2 ponte H

            if( cfg->pinoDirN > 0 ) // pino de direcao invertido
            {
                if( atual ) // movendo, dirN = !dir
                    digitalWrite( cfg->pinoDirN, (atual < 0) ^ cfg->invertido ? LOW : HIGH); // outra 1/2 ponte H
                else        // freio, dirN = dir
                    digitalWrite( cfg->pinoDirN, (atual < 0) ^ cfg->invertido ? HIGH : LOW);
            }

            if( atual == 0 && cfg->pinoDirN > 0 )  // conduz 100% pra freiar
                analogWrite( cfg->pino, 255 );
            else                                   // operacao normal
                analogWrite( cfg->pino, abs( atual ) );
        }
        else // tipo == MOTOR_SERVO
        {
            if( eeprom.dados.handBrake ) meta = atual = cfg->centro;

            atual = constrain( atual, 1000, 2000 );
            servo.writeMicroseconds( atual );
        }
    }
protected:
    Servo servo;
    short atual;
    unsigned long ultimoAcel;
    short meta;
    char prioMeta; // TODO (Mauricio#1#): prioridade de quem setou a meta
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

    void refresh()
    {
        motorEsq.refresh();
        motorDir.refresh();
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
        SERIALX.print( VAR_RODA_ESQ );
        SERIALX.print(" ");
        SERIALX.println( motorEsq.read() );

        SERIALX.print( VAR_RODA_DIR );
        SERIALX.print(" ");
        SERIALX.println( motorDir.read() );
    }

    void giraEsq( char porc = 100 )
    {
        move( -porc, porc );
    }

    void giraDir( char porc = 100 )
    {
        move( porc, -porc );
    }

    void praFrente( char porc = 100 )
    {
        move( porc, porc );
    }
}
drive, drive2;

// ******************************************************************************
//		FOTOVORO
// ******************************************************************************
void fotovoro()
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

// ******************************************************************************
//   LINE FOLLOWER
// ******************************************************************************

//#ifdef LINE_FOLLOWER
#define TRACE_LF
class LineFollower
{
public:
    LineFollower() :
        acumulador(0),
        nGrupos(0), tamMaior(0),
        Proporcional(0), Integral(0), Derivada(0), MV(0),
        erro(0), erroAnterior(0), direcao(0), tEanterior(0), tEatual(0), fimDaVolta(0), debounce(0),
        marcaEsq(false), marcaDir(false), buscaInicioVolta(true), fodeu(false), estadoLed(false)
    {}
    void calibrar();
    void loop();

    void print()
    {
        SERIALX.print( "P " );
        SERIALX.print( Proporcional );
        SERIALX.print( " I " );
        SERIALX.print( Integral );
        SERIALX.print( " D " );
        SERIALX.println( Derivada );
    }

    void iniciarCorrida()
    {
        eeprom.dados.programa = PRG_LINE_FOLLOW;
        fimDaVolta = 0;
        buscaInicioVolta = true;
        estadoLed = true;
    }

    int acumulador;

    int nGrupos;
    int tamMaior;

    int Proporcional;
    int Integral;
    int Derivada;
    int MV;

    int erro;
    int erroAnterior;
    int direcao;
    int rodaEsq;
    int rodaDir;
    unsigned long tEanterior;
    unsigned long tEatual;
    unsigned long inicioCorrida;
    unsigned long fimDaVolta;
    unsigned long debounce;
    bool sensoresBool[ NUM_IR_TRACK ];
    bool debounceArray[ NUM_IR_TRACK ];
    bool marcaEsq;
    bool marcaDir;
    bool buscaInicioVolta;
    bool fodeu;
    bool estadoLed;

    class Grupo
    {
    public:
        int pontoMedio;
        int pontoMin;
        int pontoMax;
        int tamanho;
        Grupo()
        {}
        void print()
        {
            #ifdef TRACE_LF
                SERIALX.print(" m ");
                SERIALX.print((int)pontoMedio);
                SERIALX.print(" i ");
                SERIALX.print((int)pontoMin);
                SERIALX.print(" x ");
                SERIALX.print((int)pontoMax);
                SERIALX.print(" t ");
                SERIALX.print((int)tamanho);
                SERIALX.println("");
            #endif
        }
    }
    grupos[NUM_IR_TRACK/2], trilho;

    void refresh()
    {
        nGrupos = 0;
        tamMaior = 0;

        for(int sb = 0; sb < NUM_IR_TRACK; sb++)
            sensoresBool[sb] = sensores[ PINO_TRACK_0 + sb ].refresh().getBool();

        for(int s = 0; s < NUM_IR_TRACK; s++)
        {
            if( sensoresBool[s] )
            {
                int peso = 2*s + 1;

                grupos[ nGrupos ].pontoMin = peso;
                grupos[ nGrupos ].pontoMedio = peso;
                grupos[ nGrupos ].pontoMax = peso;
                grupos[ nGrupos ].tamanho = 1;

                // agrupa enquanto o proximo for "trilho"
                while( ( s < NUM_IR_TRACK-1 ) && sensoresBool[ s + 1 ] )
                {
                    s++;
                    peso = 2*s + 1;
                    grupos[ nGrupos ].pontoMax = peso;
                    grupos[ nGrupos ].pontoMedio += peso;
                    grupos[ nGrupos ].tamanho++;
                }

                grupos[ nGrupos ].pontoMedio /= grupos[ nGrupos ].tamanho;

                if( grupos[ nGrupos ].tamanho > tamMaior )
                    tamMaior = grupos[ nGrupos ].tamanho;

                nGrupos++;
            }
        }
    }
}
lineFollower;

void LineFollower::loop()
{
    if( fimDaVolta )
    {
        if( agora > fimDaVolta )
        {
            fimDaVolta = 0;
            drive.parar();
            drive.refresh();
            SERIALX.print("Lap:");
            SERIALX.println(int((agora-inicioCorrida)/1000));
            delay( 2000 );
        }
    }

    refresh();

    if( nGrupos )
    {
        fodeu = false;

        if( nGrupos == 1 && tamMaior <= (NUM_IR_TRACK/3) )
        {
            trilho = grupos[0];

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
                        SERIALX.println("C");
                    #endif
                }
                else if( marcaEsq )
                {
                    estadoLed = ! estadoLed;
                    #ifdef TRACE_LF
                        SERIALX.println("E");
                    #endif
                }
                else if( marcaDir )
                {
                    #ifdef TRACE_LF
                        SERIALX.println("D");
                        for( int x = 0; x < NUM_IR_TRACK; x++ )
                            SERIALX.print( sensoresBool[x] ? "1" : "0" );
                        SERIALX.println();

                        for( int ig = 0 ; ig < nGrupos ; ig++ )
                            grupos[ig].print();
                    #endif
                    if( buscaInicioVolta )
                    {
                        inicioCorrida = agora;
                        buscaInicioVolta = false;
                    }
                    else
                    {
                        fimDaVolta = agora; // + 500;
                    }
                }
                marcaEsq = marcaDir = false;
            }
        }
        else // ( nGrupos > 1 ) || tamMaior > (NUM_IR_TRACK/3)
        {
            #ifdef TRACE_LF
                if( ! debounce )
                {
                    for( int x = 0; x < NUM_IR_TRACK; x++ )
                        SERIALX.print( sensoresBool[x] ? "1" : "0" );
                    SERIALX.println();

                    for( int ig = 0 ; ig < nGrupos ; ig++ )
                        grupos[ig].print();
                }
            #endif

            debounce = eeprom.dados.pid.debounce;

            for( int s = 0; s < NUM_IR_TRACK; s++ )
                debounceArray[s] |= sensoresBool[s];

            if( nGrupos == 1 )
                trilho = grupos[0];
            else
            {
                for( int ig = 0 ; ig < nGrupos ; ig++ )
                {
                    if( ( abs( grupos[ig].pontoMedio - trilho.pontoMedio ) <= 1  )
                       && ( grupos[ig].tamanho <= (NUM_IR_TRACK/3) ) )
                        trilho = grupos[ig];
                }

                for( int ig = 0 ; ig < nGrupos ; ig++ )
                {
                    if( ( grupos[ig].pontoMax + 2 ) < trilho.pontoMin )
                        marcaEsq = true;

                    if( ( trilho.pontoMax + 2 ) < grupos[ig].pontoMin )
                        marcaDir = true;
                }
            }
        }

        erro = trilho.pontoMedio - NUM_IR_TRACK;

        Proporcional = erro;

        if( Proporcional > eeprom.dados.pid.limiteP )
            Proporcional = eeprom.dados.pid.limiteP;
        else if( Proporcional < -eeprom.dados.pid.limiteP )
            Proporcional = -eeprom.dados.pid.limiteP;

        Proporcional *= eeprom.dados.pid.Kp;

        Integral = 0;

        if( eeprom.dados.pid.Ki ) // zero desativa
        {
            static unsigned long ultimoLoop = 0;

            if( erro && ultimoLoop )
                acumulador += erro * (agora - ultimoLoop);
            else if( eeprom.dados.pid.zeraAcc )
                acumulador = 0;

            ultimoLoop = agora;

            if( acumulador > eeprom.dados.pid.limiteI )
                acumulador = eeprom.dados.pid.limiteI;
            else if( acumulador < -eeprom.dados.pid.limiteI )
                acumulador = -eeprom.dados.pid.limiteI;

            Integral = acumulador / eeprom.dados.pid.Ki;
        }

        Derivada = 0;

        if( erro != erroAnterior )
        {
            tEanterior = tEatual;
            tEatual = agora;
            direcao = erro - erroAnterior;
            erroAnterior = erro;
        }

        int intervalo = tEatual - tEanterior;

        if( (int) ( agora - tEatual ) > intervalo )
            intervalo = agora - tEatual;

        if( intervalo > eeprom.dados.pid.limiteD )
            intervalo = eeprom.dados.pid.limiteD;

        Derivada = ( eeprom.dados.pid.Kd * direcao ) / intervalo;

        MV = constrain( ( Proporcional + Integral + Derivada ), -eeprom.dados.pid.maxMV , eeprom.dados.pid.maxMV );

        rodaEsq = (MV < 0) ? (100 + MV) : 100;
        rodaDir = (MV > 0) ? (100 - MV) : 100;
    }
    else
    {
        if( ! fodeu )
        {
            fodeu = true;
            #ifdef TRACE_LF
                print();
            #endif
        }

        static unsigned long piscaLed=0;
        if( delaySemBlock( &piscaLed, 500 ) )
            estadoLed = ! estadoLed;

        rodaEsq = rodaDir = 0;
    }

    drive.move( rodaEsq , rodaDir );
    drive.refresh();

    digitalWrite( PINO_LED, estadoLed );
}

void LineFollower::calibrar()
{
    unsigned short valores[NUM_IR_TRACK];
    int num0s = 0;
    int num1s = 0;
    unsigned short minimo = 1023;
    unsigned short maximo = 0;

    drive.parar();
    drive.refresh();

    digitalWrite( PINO_LED, true );
    delay(100);

    agora = millis();
    drive.refresh();

    digitalWrite( PINO_LED, false );
    delay(1000);
    digitalWrite( PINO_LED, true );

    // primeira rodada de leitura

    for(int x = 0; x < NUM_IR_TRACK; x++)
    {
        valores[x] = sensores[ PINO_TRACK_0 + x ].calibrar();

        if( valores[x] < minimo ) minimo = valores[x];
        if( valores[x] > maximo ) maximo = valores[x];
    }

    // descobre threshold medio e classifica

    unsigned short meio = ( ( maximo - minimo ) >> 1 ) + minimo;

    for( int x = 0; x < NUM_IR_TRACK; x++ )
    {
        if( valores[x] > meio )
            num1s++;
        else
            num0s++;
    }

    // se tem mais trilho que brita inverte tds

    for( int x = 0; x < NUM_IR_TRACK; x++ )
        sensores[PINO_TRACK_0 + x].cfg->invertido = ( num1s > num0s );

    int backupVE = eeprom.dados.velEscala;
    eeprom.dados.velEscala = 100;

    const char v = 50;

    drive.giraEsq( v );

    unsigned long timeout = millis() + 5000;

    do
    {
        agora = millis();
        drive.refresh();
        refresh();
    }
    while( agora < timeout && ( ! nGrupos || grupos[0].pontoMedio < ( 2 * ( NUM_IR_TRACK-1 ) ) + 1 ));

    drive.parar();
    drive.refresh();

    delay( 100 );

    drive.giraDir( v );

    do
    {
        agora = millis();
        drive.refresh();
        refresh();
    }
    while( agora < timeout && grupos[0].pontoMedio > 1 );

    drive.parar();
    drive.refresh();

    delay( 100 );

    drive.giraEsq( v );

    do
    {
        agora = millis();
        drive.refresh();
        refresh();
    }
    while( agora < timeout && grupos[0].pontoMedio <  NUM_IR_TRACK );

    drive.parar();
    drive.refresh();

    delay( 100 );

    drive.giraDir( v );

    do
    {
        agora = millis();
        drive.refresh();
        refresh();
    }
    while( agora < timeout && grupos[0].pontoMedio > NUM_IR_TRACK );

    drive.parar();
    drive.refresh();

    digitalWrite( PINO_LED, false );

    eeprom.dados.velEscala = backupVE;
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
//		SERVIDOR TELNET
// ******************************************************************************
class Telnet
{
public:
    Telnet() : pos(0)
    {}

    bool recebe()
    {
        while(SERIALX.available() > 0)
        {
            char c = SERIALX.read();

            if (pos == MAX_CMD)
            {
                pos = 0;
                SERIALX.println("ERRO_TAM_MAX_CMD");
                ultimoErro = ERRO_TAM_MAX_CMD;
            }
            else if(c == CMD_EOL)
            {
                command[pos] = 0;
                pos = 0;
                return true;
            }
            else
                command[pos++] = c;
        }
        return false;
    }

    void loop()
    {
        if( recebe() )
        {
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

                        if((tok = STRTOK(NULL, " ")))			// terceiro token eh o valor a ser atribuido
                        {
                            int valor = atoi(tok);

                            if(isdigit(dest[0]))                // se destino for um numero entaum eh um pino digital
                                digitalWrite(atoi(dest), valor ? HIGH : LOW);
                            else if(strcmp(dest, VAR_RODA_ESQ) == 0)
                                drive.motorEsq.move(valor);
                            else if(strcmp(dest, VAR_RODA_DIR) == 0)
                                drive.motorDir.move(valor);
                            else if(strcmp(dest, VAR_ZERO_ESQ) == 0)
                                eeprom.dados.motorEsq.centro = valor;
                            else if(strcmp(dest, VAR_ZERO_DIR) == 0)
                                eeprom.dados.motorDir.centro = valor;
                            else if(strcmp(dest, VAR_ZERO_ESQ_T) == 0)
                                eeprom.dados.motorEsqT.centro = valor;
                            else if(strcmp(dest, VAR_ZERO_DIR_T) == 0)
                                eeprom.dados.motorDirT.centro = valor;
                            else if(strcmp(dest, VAR_PROGRAMA) == 0)
                            {
                                drive.parar();
                                eeprom.dados.programa = valor;
                            }
                            else if(strcmp(dest, VAR_T_RF) == 0)
                                eeprom.dados.delays.ES = valor;
                            #ifdef PINO_SERVO_PAN
                            else if(strcmp(dest, VAR_SERVO_X) == 0)
                                pan.write(valor);
                            #endif
                            #ifdef PINO_SERVO_TILT
                            else if(strcmp(dest, VAR_SERVO_Y) == 0)
                                tilt.write(valor);
                            #endif
                            #ifdef PINO_SERVO_ROLL
                            else if(strcmp(dest, VAR_SERVO_Z) == 0)
                                roll.write(valor);
                            #endif
                            else if(strcmp(dest, VAR_PID) == 0)
                            {
                                eeprom.dados.pid.Kp = valor;	// P
                                if((tok = STRTOK(NULL, " ")))	// I
                                    eeprom.dados.pid.Ki = atoi(tok);
                                if((tok = STRTOK(NULL, " ")))	// D
                                    eeprom.dados.pid.Kd = atoi(tok);
                            }
                            else if(strcmp(dest, VAR_PID_MMV) == 0)
                                eeprom.dados.pid.maxMV = valor;
                            else if(strcmp(dest, VAR_PID_DEB) == 0)
                                eeprom.dados.pid.debounce = valor;
                            else if(strcmp(dest, VAR_PID_LIM_P) == 0)
                                eeprom.dados.pid.limiteP = valor;
                            else if(strcmp(dest, VAR_PID_LIM_I) == 0)
                                eeprom.dados.pid.limiteI = valor;
                            else if(strcmp(dest, VAR_PID_LIM_D) == 0)
                                eeprom.dados.pid.limiteD = valor;
                            else if(strcmp(dest, VAR_PID_ZAC) == 0)
                                eeprom.dados.pid.zeraAcc = valor;
                            else if(strcmp(dest, VAR_FREIO) == 0)
                                eeprom.dados.handBrake = valor;
                            else if(strcmp(dest, VAR_ACEL_ESQ) == 0)
                                eeprom.dados.motorEsq.aceleracao = valor;
                            else if(strcmp(dest, VAR_ACEL_DIR) == 0)
                                eeprom.dados.motorDir.aceleracao = valor;
                            else if(strcmp(dest, VAR_T_ST) == 0)
                                eeprom.dados.delays.status = (unsigned short)valor;
                            else if(strcmp(dest, VAR_T_SE) == 0)
                                eeprom.dados.delays.sensores = (unsigned short)valor;
                            else if(strcmp(dest, VAR_VEL_MAX) == 0)
                                eeprom.dados.velMax = valor;
                            else if(strcmp(dest, VAR_VEL_ESCALA) == 0)
                                eeprom.dados.velEscala = valor;
                            else if(strcmp(dest, VAR_T_MOTOR) == 0)
                                eeprom.dados.delays.motores = valor;
                            else if(strcmp(dest, VAR_BALANCO) == 0)
                                eeprom.dados.balanco = valor;
                        }
                    }
                }
                else if(strcmp(tok, CMD_GET) == 0)	// le uma variavel
                {
                    if((tok = STRTOK(NULL, " ")))	// segundo token eh o nome da variavel a ser lida
                    {
                        SERIALX.print(tok);          // ecoa nome da variavel
                        SERIALX.print(" ");

                        if(strcmp(tok, VAR_RODA_ESQ) == 0)
                            SERIALX.println(drive.motorEsq.read());
                        else if(strcmp(tok, VAR_ZERO_ESQ) == 0)
                            SERIALX.println(eeprom.dados.motorEsq.centro);
                        else if(strcmp(tok, VAR_RODA_DIR) == 0)
                            SERIALX.println(drive.motorDir.read());
                        else if(strcmp(tok, VAR_ZERO_DIR) == 0)
                            SERIALX.println(eeprom.dados.motorDir.centro);
                        else if(strcmp(tok, VAR_PROGRAMA) == 0)
                            SERIALX.println(eeprom.dados.programa);
                        else if(strcmp(tok, VAR_T_RF) == 0)
                            SERIALX.println(eeprom.dados.delays.ES);
                        #ifdef PINO_SERVO_PAN
                        else if(strcmp(tok, VAR_SERVO_X) == 0)
                            SERIALX.println(pan.read());
                        #endif
                        #ifdef PINO_SERVO_TILT
                        else if(strcmp(tok, VAR_SERVO_Y) == 0)
                            SERIALX.println(tilt.read());
                        #endif
                        #ifdef PINO_SERVO_ROLL
                        else if(strcmp(tok, VAR_SERVO_Z) == 0)
                            SERIALX.println(roll.read());
                        #endif
                        else if(strcmp(tok, VAR_FREIO) == 0)
                            SERIALX.println((int)eeprom.dados.handBrake);
                        else if(strcmp(tok, VAR_AS) == 0)
                            enviaSensores(false);
                        else if(strcmp(tok, VAR_PID) == 0)
                        {
                            SERIALX.print(eeprom.dados.pid.Kp);
                            SERIALX.print(" ");
                            SERIALX.print(eeprom.dados.pid.Ki);
                            SERIALX.print(" ");
                            SERIALX.println(eeprom.dados.pid.Kd);
                        }
                        else if(strcmp(tok, VAR_PID_MMV) == 0)
                            SERIALX.println((int)eeprom.dados.pid.maxMV);
                        else if(strcmp(tok, VAR_PID_DEB) == 0)
                            SERIALX.println((int)eeprom.dados.pid.debounce);
                        else if(strcmp(tok, VAR_PID_LIM_P) == 0)
                            SERIALX.println((int)eeprom.dados.pid.limiteP);
                        else if(strcmp(tok, VAR_PID_LIM_I) == 0)
                            SERIALX.println((int)eeprom.dados.pid.limiteI);
                        else if(strcmp(tok, VAR_PID_LIM_D) == 0)
                            SERIALX.println((int)eeprom.dados.pid.limiteD);
                        else if(strcmp(tok, VAR_PID_ZAC) == 0)
                            SERIALX.println((int)eeprom.dados.pid.zeraAcc);
                        else if(strcmp(tok, VAR_ACEL_ESQ) == 0)
                            SERIALX.println((int)eeprom.dados.motorEsq.aceleracao);
                        else if(strcmp(tok, VAR_ACEL_DIR) == 0)
                            SERIALX.println((int)eeprom.dados.motorDir.aceleracao);
                        else if(strcmp(tok, VAR_T_ST) == 0)
                            SERIALX.println((int)eeprom.dados.delays.status);
                        else if(strcmp(tok, VAR_T_SE) == 0)
                            SERIALX.println((int)eeprom.dados.delays.sensores);
                        else if(strcmp(tok, VAR_VEL_MAX) == 0)
                            SERIALX.println((int)eeprom.dados.velMax);
                        else if(strcmp(tok, VAR_VEL_ESCALA) == 0)
                            SERIALX.println((int)eeprom.dados.velEscala);
                        else if(strcmp(tok, VAR_T_MOTOR) == 0)
                            SERIALX.println((int)eeprom.dados.delays.motores);
                        else if(strcmp(tok, VAR_BALANCO) == 0)
                            SERIALX.println((int)eeprom.dados.balanco);
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
                    drive2.parar();
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

                            if ((tok = STRTOK(NULL, " ")))  // segundo token eh o percentual de potencia p/ eixo X
                            {
                                direcao.x = atoi(tok);
                                if ((tok = STRTOK(NULL, " ")))// terceiro token eh o percentual de potencia p/ eixo Y
                                {
                                    direcao.y = atoi(tok);
                                    drive2.vetorial( direcao );
                                }
                            }
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
                    ultimoErro = SUCCESSO;
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
            SERIALX.print( VAR_AS);
            SERIALX.print(" ");
        }
        for (int x = 0; x < NUM_SENSORES; x++)
        {
            SERIALX.print( sensores[x].refresh().getValor() );
            SERIALX.print(" ");
        }
        SERIALX.println("");

        for( int x = 0; x < NUM_IR_TRACK; x++ )
            SERIALX.print( sensores[PINO_TRACK_0 + x].getBool() ? "1" : "0" );
        SERIALX.println();
    }

    void enviaStatus(bool enviaComando = true)
    {
        if(enviaComando)
        {
            SERIALX.print(CMD_STATUS);
            SERIALX.print(" ");
        }
        SERIALX.print(eeprom.dados.programa);
        SERIALX.print(" ");
        SERIALX.print(ultimoErro);
        SERIALX.print(" ");
        SERIALX.print((int)eeprom.dados.handBrake);
        SERIALX.print(" ");
        SERIALX.print(drive.motorEsq.read());
        SERIALX.print(" ");
        SERIALX.print(drive.motorDir.read());
        SERIALX.print(" ");
        SERIALX.print(drive2.motorEsq.read());
        SERIALX.print(" ");
        SERIALX.print(drive2.motorDir.read());
        SERIALX.print(" ");
        #ifdef PINO_SERVO_PAN
            SERIALX.print(pan.read());
            SERIALX.print(" ");
        #endif
        #ifdef PINO_SERVO_TILT
            SERIALX.print(tilt.read());
            SERIALX.print(" ");
        #endif
        #ifdef PINO_SERVO_ROLL
        SERIALX.print(roll.read());
        #endif
        //SERIALX.print(" ");
        //for(int p=13; p>=0; p--)
        //for(int p=13; p>5; p--)
        //    SERIALX.print(digitalRead(p));
        SERIALX.println("");
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
    short pos;
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
                    gamepad.refreshBotoes((duracao < 1500) ? BT_RT : 0);
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
    if(gamepad.botoesEdgeF & BT_SEL)
    {
        gamepad.calibrar();
        eeprom.dados.handBrake = 1;
        eeprom.dados.programa = PRG_RC_SERIAL;
    }

    if(gamepad.botoesEdgeF & BT_STR)
    {
        drive.parar();
        drive2.parar();

        // auto centra joystick
        gamepad.centrar();

        // solta freio de mao e poe no modo RC
        eeprom.dados.handBrake = 0;
        eeprom.dados.programa = PRG_RC_SERIAL;
    }

    if(gamepad.botoesEdgeF & BT_Y)
    {
        eeprom.dados.handBrake = 0;
        lineFollower.iniciarCorrida();
    }

    if(gamepad.botoesEdgeF & BT_A)
    {
        drive.parar();
        drive2.parar();
        eeprom.dados.programa = PRG_RC_SERIAL;
    }

    if(gamepad.botoesEdgeF & BT_X)
    {
        lineFollower.calibrar();
    }
//    if(gamepad.botoesEdgeF & BT_B)

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
//                if(gamepad.botoesEdge & BT_LB)
//                if(gamepad.botoesEdge & BT_RB)
//                if(gamepad.botoesEdge & BT_L3)
//                if(gamepad.botoesEdge & BT_R3)

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
    int unused[] = PINO_UNUSED_ARRAY;
    for(int p=0; p < PINO_UNUSED_CNT; p++)
    {
        pinMode(unused[p], INPUT);
        digitalWrite(unused[p], HIGH);
    }

/*
    // plaquinha com 8 leds conectada no canto do mega
    for( int i = 39; i <= 53; i += 2 )
        pinMode(i, OUTPUT);
    for( int x = 0; x < NUM_IR_TRACK; x++ )
        digitalWrite( (53 - (2 * x)) , sensoresBool[x] );
*/
//    if( eeprom.dados.programa == PRG_LINE_FOLLOW )
//        lineFollower.calibrar();
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

    static unsigned long ultimoStatus = 0;
    if( delaySemBlock(&ultimoStatus, eeprom.dados.delays.status) )
    {
        SERIALX.println(agora);
        telnet.enviaStatus();
        //digitalWrite(PINO_LED, !digitalRead(PINO_LED));
    }

    static unsigned long ultimoSensores = 0;
    if( delaySemBlock(&ultimoSensores, eeprom.dados.delays.sensores) )
    {
        SERIALX.println(agora);
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
                    intervalo = 1000;
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
            fotovoro();
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
            const unsigned int minDist = 200;
            const int timeAlarm = 3;	// segundos pra colisao
            const int s0 = 700;			// posicao da colisao

            // ignora objetos muito longe
            if( drive.sensorFre->refresh().getValor() > minDist )
            {
                // calc velocidade em unidades sensor / segundo
                int v = ((long)drive.sensorFre->delta() * 1000) / eeprom.dados.delays.ES;

                // s = s0 + v * t;
                int timeToCollision = (s0 - drive.sensorFre->getValor()) / v;

                if(timeToCollision < timeAlarm)
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
        eeprom.dados.delays.sensores = 0;
        eeprom.dados.delays.status = 0;
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
                if( ! palpite )
                    palpite = constrain( ( drive.sensorDir->getReta() - drive.sensorEsq->getReta() ), -1, 1 );

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
            if( ultimoErro != ERRO_PRG_INVALIDO )
            {
                ultimoErro = ERRO_PRG_INVALIDO;
                SERIALX.print("ERRO_PRG_INVALIDO ");
                SERIALX.println(eeprom.dados.programa);
                eeprom.dados.programa = DFT_PROGRAMA;
            }
        break;
        }
    }

    drive.refresh();
    #ifdef RODAS_PWM_x4
        drive2.refresh();
    #endif
}
