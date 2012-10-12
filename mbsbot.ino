/**	Copyright (C) 2010-2012 - Mauricio Bieze Stefani
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

#include "Geometria.h"

// speaker
#ifdef PINO_BIP
#define BEEP(freq, dur) tone(PINO_BIP,freq,dur)
#else
#define BEEP(freq, dur)
#endif

// ******************************************************************************
//		Plin-plin
// ******************************************************************************

Servo pan;
Servo tilt;
Servo roll;

enum Errors lastError = SUCCESSO;
unsigned long agora = 0;

// ******************************************************************************
//		EEPROM
// ******************************************************************************
#include <avr/eeprom.h>

typedef struct sConfigSensor
{
    enum eTipoSensor { SENSOR_ANALOGICO, SENSOR_PING, SENSOR_VIRTUAL, SENSOR_RC } tipo;
    unsigned char pino;
    bool invertido;
    volatile unsigned short anterior, minimo, maximo, centro;
    int a, b; // = a*x + b

    void init(unsigned char pino_ = 0,
              eTipoSensor tipo_ = SENSOR_ANALOGICO,
              bool invertido_ = false)
    {
        if( pino_ )
        {
            pino = pino_;
            tipo = tipo_;
            invertido = invertido_;
            anterior = 0;
            a = invertido ? -1 : 1;
            b = 0;

            if(SENSOR_RC == tipo)
            {
                minimo = 1300;
                centro = 1500;
                maximo = 1700;
            }
            else
            {
                minimo = 65535;
                centro = 32767;
                maximo = 0;
            }
        }
    }

    void print()
    {
        Serial.print((int)pino);
        Serial.print(" ");
        Serial.print((int)tipo);
        Serial.print(" ");
        Serial.print((int)invertido);
        Serial.print(" ");
        Serial.print(anterior);
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
} ConfigSensor;


typedef struct
{
    short centro;
    short aceleracao;
} ConfigMotor;

class Eeprom
{
public:
    struct sConfiguracao
    {
        short programa;

        char handBrake;

        // TODO (mbs#1#): configuracao via serial
        char velMax;    // %
        char velEscala; // %

        ConfigMotor motorEsq;
        ConfigMotor motorEsqT;
        ConfigMotor motorDir;
        ConfigMotor motorDirT;

        struct sDelays  // duracao (ms) de movimentos pra animacao
        {
            unsigned short mvPol;	// tempo pra mover 1 polegada
            unsigned short mv90;	// tempo pra girar 90 graus (angulo reto)

            // intervalo entre envios pela serial de status e sensores
            unsigned short sensores;
            unsigned short status;

            short reads; // intervalo generico de leitura de sensores
        } delays;

        // parametros do seguidor de linha
        #ifdef LINE_FOLLOWER
            unsigned short LF_threshold[NUM_IR_TRACK];
            bool LF_reverseColor;
        #endif

        // controlador PID
        struct sPID
        {
            int Kp;		// proporcional
            int Ki;		// integral
            int Kd;		// derivativo
        } pid;

        ConfigSensor gameX, gameY, gameZ, gameR;

        ConfigSensor sensores[NUM_SENSORES];
    } dados;
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
        dados.programa = PRG_RC;
        dados.handBrake = 1;
        dados.velMax = 100;
        dados.velEscala = 100;

        #ifdef RODAS_PWM
        dados.motorEsq.centro = 70;
        dados.motorDir.centro = 70;
        dados.motorEsqT.centro = 70;
        dados.motorDirT.centro = 70;
        #else
        dados.motorEsq.centro = 1410;
        dados.motorDir.centro = 1384;
        #endif

        dados.motorEsq.aceleracao = 10;
        dados.motorDir.aceleracao = 10;
        dados.motorEsqT.aceleracao = 10;
        dados.motorDirT.aceleracao = 10;

        dados.delays.mvPol = 200;
        dados.delays.mv90 = 400;
        dados.delays.sensores = 0;
        dados.delays.status = 0;
        dados.delays.reads = 100;

        #ifdef LINE_FOLLOWER
        for(int x = 0; x < NUM_IR_TRACK; x++)
        {
            dados.LF_threshold[x] = 512;
            dados.LF_reverseColor = false;
        }
        #endif

        dados.pid.Kp = 100;
        dados.pid.Ki = 0;
        dados.pid.Kd = 0;

        // TODO (mbs#1#): remover config de sensores hard-coded e permitir config serial

        dados.sensores[0].init(0, ConfigSensor::SENSOR_ANALOGICO); // bateria

        dados.sensores[1].init(15, ConfigSensor::SENSOR_PING);
        dados.sensores[1].minimo = 150;

#ifdef VERSAO_PLACA == 22
        dados.sensores[2].init(16, ConfigSensor::SENSOR_PING);
        dados.sensores[2].minimo = 150;
#else
        dados.sensores[2].init(2, ConfigSensor::SENSOR_ANALOGICO, true);
        dados.sensores[2].minimo = 100;
        dados.sensores[2].maximo = 630;
#endif

        dados.sensores[3].init(3, ConfigSensor::SENSOR_ANALOGICO, true);
        dados.sensores[3].minimo = 100;
        dados.sensores[3].maximo = 630;

        #ifdef PINO_JOY_X
            dados.gameX.init(PINO_JOY_X, ConfigSensor::SENSOR_RC);
        #endif
        #ifdef PINO_JOY_Y
            dados.gameY.init(PINO_JOY_Y, ConfigSensor::SENSOR_RC);
        #endif
        #ifdef PINO_JOY_Z
            dados.gameZ.init(PINO_JOY_Z, ConfigSensor::SENSOR_RC);
        #endif
        #ifdef PINO_JOY_R
            dados.gameR.init(PINO_JOY_R, ConfigSensor::SENSOR_RC);
        #endif
    }
}
eeprom;

// ******************************************************************************
//		DELAY SEM BLOCK (nao cumulativo)
// ******************************************************************************
bool delaySemBlock(unsigned long *ultimaVez, unsigned long ms)
{
    // 0 desativa
    if( ms && (agora > *ultimaVez + ms) )
    {
        *ultimaVez = agora; // += ms;
        return true;
    }
    return false;
}


/*
	Vetor2i
*/

