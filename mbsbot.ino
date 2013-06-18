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
        Serial.print((int)pino);
        Serial.print(" ");
        Serial.print((int)tipo);
        Serial.print(" ");
        Serial.print((int)invertido);
        Serial.print(" ");
        Serial.print(minimo);
        Serial.print(" ");
        Serial.print(maximo);
        Serial.print(" ");
        Serial.print(centro);
        Serial.print(" ");
        Serial.print(a);
        Serial.print(" ");
        Serial.print(b);
        Serial.println("");
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
    short aceleracao;   // % de potencia => dv / eeprom.velRefresh

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
        unsigned short velRefresh; // intervalo de execucao entre os refresh de motores

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
        } delays;

        // controlador PID
        struct sPID
        {
            int Kp;		// proporcional
            int Ki;		// integral
            int Kd;		// derivativo
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
        dados.velRefresh = DFT_VEL_REFRESH;
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

        dados.delays.sensores = -1; // 3m
        dados.delays.status = -1;
        dados.delays.ES = DFT_DELAY_ES;

        dados.pid.Kp = 40;
        dados.pid.Ki = 0;
        dados.pid.Kd = 0;

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
        Serial.print( "(" );
        Serial.print( x );
        Serial.print( "," );
        Serial.print( y );
        Serial.print( ")" );
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
                        digitalRead( cfg->pino );
                        /*
                        if (reading != lastButtonState)
                            lastDebounceTime = millis();

                        if ((millis() - lastDebounceTime) > debounceDelay)
                        {
                            if (reading != buttonState)
                            {
                                buttonState = reading;

                                if (buttonState == HIGH)
                                    ledState = !ledState;
                            }
                        }
                        digitalWrite(ledPin, ledState);
                        lastButtonState = reading;
                        */
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
            if( cfg->maximo - cfg->minimo < 200 )
                return false;
            unsigned short meio = ( ( cfg->maximo - cfg->minimo ) >> 1 ) + cfg->minimo;
            return ( ( valor > meio ) ^ cfg->invertido );
        }
}
sensores[NUM_SENSORES];

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

        if( delaySemBlock( &ultimoAcel, eeprom.dados.velRefresh ) )
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

    void vetorial(Vetor2i direcao);

    void vetorialSensor(Vetor2i direcao);

    void printRodas()
    {
        Serial.print( VAR_RODA_ESQ );
        Serial.print(" ");
        Serial.println( motorEsq.read() );

        Serial.print( VAR_RODA_DIR );
        Serial.print(" ");
        Serial.println( motorDir.read() );
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

void Drive::vetorial( Vetor2i direcao )
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

void Drive::vetorialSensor( Vetor2i direcao )
{
    direcao.Constrain();

    //#define TRACE
    #ifdef TRACE
        Serial.print(" dir");
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
            Serial.print(" se=");
            Serial.print(s_esq);
            Serial.print(" sd=");
            Serial.print(s_dir);
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
            Serial.print(" esq");
            esq.print();
        #endif

        Vetor2i dir( ( ( seno * s_dir ) / 100 ) , ( ( cosseno * s_dir ) / 100 ) );

        #ifdef TRACE
            Serial.print(" dir");
            dir.print();
        #endif

        Vetor2i obstaculos = esq + dir;

        #ifdef TRACE
            Serial.print(" obs" );
            obstaculos.print();
            Serial.print(" |obs| ");
            Serial.print(obstaculos.norma());
        #endif

        //obstaculos.normalizar();
        obstaculos.Constrain();

        #ifdef TRACE
            Serial.print(" obs");
            obstaculos.print();
        #endif

        resultante += obstaculos ;

        #ifdef TRACE
            Serial.print(" |res| ");
            Serial.print(resultante.norma());
        #endif

        //resultante.normalizar();

        //resultante.y = constrain( resultante.y, 0, 100 ); // never come back !
    }

    #ifdef TRACE
        Serial.print(" res");
        resultante.print();
    #endif

    drive.vetorial( resultante );

    #ifdef TRACE
        Serial.println("\n");
    #endif

    #ifdef TRACE
        printRodas();
    #endif
    //#undef TRACE
}

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
class LineFollower
{
public:
    LineFollower() : acumulador(0), iniHist(0), fimHist(0), nGrupos(0)
    {}
    void calibrar();
    void loop();
private:
    void refresh()
    {
        nGrupos = 0;
        tamMaior = 0;

        for(int s = 0; s < NUM_IR_TRACK; s++)
        {
            if( sensores[ PINO_TRACK_0 + s ].refresh().getBool() )
            {
                grupos[ nGrupos ].trilho = false;
                grupos[ nGrupos ].pontoMedio = 2*s + 1;
                grupos[ nGrupos ].tamanho = 1;

                // agrupa enquanto o proximo for "trilho"
                while( ( s < NUM_IR_TRACK-1 ) && ( sensores[ PINO_TRACK_0 + s + 1 ].refresh().getBool() ) )
                {
                    s++;
                    grupos[ nGrupos ].pontoMedio += 2*s + 1;
                    grupos[ nGrupos ].tamanho++;
                }

                grupos[ nGrupos ].pontoMedio /= grupos[ nGrupos ].tamanho;

                if( grupos[ nGrupos ].tamanho > tamMaior )
                    tamMaior = grupos[ nGrupos ].tamanho;

                nGrupos++;
            }
        }
    }