Vetor2i Vetor2i::operator+(const Vetor2i& v) const
{
	Vetor2i n=*this;
	n.x+=v.x;
	n.y+=v.y;
	return n;
}

Vetor2i Vetor2i::operator-(const Vetor2i& v) const
{
	Vetor2i n=*this;
	n.x-=v.x;
	n.y-=v.y;
	return n;
}

Vetor2i Vetor2i::operator*(int i) const
{
	Vetor2i n=*this;
	n.x*=i;
	n.y*=i;
	return n;
}

Vetor2i Vetor2i::operator/(int i) const
{
	Vetor2i n=*this;
	n.x/=i;
	n.y/=i;
	return n;
}

Vetor2i& Vetor2i::operator+=(const Vetor2i& v)
{
	x+=v.x;
	y+=v.y;
	return *this;
}

Vetor2i& Vetor2i::operator-=(const Vetor2i& v)
{
	x-=v.x;
	y-=v.y;
	return *this;
}

Vetor2i& Vetor2i::operator*=(int i)
{
	x*=i;
	y*=i;
	return *this;
}

Vetor2i& Vetor2i::operator/=(int i)
{
	x/=i;
	y/=i;
	return *this;
}

// ******************************************************************************
//		SENSOR UNIVERSAL
// ******************************************************************************
class Sensor
{
public:
    volatile unsigned short valor;
    ConfigSensor *cfg;

    Sensor()
    {
        valor = 0;
        cfg = NULL;
    }
    void setConfig(ConfigSensor *c)
        { cfg=c; }
    unsigned short getValor()
        { return valor; }
    void setValor(unsigned short v)
        {
            if( cfg )
            {
                cfg->anterior = valor;
                if (v < cfg->minimo) cfg->minimo = v;
                if (v > cfg->maximo) cfg->maximo = v;
            }
            valor = v;
        }
    void calibrar()
        {
            if( cfg )
            {
                cfg->minimo = 65537;
                cfg->maximo = 0;
                cfg->centro = valor;
            }
        }
    void centrar()
        { if( cfg ) cfg->centro = valor; }
    unsigned short refresh()
        {
            if(cfg)
            {
                switch(cfg->tipo)
                {
                    case ConfigSensor::SENSOR_ANALOGICO:
                        setValor(analogRead(cfg->pino));
                    break;
                    case ConfigSensor::SENSOR_PING:
                        // manda pulso de 2ms pro ping))) pra acionar leitura
                        pinMode(cfg->pino, OUTPUT);
                        digitalWrite(cfg->pino, LOW);
                        delayMicroseconds(2);
                        digitalWrite(cfg->pino, HIGH);
                        delayMicroseconds(5);
                        digitalWrite(cfg->pino, LOW);

                        // duracao do pulso = distancia
                        pinMode(cfg->pino, INPUT);
                        setValor(pulseIn(cfg->pino, HIGH));
                    default:
                    break;
                }
            }
            return valor;
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
        { return valor - cfg->anterior; }
}
sensores[NUM_SENSORES], *sensorFrente, *sensorEsquerda, *sensorDireita;

// ******************************************************************************
//		GAMEPAD E R/C
// ******************************************************************************
class MbsGamePad
{
public:
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
    enum eTipoMotor { MOTOR_SERVO, MOTOR_DC } tipo;

    Motor() : tipo(MOTOR_DC), atual(0), centro(0), aceleracao(0), ultimoAcel(0), meta(0),
        invertido(false), pwm(-1), dir(-1), dirN(-1) { }

    void initServo(int pin, short centerAng=1500, bool inverso=false)
    {
        tipo = MOTOR_SERVO;
        servo.attach(pin);
        setCenter(centerAng);
        invertido = inverso;
        stop();
    }

    void initDC(int pinoPWM, int pinoDIR, short offsetZero=0, short acel=255, bool inverso=false)
    {
        tipo = MOTOR_DC;
        pwm = pinoPWM;
        pinMode(pwm, OUTPUT);
        analogWrite(pwm, 0);

        dir = pinoDIR;
        pinMode(dir, OUTPUT);
        digitalWrite(dir, 0);

        setCenter(offsetZero);
        invertido = inverso;
        aceleracao = acel;
        stop();
    }