    long acumulador;

    int historico[50]; // buffer circular erros
    int iniHist;
    int fimHist;

    typedef struct
    {
        int pontoMedio;
        int tamanho;
        bool trilho;
    } Grupo;

    Grupo grupos[NUM_IR_TRACK/2];
    int nGrupos;

    Grupo trilho;

    int tamMaior;
}
lineFollower;

void LineFollower::loop()
{
    refresh();

    if( nGrupos )
    {

        trilho = grupos[0];

        int erro = trilho.pontoMedio - NUM_IR_TRACK;

        #ifdef TRACE
        Serial.print("Erro = ");
        Serial.println(erro);
        #endif

        int Proporcional = eeprom.dados.pid.Kp * erro;

        acumulador += erro;

        int Integral = acumulador / eeprom.dados.pid.Ki;

        int Derivativo = eeprom.dados.pid.Kd;// * ( trilho - trilhoAntes );

        int MV = constrain( (Proporcional + Integral + Derivativo), -200, 200 );

        drive.move( ( (MV < 0) ? (100 + MV) : 100 ) , ( (MV > 0) ? (100 - MV) : 100 ) );

     }
    else
        drive.parar();
}

void LineFollower::calibrar()
{
    unsigned short valores[NUM_IR_TRACK];
    int num0s = 0;
    int num1s = 0;
    unsigned short minimo = 1023;
    unsigned short maximo = 0;

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

    drive.giraEsq( 60 );

    unsigned long timeout = millis() + 5000;

    do
    {
        agora = millis();
        drive.refresh();
        refresh();
    }
    while( agora < timeout && ( ! nGrupos || grupos[0].pontoMedio < 2 * ( NUM_IR_TRACK-1 ) ));

    drive.parar();

    if( agora < timeout )
    {
        delay( 100 );

        drive.giraDir( 60 );

        do
        {
            agora = millis();
            drive.refresh();
            refresh();
        }
        while( agora < timeout && grupos[0].pontoMedio > 1 );

        drive.parar();
    }
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
    bool stepUp();
    bool stepDown();
    void refreshServo()
        {
            #ifdef PINO_SERVO_PAN
            pan.write( (SCANNER_ANG/2) + stepAtual * SCANNER_ANG );
            #endif
        }
    void chase();
    void fillArray();
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

bool Scanner::stepUp()
{
    if(upperBound())
        stepAtual = (SCANNER_STEPS-1);
    else
        stepAtual++;
    return upperBound();
}

bool Scanner::stepDown()
{
    if(lowerBound())
        stepAtual = 0;
    else
        stepAtual--;
    return lowerBound();
}

void Scanner::chase()
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

void Scanner::fillArray()
{
    dataArray[stepAtual] = s->refresh(); // le sensor

    if( step() ) // calcula proxima posicao, retorna true se encheu array
    {
        reverseDir();
/*
        Serial.print("RFA ");
        for(int x = 0; x < SCANNER_STEPS; x++)
        {
            Serial.print(dataArray[x]);
            Serial.print(" ");
        }
        Serial.println("");
*/
    }
    refreshServo();
}
#endif
// ******************************************************************************
//		DEBUG / SENSORES
// ******************************************************************************
void enviaSensores(bool enviaComando = true)
{
    if(enviaComando)
    {
        Serial.print( VAR_AS);
        Serial.print(" ");
    }
    for (int x = 0; x < NUM_SENSORES; x++)
    {
        Serial.print( sensores[x].refresh().getValor() );
        Serial.print(" ");
    }
    Serial.println("");

    for( int x = 0; x < NUM_IR_TRACK; x++ )
        Serial.print( sensores[PINO_TRACK_0 + x].getBool() ? "1" : "0" );
    Serial.println();
}

void enviaStatus(bool enviaComando = true)
{
    if(enviaComando)
    {
        Serial.print(CMD_STATUS);
        Serial.print(" ");
    }
    Serial.print(eeprom.dados.programa);
    Serial.print(" ");
    Serial.print(ultimoErro);
    Serial.print(" ");
    Serial.print((int)eeprom.dados.handBrake);
    Serial.print(" ");
    Serial.print(drive.motorEsq.read());
    Serial.print(" ");
    Serial.print(drive.motorDir.read());
    Serial.print(" ");
    Serial.print(drive2.motorEsq.read());
    Serial.print(" ");
    Serial.print(drive2.motorDir.read());
    Serial.print(" ");
    #ifdef PINO_SERVO_PAN
        Serial.print(pan.read());
        Serial.print(" ");
    #endif
    #ifdef PINO_SERVO_TILT
        Serial.print(tilt.read());
        Serial.print(" ");
    #endif
    #ifdef PINO_SERVO_ROLL
    Serial.print(roll.read());
    #endif
    //Serial.print(" ");
    //for(int p=13; p>=0; p--)
    //for(int p=13; p>5; p--)
    //    Serial.print(digitalRead(p));
    Serial.println("");
}

void enviaJoystick()
{
    Serial.print(CMD_JOYPAD);
    Serial.print(" X ");
    Serial.print(gamepad.x.getPorcentoAprox(0));
    Serial.print(" ~ ");
    Serial.print(gamepad.x.getPorcentoAprox());

    Serial.print(" (");
    Serial.print(gamepad.x.cfg->minimo);
    Serial.print(",");
    Serial.print(gamepad.x.cfg->centro);
    Serial.print(",");
    Serial.print(gamepad.x.cfg->maximo);
    Serial.print(") ");
    Serial.println(gamepad.x.valor);

    Serial.print(CMD_JOYPAD);
    Serial.print(" Y ");
    Serial.print(gamepad.y.getPorcentoAprox(0));
    Serial.print(" ~ ");
    Serial.print(gamepad.y.getPorcentoAprox());

    Serial.print(" (");
    Serial.print(gamepad.y.cfg->minimo);
    Serial.print(",");
    Serial.print(gamepad.y.cfg->centro);
    Serial.print(",");
    Serial.print(gamepad.y.cfg->maximo);
    Serial.print(") ");
    Serial.println(gamepad.y.valor);
/*
    Serial.print(" Z ");
    Serial.print(gamepad.z.getPorcentoAprox(0));
    Serial.print(" R ");
    Serial.println(gamepad.r.getPorcentoAprox(0));
*/
}

void uname()
{
    Serial.print("MBSBOT hw ");
    Serial.print(VERSAO_PLACA);
    Serial.print(" sw ");
    Serial.println(VERSAO_PROTOCOLO);
}

// ******************************************************************************
//		SERVIDOR TELNET
// ******************************************************************************
class Telnet
{
public:
    Telnet() : pos(0) {}
    bool recebe();
    void loop();
private:
    char command[MAX_CMD];
    short pos;
}
telnet;

bool Telnet::recebe()
{
    while(Serial.available() > 0)
    {
        char c = Serial.read();

        if (pos == MAX_CMD)
        {
            pos = 0;
            Serial.println("ERRO_TAM_MAX_CMD");
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

void Telnet::loop()
{
    if( telnet.recebe() )
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
                        else if(strcmp(dest, VAR_VEL_REFRESH) == 0)
                            eeprom.dados.velRefresh = valor;
                        else if(strcmp(dest, VAR_BALANCO) == 0)
                            eeprom.dados.balanco = valor;
                    }
                }
            }
            else if(strcmp(tok, CMD_GET) == 0)	// le uma variavel
            {
                if((tok = STRTOK(NULL, " ")))	// segundo token eh o nome da variavel a ser lida
                {
                    Serial.print(tok);          // ecoa nome da variavel
                    Serial.print(" ");

                    if(strcmp(tok, VAR_RODA_ESQ) == 0)
                        Serial.println(drive.motorEsq.read());
                    else if(strcmp(tok, VAR_ZERO_ESQ) == 0)
                        Serial.println(eeprom.dados.motorEsq.centro);
                    else if(strcmp(tok, VAR_RODA_DIR) == 0)
                        Serial.println(drive.motorDir.read());
                    else if(strcmp(tok, VAR_ZERO_DIR) == 0)
                        Serial.println(eeprom.dados.motorDir.centro);
                    else if(strcmp(tok, VAR_PROGRAMA) == 0)
                        Serial.println(eeprom.dados.programa);
                    else if(strcmp(tok, VAR_T_RF) == 0)
                        Serial.println(eeprom.dados.delays.ES);
                    #ifdef PINO_SERVO_PAN
                    else if(strcmp(tok, VAR_SERVO_X) == 0)
                        Serial.println(pan.read());
                    #endif
                    #ifdef PINO_SERVO_TILT
                    else if(strcmp(tok, VAR_SERVO_Y) == 0)
                        Serial.println(tilt.read());
                    #endif
                    #ifdef PINO_SERVO_ROLL
                    else if(strcmp(tok, VAR_SERVO_Z) == 0)
                        Serial.println(roll.read());
                    #endif
                    else if(strcmp(tok, VAR_FREIO) == 0)
                        Serial.println((int)eeprom.dados.handBrake);
                    else if(strcmp(tok, VAR_AS) == 0)
                        enviaSensores(false);
                    else if(strcmp(tok, VAR_PID) == 0)
                    {
                        Serial.print(eeprom.dados.pid.Kp);
                        Serial.print(" ");
                        Serial.print(eeprom.dados.pid.Ki);
                        Serial.print(" ");
                        Serial.println(eeprom.dados.pid.Kd);
                    }
                    else if(strcmp(tok, VAR_ACEL_ESQ) == 0)
                        Serial.println((int)eeprom.dados.motorEsq.aceleracao);
                    else if(strcmp(tok, VAR_ACEL_DIR) == 0)
                        Serial.println((int)eeprom.dados.motorDir.aceleracao);
                    else if(strcmp(tok, VAR_T_ST) == 0)
                        Serial.println((int)eeprom.dados.delays.status);
                    else if(strcmp(tok, VAR_T_SE) == 0)
                        Serial.println((int)eeprom.dados.delays.sensores);
                    else if(strcmp(tok, VAR_VEL_MAX) == 0)
                        Serial.println((int)eeprom.dados.velMax);
                    else if(strcmp(tok, VAR_VEL_ESCALA) == 0)
                        Serial.println((int)eeprom.dados.velEscala);
                    else if(strcmp(tok, VAR_VEL_REFRESH) == 0)
                        Serial.println((int)eeprom.dados.velRefresh);
                    else if(strcmp(tok, VAR_BALANCO) == 0)
                        Serial.println((int)eeprom.dados.balanco);
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
        eeprom.dados.programa = PRG_NAV_3S;

    if(gamepad.botoesEdgeF & BT_A)
    {
        eeprom.dados.handBrake = 1;
        eeprom.dados.programa = PRG_RC_SERIAL;
    }

//    if(gamepad.botoesEdgeF & BT_X)
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
    Serial.begin(115200);
    uname();

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

//#if VERSAO_PLACA == 22
    drive.sensorEsq = &sensores[1];
    drive.sensorDir = &sensores[2];
    drive.sensorFre = &sensores[3];
/*
#else
    sensorFre = &sensores[1];
    sensorDir = &sensores[2];
    sensorEsq = &sensores[3];
#endif
*/
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

    if( eeprom.dados.programa == PRG_LINE_FOLLOW )
        lineFollower.calibrar();
}

// ******************************************************************************
//		for(;;) ...
// ******************************************************************************
void loop()
{
    agora = millis();

    telnet.loop();

    trataJoystick();

    static unsigned long ultimoStatus = 0;
    if( delaySemBlock(&ultimoStatus, eeprom.dados.delays.status) )
    {
        Serial.println(agora);
        enviaStatus();
        //digitalWrite(PINO_LED, !digitalRead(PINO_LED));
    }

    static unsigned long ultimoSensores = 0;
    if( delaySemBlock(&ultimoSensores, eeprom.dados.delays.sensores) )
    {
        Serial.println(agora);
        //enviaJoystick();
        enviaSensores();
        #ifdef WIICHUCK
            nunchuck_print_data();
        #endif
    }

    #ifdef TESTE_PERFORMANCE
    static unsigned long passagensLoop = 0;
    static unsigned long ultimoLoop = 0;
    if( delaySemBlock(&ultimoLoop, 10000) )
    {
        Serial.print( passagensLoop / 10 );
        Serial.println(" fps");
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
            Serial.write((analogRead(0) >> 2) & 0xFF);
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
            Serial.println("ALARM");

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
                Serial.print("ERRO_PRG_INVALIDO ");
                Serial.println(eeprom.dados.programa);
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