    void stop()
    {
        write( ( tipo == MOTOR_SERVO ) ? centro : 0 );
    }

    void move(char potencia100)
    {
        if ( tipo == MOTOR_SERVO )
        {
            // TODO (mbs#1#): usar aceleracao e escala pro servo tb
            // servos tipicos aceitam pulsos entre 1000us e 2000us, ou seja, centro(1500us) +/- 500us
            write( invertido ? (centro - potencia100*5) : (centro + potencia100*5) );
            return;
        }
        /* onde:
            potencia100 = +/- 0-100 %
            centro = pwm a partir do qual o motor comeca a se mover
        */
        if ( potencia100 )
        {
            potencia100 = map(potencia100, -100, 100, -eeprom.dados.velEscala, eeprom.dados.velEscala);
            potencia100 = constrain(potencia100, -eeprom.dados.velMax, eeprom.dados.velMax);  // velocidade maxima
            short c = potencia100 > 0 ? centro : -centro;           // c = "centro" com sinal
            short fator = 255 - centro;                             // faixa de controle (linear?)
            meta = c + ( potencia100 * fator ) / 100;               // converte % de potencia pra PWM 8 b
            meta = constrain(meta, -255, 255);                      // range de saida: +/- centro ... 255
        }
        else
            meta = 0;

        refresh();
    }

    void write(int valor)
    {
        meta = atual = valor;
        refresh();
    }

    void refresh()
    {
        // acelerador: v = v0 + at

        while(agora >= ultimoAcel + 10)
        {
            ultimoAcel += 10;
            if ( meta > atual)
            {
                if( atual == 0 ) // estava parado
                    atual = centro;
                else
                    atual += aceleracao;

                if( meta < atual) // passou do ponto
                    atual = meta;
            }
            else if ( meta < atual)
            {
                if( atual == 0 ) // estava parado
                    atual = -centro;
                else
                    atual -= aceleracao;

                if ( meta > atual)
                    atual = meta;
            }

            if ( abs(atual) < centro )
                atual = 0;
        }

        // I/O

        if( tipo == MOTOR_DC )
        {
            atual = constrain(atual, -255, 255); // protecao de range

            // uma ultima olhada no freio de mao
            if(eeprom.dados.handBrake) meta = atual = 0;

            digitalWrite(dir, (atual < 0) ^ invertido ? HIGH : LOW); // 1/2 ponte H

            if(dirN > 0) // pino de direcao invertido
            {
                if( atual ) // movendo, dirN = !dir
                    digitalWrite(dirN, (atual < 0) ^ invertido ? LOW : HIGH); // outra 1/2 ponte H
                else        // freio, dirN = dir
                    digitalWrite(dirN, (atual < 0) ^ invertido ? HIGH : LOW);
            }

//            if( ( !atual ) && ( dirN > 0 ) )    // freio
//                analogWrite(pwm, 255);          // conduz 100% pra freiar
//            else                                // operacao normal
                analogWrite(pwm, abs(atual));
        }
        else // tipo == MOTOR_SERVO
        {
            if(eeprom.dados.handBrake) meta = atual = centro;

            atual = constrain(atual, 1000, 2000);
            servo.writeMicroseconds(atual);
        }
    }

    short read() { return atual; }
    void setCenter(short valor) { centro = valor; }
    void setAceleracao(short acel) { aceleracao = acel; }
    void setN(int pinoN)
    {
        dirN = pinoN;
        pinMode(dirN, OUTPUT);
        digitalWrite(dirN, 0);
    }
protected:
    short atual;
    short centro;
    short aceleracao;
    unsigned long ultimoAcel;
    short meta;
    bool invertido;

    Servo servo;
    int pwm;
    int dir;
    int dirN;
};

class Drive
{
public:
    Motor motorEsq;
    Motor motorDir;
    void forward(char percent=100, int period=0)
    {
        motorEsq.move(percent);
        motorDir.move(percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void backward(char percent=100, int period=0)
    {
        forward(-percent, period);
    }
    void left(char percent=100, int period=0)
    {
        motorEsq.move(-percent);
        motorDir.move(percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void leftSmooth(char percent=100, int period=0)
    {
        motorEsq.stop();
        motorDir.move(percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void right(char percent=100, int period=0)
    {
        motorEsq.move(percent);
        motorDir.move(-percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void rightSmooth(char percent=100, int period=0)
    {
        motorEsq.move(percent);
        motorDir.stop();
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void stop()
    {
        motorEsq.stop();
        motorDir.stop();
    }
    void refresh()
    {
        motorEsq.refresh();
        motorDir.refresh();
    }
    void inch(bool goForward=true)
    {
        forward((goForward ? 100 : -100), eeprom.dados.delays.mvPol);
    }
    void vetorial(int x, int y);
    void vetorialSensor(Vetor2i intencao);
    void turnLeft()
    {
        left(100, eeprom.dados.delays.mv90);
    }
    void turnRight()
    {
        right(100, eeprom.dados.delays.mv90);
    }
}
drive, drive2;

void Drive::vetorial(int x, int y)
{
    // protecao range
    x = constrain(x, -100, 100);
    y = constrain(y, -100, 100);

    /* as equacoes de interpolacao dos movimentos de giro a esquerda
       e a direita estao divididas em 4 quadrantes:

              y
              |
     Q2 (-,+) | Q1 (+,+)
    __________|__________ x
              |
     Q3 (-,-) | Q4 (+,-)
              |
    */

    if(y >= 0)
    {
        if(x >= 0)   // Q1 (+,+)
        {
            motorEsq.move( max(x,y) );
            motorDir.move( y - x );
        }
        else        // Q2 (-,+)
        {
            motorDir.move( max(-x,y) );
            motorEsq.move( x + y );
        }
    }
    else // (y < 0)
    {
        if(x < 0)   // Q3(-,-)
        {

            motorEsq.move( min(x,y) );
            motorDir.move( y-x );
        }
        else        // Q4 (+,-)
        {
            motorDir.move( min(-x,y) );
            motorEsq.move( x+y );
        }
    }
}

void Drive::vetorialSensor(Vetor2i intencao)
{
    intencao.x = constrain(intencao.x, -100, 100);
    intencao.y = constrain(intencao.y, -100, 100);

    Serial.print("v ");
    Serial.print(intencao.x);
    Serial.print(" ");
    Serial.print(intencao.y);

    Vetor2i resultante = intencao;

    if( intencao.y > 0 ) // ainda nao ha sensores atras :. soh trata sensores se estiver indo pra frente
    {
        int angulo = 45;    // em relacao ao eixo Y / direcao "frente", sen=cos=0,707
        int seno = 70;      // /=100
        int cosseno = 70;   // /=100
        int infinito = 3000;// limite de distancia infinita
        int escala = 30; // escala pra chegar no 0-100%

        // primeiro inverte e aplica limite
        int esq = constrain( ( infinito - sensorEsquerda->refresh() ) / escala , 0, 100 );
        int dir = constrain( ( infinito - sensorDireita->refresh() ) / escala , 0, 100 );

        Serial.print(" ");
        Serial.print(esq);
        Serial.print(" ");
        Serial.print(dir);

        /*        y
                  |
           Esq \  |  / Dir
                \ | /
           ______V|V_____ x
                  |
                  |
                  |
                  V obstaculos
        */

        Vetor2i obstaculos;

        obstaculos.x = constrain( ( ( seno * esq ) / 100 ) - ( ( seno * dir ) / 100 ), -100, 100);
        obstaculos.y = constrain( -( ( cosseno * esq ) / 100 ) - ( ( cosseno * dir ) / 100 ), -100, 0 );

        resultante += obstaculos ;

        resultante.x = constrain( resultante.x, 0, 100 );
        resultante.y = constrain( resultante.y, 0, 100 );

        // TRACE

        Serial.print(" ");
        Serial.print(obstaculos.x);
        Serial.print(" ");
        Serial.print(obstaculos.y);
        //    Serial.print("");
        //    Serial.print();



    }

    drive.vetorial(resultante.x, resultante.y);

    Serial.print(" ");
    Serial.print(drive.motorEsq.read());
    Serial.print(" ");
    Serial.print(drive.motorDir.read());
    Serial.println("\n");

    delay(200);
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
        drive.stop();
        drive2.stop();

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

    if(gamepad.botoesEdgeF & BT_X)
        drive.turnLeft();   // vira 90 graus pra esquerda

    if(gamepad.botoesEdgeF & BT_B)
        drive.turnRight();  // vira 90 graus pra direita

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

    if( eeprom.dados.programa == PRG_RC_SERIAL
        || eeprom.dados.programa == PRG_RC )
    {
    #ifdef RODAS_PWM_x4
        if( gamepad.x.getPorcentoAprox() || gamepad.y.getPorcentoAprox() || gamepad.z.getPorcentoAprox() )
            drive.vetorial(gamepad.x.getPorcentoAprox() + gamepad.z.getPorcentoAprox(), -gamepad.y.getPorcentoAprox());
        else
            drive.stop();

        if( gamepad.x.getPorcentoAprox() || gamepad.y.getPorcentoAprox() || gamepad.z.getPorcentoAprox() )
            drive2.vetorial(-gamepad.x.getPorcentoAprox() + gamepad.z.getPorcentoAprox(), -gamepad.y.getPorcentoAprox());
        else
            drive2.stop();
    #else
        if( gamepad.x.getPorcentoAprox() || gamepad.y.getPorcentoAprox() )
        {
            if( gamepad.botoesAgora & BT_RT ) // por seguranca arma ligada desabilita sensores
                drive.vetorial(gamepad.x.getPorcentoAprox(), -gamepad.y.getPorcentoAprox());
            else
                drive.vetorialSensor( Vetor2i(gamepad.x.getPorcentoAprox(), -gamepad.y.getPorcentoAprox()) );
        }
        else
            drive.stop();
    #endif
    }
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
        drive.left();
    else if ( (ldrEsq - ldrDir) > threshold )	// vira pra direita
        drive.right();
    else
        drive.forward();
}

// ******************************************************************************
//		LINE FOLLOWER
// ******************************************************************************

#ifdef LINE_FOLLOWER
class LineFollower
{
public:
    LineFollower() : lastError(0), accError(0), nextIter(0)
    {}
    void autoCalibrate();
    void loop();
    char calcError(bool * isIRSensorOverLine);
private:
    char lastError;
    int accError;
    int nextIter;
}
lineFollower;

void LineFollower::loop()
{
    unsigned short IRSensor[NUM_IR_TRACK];
    bool IRSensorOverLine[NUM_IR_TRACK];		// true if sensor is over the line

    // read IR sensor dados and map into IRSensor[]
    for(int x = 0; x < NUM_IR_TRACK; x++)
    {
        IRSensor[x] = analogRead(PINO_FIRST_IR_SENSOR + x);
        IRSensorOverLine[x] = (IRSensor[x] > eeprom.dados.LF_threshold[x]) ^ eeprom.dados.LF_reverseColor;
    }

    // first handle special conditions
    if (IRSensorOverLine[0] && IRSensorOverLine[1] && IRSensorOverLine[2])	// end of line, stop
        drive.stop();
    else
    {
        // regular PID control

        int error = calcError(IRSensorOverLine);

        // Proportional
        int Pterm = eeprom.dados.pid.Kp * error;

        // Integral
        accError += error;

        // TODO (mbs#1#): limitar acumulacao do componente integral
        /*
        if (accErr > MAX_ERROR)
        	accErr = MAX_ERROR;
        else if (accErr < MIN_ERROR)
        	accErr = MIN_ERROR;
        */

        int Iterm = eeprom.dados.pid.Ki * accError;

        // Deritavive
        int Dterm = eeprom.dados.pid.Kd * ( error - lastError );

        int MV = Pterm + Iterm + Dterm;

        drive.motorEsq.move ( (MV < 0) ? (100 + MV) : 100 );
        drive.motorDir.move( (MV > 0) ? (100 - MV) : 100 );

        lastError = error;
    }
}

char LineFollower::calcError(bool * isIRSensorOverLine)
{
    if( isIRSensorOverLine[0] )		// line is to the left
        return -1;
    else if( isIRSensorOverLine[2] )// line is to the right
        return 1;
    else if( isIRSensorOverLine[1] )// centered
        return 0;

    // lost track, redo last correction in the hope the track is just a bit ahead
    return lastError;
}

void LineFollower::autoCalibrate()
{
    unsigned short sensorTrack[NUM_IR_TRACK];
    unsigned short sensorTrackMax[NUM_IR_TRACK];
    unsigned short sensorTrackMin[NUM_IR_TRACK];

    unsigned short sensorOut[NUM_IR_TRACK];
    unsigned short sensorOutMax[NUM_IR_TRACK];
    unsigned short sensorOutMin[NUM_IR_TRACK];

    memset(sensorTrackMax, 0, sizeof(sensorTrackMax));
    memset(sensorTrackMin, 1023, sizeof(sensorTrackMin));
    memset(sensorOutMax, 0, sizeof(sensorOut));
    memset(sensorOutMin, 1023, sizeof(sensorOut));

    Serial.println("Calibrating TRACK");

    // read all sensors at TRACK position

    for(int y=0; y < NUM_IR_TRACK; y++)
    {
        sensorTrack[y] = analogRead(PINO_FIRST_IR_SENSOR + y);

        Serial.print(sensorTrack[y]);
        Serial.print(" ");

        sensorTrackMax[y] = max(sensorTrack[y],sensorTrackMax[y]);
        sensorTrackMin[y] = min(sensorTrack[y],sensorTrackMin[y]);
    }

    Serial.println("\nCalibrate OUTSIDE");

    drive.inch();

    // read all sensors at OUTSIDE position

    for(int y=0; y < NUM_IR_TRACK; y++)
    {
        sensorOut[y] = analogRead(PINO_FIRST_IR_SENSOR + y);

        Serial.print(sensorOut[y]);
        Serial.print(" ");

        sensorOutMax[y] = max(sensorOut[y], sensorOutMax[y]);
        sensorOutMin[y] = min(sensorOut[y], sensorOutMin[y]);
    }

    bool reverseSensor[NUM_IR_TRACK];

    Serial.println("\nThresholds: ");

    for(int y=0; y < NUM_IR_TRACK; y++)
    {
        reverseSensor[y] = ( sensorOut[y] > sensorTrack[y] );
        eeprom.dados.LF_threshold[y] = ( sensorOut[y] + sensorTrack[y] ) / 2;

        Serial.print(eeprom.dados.LF_threshold[y]);
        Serial.print(" ");
    }

    eeprom.dados.LF_reverseColor = reverseSensor[0];

    if(eeprom.dados.LF_reverseColor)
        Serial.println("\nReversed Track Color");
    else
        Serial.println("\nNormal Track Color");

    for(int x=0; x < NUM_IR_TRACK; x++)
    {
        // if one sensor is reversed then all others must also be!
        if (reverseSensor[x] ^ eeprom.dados.LF_reverseColor)
        {
            Serial.print("BAD Reverse ");
            Serial.println(x);
            return;
        }

        if(eeprom.dados.LF_reverseColor)
        {
            if(sensorOutMin[x] <= sensorTrackMax[x])
            {
                Serial.print("BAD limits ");
                Serial.println(x);
                return;
            }
        }
        else
        {
            if(sensorOutMax[x] >= sensorTrackMin[x])
            {
                Serial.print("BAD limits ");
                Serial.println(x);
                return;
            }
        }
    }
}
#endif

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
        { pan.write( (SCANNER_ANG/2) + stepAtual * SCANNER_ANG ); }
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
        Serial.print(VAR_AS);
        Serial.print(" ");
    }
    for (int x = 0; x < NUM_SENSORES; x++)
    {
        Serial.print(sensores[x].refresh());
        Serial.print(" ");
    }
    Serial.println("");
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
    Serial.print(lastError);
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
    Serial.print(pan.read());
    Serial.print(" ");
    Serial.print(tilt.read());
    Serial.print(" ");
    Serial.print(roll.read());
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
class Server
{
public:
    Server() : pos(0) {}
    bool receive();
    void loop();
private:
    char command[MAX_CMD];
    short pos;
}
server;

bool Server::receive()
{
    while(Serial.available() > 0)
    {
        char c = Serial.read();

        if (pos == MAX_CMD)
        {
            pos = 0;
            Serial.println("ERRO_TAM_MAX_CMD");
            lastError = ERRO_TAM_MAX_CMD;
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

void Server::loop()
{
    if( server.receive() )
    {
        char *tok;

        char *pqp;	// algumas avr-libc linux naum tem strtok():
        #define STRTOK(a, b) strtok_r(a, b, &pqp)

        if((tok = STRTOK(command, " ")))					// primeiro token eh o comando/acao
        {
            if(strcmp(tok, CMD_WRITE) == 0)					// atribui um valor a uma variavel
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
                            drive.motorEsq.write(valor);
                        else if(strcmp(dest, VAR_RODA_DIR) == 0)
                            drive.motorDir.write(valor);
                        else if(strcmp(dest, VAR_ZERO_ESQ) == 0)
                            drive.motorEsq.setCenter(eeprom.dados.motorEsq.centro = valor);
                        else if(strcmp(dest, VAR_ZERO_DIR) == 0)
                            drive.motorDir.setCenter(eeprom.dados.motorDir.centro = valor);
                        else if(strcmp(dest, VAR_ZERO_ESQ_T) == 0)
                            drive2.motorEsq.setCenter(eeprom.dados.motorEsqT.centro = valor);
                        else if(strcmp(dest, VAR_ZERO_DIR_T) == 0)
                            drive2.motorDir.setCenter(eeprom.dados.motorDirT.centro = valor);
                        else if(strcmp(dest, VAR_PROGRAMA) == 0)
                        {
                            drive.stop();
                            eeprom.dados.programa = valor;
                        }
                        else if(strcmp(dest, VAR_T_POL) == 0)
                            eeprom.dados.delays.mvPol = valor;
                        else if(strcmp(dest, VAR_T_90) == 0)
                            eeprom.dados.delays.mv90 = valor;
                        else if(strcmp(dest, VAR_T_RF) == 0)
                            eeprom.dados.delays.reads = valor;
                        else if(strcmp(dest, VAR_SERVO_X) == 0)
                            pan.write(valor);
                        else if(strcmp(dest, VAR_SERVO_Y) == 0)
                            tilt.write(valor);
                        else if(strcmp(dest, VAR_SERVO_Z) == 0)
                            roll.write(valor);
                        else if(strcmp(dest, VAR_PID) == 0)
                        {
                            eeprom.dados.pid.Kp = valor;		// P
                            if((tok = STRTOK(NULL, " ")))	// I
                                eeprom.dados.pid.Ki = atoi(tok);
                            if((tok = STRTOK(NULL, " ")))	// D
                                eeprom.dados.pid.Kd = atoi(tok);
                        }
                        else if(strcmp(dest, VAR_FREIO) == 0)
                            eeprom.dados.handBrake = valor;
                        else if(strcmp(dest, VAR_ACEL_ESQ) == 0)
                            drive.motorEsq.setAceleracao(eeprom.dados.motorEsq.aceleracao = valor);
                        else if(strcmp(dest, VAR_ACEL_DIR) == 0)
                            drive.motorDir.setAceleracao(eeprom.dados.motorDir.aceleracao = valor);
                        else if(strcmp(dest, VAR_T_ST) == 0)
                            eeprom.dados.delays.status = (unsigned short)valor;
                        else if(strcmp(dest, VAR_T_SE) == 0)
                            eeprom.dados.delays.sensores = (unsigned short)valor;
                    }
                }
            }
            else if(strcmp(tok, CMD_READ) == 0)	// le uma variavel
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
                    else if(strcmp(tok, VAR_T_POL) == 0)
                        Serial.println(eeprom.dados.delays.mvPol);
                    else if(strcmp(tok, VAR_T_90) == 0)
                        Serial.println(eeprom.dados.delays.mv90);
                    else if(strcmp(tok, VAR_T_RF) == 0)
                        Serial.println(eeprom.dados.delays.reads);
                    else if(strcmp(tok, VAR_SERVO_X) == 0)
                        Serial.println(pan.read());
                    else if(strcmp(tok, VAR_SERVO_Y) == 0)
                        Serial.println(tilt.read());
                    else if(strcmp(tok, VAR_SERVO_Z) == 0)
                        Serial.println(roll.read());
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
                }
            }
            else if(strcmp(tok, CMD_SAVE) == 0)	// salva temporarios na EEPROM
                eeprom.save();
            else if(strcmp(tok, CMD_LOAD) == 0)	// descarta mudancas e recarrega da EEPROM
            {
                eeprom.load();
                drive.stop();
            }
            else if(strcmp(tok, CMD_DEFAULT) == 0)  // hard-coded
            {
                eeprom.defaults();
                drive.stop();
            }
            else if(strcmp(tok, CMD_CAL) == 0)	// re-calibra sensores do line follower
                gamepad.calibrar();
            #ifdef LINE_FOLLOWER
            else if(strcmp(tok, CMD_LF_CAL) == 0)	// re-calibra sensores do line follower
                lineFollower.autoCalibrate();
            #endif
            else if(strcmp(tok, CMD_MV_INCH) == 0)
                drive.inch();
            else if(strcmp(tok, CMD_MV_STOP) == 0)
            {
                drive.stop();
                drive2.stop();
                eeprom.dados.programa = PRG_RC_SERIAL;
            }
            else if(strcmp(tok, CMD_MV_WHEELS) == 0)
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
            else if(strcmp(tok, CMD_MV_VECT) == 0)
            {
                eeprom.dados.programa = PRG_RC_SERIAL;
                if ((tok = STRTOK(NULL, " ")))			// segundo token eh o percentual de potencia p/ eixo X
                {
                    int x = atoi(tok);
                    if ((tok = STRTOK(NULL, " ")))		// terceiro token eh o percentual de potencia p/ eixo Y
                    {
                        int y = atoi(tok);

                        if( x || y )
                            drive.vetorial(x, y);
                        else
                            drive.stop();

                        if ((tok = STRTOK(NULL, " ")))  // segundo token eh o percentual de potencia p/ eixo X
                        {
                            int xt = atoi(tok);
                            if ((tok = STRTOK(NULL, " ")))// terceiro token eh o percentual de potencia p/ eixo Y
                            {
                                int yt = atoi(tok);

                                if( xt || yt )
                                    drive2.vetorial(xt, yt);
                                else
                                    drive2.stop();
                            }
                        }
                    }
                }
            }
            else if(strcmp(tok, CMD_TURN_LEFT) == 0)
                drive.turnLeft();
            else if(strcmp(tok, CMD_TURN_RIGHT) == 0)
                drive.turnRight();
            else if(strcmp(tok, CMD_STATUS) == 0)
                enviaStatus();
            else if(strcmp(tok, CMD_UNAME) == 0)
                uname();
            else if(strcmp(tok, CMD_BIP) == 0)
            {
                tok = STRTOK(NULL, " ");
                if (tok)			        // frequencia
                {
                    int hz = atoi(tok);
                    tok = STRTOK(NULL, " ");
                    if (tok)                // duracao
                        BEEP(hz, atoi(tok));
                    else
                        BEEP(hz, 200);
                }
            }
            else if(strcmp(tok, CMD_CLEAR_ERR) == 0)
            {
                lastError = SUCCESSO;
            }
            else if(strcmp(tok, CMD_JOYPAD) == 0)
            {
                if ((tok = STRTOK(NULL, " ")))			        // segundo token eh o status dos botoes
                {
                    gamepad.refreshBotoes(atoi(tok));
                    if ((tok = STRTOK(NULL, " ")))		        // terceiro token eh o eixo X
                    {
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

// ******************************************************************************
//		SETUP
// ******************************************************************************
void setup()
{
    Serial.begin(115200);
    uname();

    eeprom.load();

#ifndef RODAS_PWM
    drive.motorEsq.initServo(PINO_MOTOR_ESQ, eeprom.dados.motorEsq.centro);
    drive.motorDir.initServo(PINO_MOTOR_DIR, eeprom.dados.motorDir.centro, true);
#else
    drive.motorEsq.initDC(PINO_MOTOR_ESQ_PWM, PINO_MOTOR_ESQ, eeprom.dados.motorEsq.centro, eeprom.dados.motorEsq.aceleracao, MOTOR_ESQ_INV);
    drive.motorDir.initDC(PINO_MOTOR_DIR_PWM, PINO_MOTOR_DIR, eeprom.dados.motorDir.centro, eeprom.dados.motorDir.aceleracao, MOTOR_DIR_INV);
    #ifdef PINO_MOTOR_ESQ_N
        drive.motorEsq.setN(PINO_MOTOR_ESQ_N);
    #endif
    #ifdef PINO_MOTOR_DIR_N
        drive.motorDir.setN(PINO_MOTOR_DIR_N);
    #endif
    #ifdef RODAS_PWM_x4
        drive2.motorEsq.initDC(PINO_MOTOR_ESQ_T_PWM, PINO_MOTOR_ESQ_T, eeprom.dados.centroMotorEsqT, eeprom.dados.motorEsqT.aceleracao, MOTOR_E_T_INV);
        drive2.motorDir.initDC(PINO_MOTOR_DIR_T_PWM, PINO_MOTOR_DIR_T, eeprom.dados.motorDirT.centro, eeprom.dados.motorDirT.aceleracao, MOTOR_D_T_INV);
    #endif
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

#ifdef WIICHUCK_POWER
    nunchuck_setpowerpins();
#endif

#ifdef WIICHUCK
    nunchuck_init();
#endif

    for(int s=0; s<NUM_SENSORES; s++)
        sensores[s].setConfig(&eeprom.dados.sensores[s]);

//#ifdef VERSAO_PLACA == 22
    sensorEsquerda = &sensores[1];
    sensorDireita = &sensores[2];
    sensorFrente= &sensores[3];
/*
#else
    sensorFrente = &sensores[1];
    sensorDireita = &sensores[2];
    sensorEsquerda = &sensores[3];
#endif
*/
    gamepad.x.setConfig(&eeprom.dados.gameX);
    gamepad.y.setConfig(&eeprom.dados.gameY);
    gamepad.z.setConfig(&eeprom.dados.gameZ);
    gamepad.r.setConfig(&eeprom.dados.gameR);

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
}

// ******************************************************************************
//		for(;;) ...
// ******************************************************************************
void loop()
{
    agora = millis();

    server.loop();

    trataJoystick();

    static unsigned long ultimoStatus = 0;
    if( delaySemBlock(&ultimoStatus, eeprom.dados.delays.status) )
    {
        enviaStatus();
        digitalWrite(PINO_LED, !digitalRead(PINO_LED));
    }

    static unsigned long ultimoSensores = 0;
    if( delaySemBlock(&ultimoSensores, eeprom.dados.delays.sensores) )
    {
        enviaJoystick();
        enviaSensores();
        #ifdef WIICHUCK
            nunchuck_print_data();
        #endif
    }

    static unsigned short msExec = 10;
    static unsigned long ultimaExec = 0;
    if( delaySemBlock(&ultimaExec, msExec) )
    {
        switch(eeprom.dados.programa)
        {
        case PRG_RC:
        case PRG_RC_SERIAL:
        {
            drive.refresh();
            #ifdef RODAS_PWM_x4
                drive2.refresh();
            #endif
        }
        msExec = 1;
        break;

        case PRG_FOTOVORO:
            fotovoro();
        msExec = eeprom.dados.delays.reads;
        break;

        #ifdef LINE_FOLLOWER
        case PRG_LINE_FOLLOW:
            lineFollower.loop();
        msExec = eeprom.dados.delays.reads;
        break;
        #endif

        #ifdef SCANNER
        case PRG_SCANNER:
            scanner.fillArray();
        msExec = eeprom.dados.delays.reads;
        break;
        case PRG_CHASE:
            scanner.chase();
        msExec = eeprom.dados.delays.reads;
        break;
        #endif

        case PRG_COLLISION:
        {
            const int minDist = 200;
            const int timeAlarm = 3;	// segundos pra colisao
            const int s0 = 700;			// posicao da colisao

            // ignora objetos muito longe
            if(sensorFrente->refresh() > minDist)
            {
                // calc velocidade em unidades sensor / segundo
                int v = ((long)sensorFrente->delta() * 1000) / eeprom.dados.delays.reads;

                // s = s0 + v * t;
                int timeToCollision = (s0 - sensorFrente->getValor()) / v;

                if(timeToCollision < timeAlarm)
                    eeprom.dados.programa = PRG_ALARME;
            }

        }
        msExec = eeprom.dados.delays.reads;
        break;

        case PRG_SENTINELA:
        {
            sensorFrente->refresh();
            if( abs(sensorFrente->delta() ) > 30 )
                eeprom.dados.programa = PRG_ALARME;
        }
        msExec = eeprom.dados.delays.reads;
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
                drive.stop();

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
        msExec = eeprom.dados.delays.reads;
        eeprom.dados.delays.sensores = 0;
        eeprom.dados.delays.status = 0;
        break;

        case PRG_KNOB:
            pan.write(map(analogRead(0), 0, 1023, 5, 174));
        msExec = 50;
        break;

        case PRG_NAV_3S:
        {
            sensorFrente->refresh();
            sensorEsquerda->refresh();
            sensorDireita->refresh();

            static char palpite = 0; // pra seguir girando pra um lado ateh encontrar um caminho livre

            #define MARGEM_SHARP 200
            #define MARGEM_PING 400

            if( sensorFrente->ehMinimo(MARGEM_PING) )
            {
                if( ! palpite )
                    palpite = constrain((sensorDireita->getReta()-sensorEsquerda->getReta()), -1, 1);

                if(palpite < 0)
                    drive.left(50);
                else
                    drive.right(50);
            }
            else if( sensorEsquerda->ehMinimo(MARGEM_SHARP) )
                drive.right(50);
            else if( sensorDireita->ehMinimo(MARGEM_SHARP) )
                drive.left(50);
            else
            {
                // ceu de brigadeiro
                drive.forward(100);
                palpite = 0;
            }

        }
        msExec = eeprom.dados.delays.reads;
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
                    BEEP(tom,SIRENE_COMPASSO);
                }

                for(int tom=SIRENE_TOM_MAX;tom>SIRENE_TOM_MIN;tom-=SIRENE_PASSO)
                {
                    BEEP(tom,SIRENE_COMPASSO/2);
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
            if( lastError != ERRO_PRG_INVALIDO )
            {
                lastError = ERRO_PRG_INVALIDO;
                Serial.print("ERRO_PRG_INVALIDO ");
                Serial.println(eeprom.dados.programa);
                eeprom.dados.programa = PRG_RC;
            }
        break;
        }
    }
}
