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

// ANSI
#include <stdio.h>
#include <ctype.h>
#include <string.h>

// Arduino
#include <Servo.h>

// ******************************************************************************
//		DEFINEs e SETUP
// ******************************************************************************

#include "protocolo.h"
#include "placa.h"

// I2C / wiichuck
#ifdef WIICHUCK
    #include <Wire.h>
    #include "nunchuck_funcs.h"
#endif

// speaker
#ifdef PINO_BIP
#define BEEP(freq, dur) tone(PINO_BIP,freq,dur)
#else
#define BEEP(freq, dur)
#endif

// range finder
#define RF_NUMBER_STEPS 30
#define SHARP_TRESHOLD 300


// ******************************************************************************
//		Plin-plin
// ******************************************************************************

Servo pan;
Servo tilt;
Servo roll;

enum Errors lastError = SUCCESS;

// ******************************************************************************
//		EEPROM
// ******************************************************************************
#include <avr/eeprom.h>

class Eeprom
{
public:
    struct sConfigurationData
    {
        short programa;

        char handBrake;

        short centroMotorEsq;
        short centroMotorDir;

        short centroMotorEsqT;
        short centroMotorDirT;

        short acelMotorEsq;
        short acelMotorDir;

        short acelMotorEsqT;
        short acelMotorDirT;

//        short balancoEsqDir;  // ajuste balanco rodas esquerda/direita

        struct sMoveDelays  // duracao (ms) de movimentos pra animacao
        {
            short inch;		// tempo pra mover 1 polegada
            short right;	// tempo pra girar 90 graus (angulo reto)
        } mvDelay;

        // line follower sensor array parameters(from auto-cal)
        unsigned short LF_threshold[NUM_IR_TRACK];
        bool LF_reverseColor;

        struct sPID
        {
            int Kp;		// proportional
            int Ki;		// integral
            int Kd;		// derivative
        } pid;

        // delay to allow the servo to position between reads
        short RF_delay_reads;

        struct sJoyCenter
        {
            int x;
            int y;
        } joyCenter;
    } data;
    void load()
    {
        char * dest = (char*) &data;
        for(unsigned int addr = 0; addr < sizeof(data); addr++, dest++ )
            *dest = eeprom_read_byte((unsigned char *) addr);
    }
    void save()
    {
        char * dest = (char*) &data;
        for(unsigned int addr = 0; addr < sizeof(data); addr++, dest++ )
            eeprom_write_byte((unsigned char *) addr, *dest);
    }
    void loadDefault()
    {
        data.programa = PRG_RC;
        data.handBrake = 1;

        #ifdef WHEEL_DC
        data.centroMotorEsq = 70;
        data.centroMotorDir = 70;
        data.centroMotorEsqT = 70;
        data.centroMotorDirT = 70;
        #else
        data.centroMotorEsq = 1410;
        data.centroMotorDir = 1384;
        #endif

        data.acelMotorEsq = 100;
        data.acelMotorDir = 100;
        data.acelMotorEsqT = 100;
        data.acelMotorDirT = 100;

        data.mvDelay.inch = 200;
        data.mvDelay.right = 400;
//        data.balancoEsqDir = 0;
        for(int x = 0; x < NUM_IR_TRACK; x++)
        {
            data.LF_threshold[x] = 512;
            data.LF_reverseColor = false;
        }
        data.pid.Kp = 100;
        data.pid.Ki = 0;
        data.pid.Kd = 0;
        data.RF_delay_reads = 100;
        data.joyCenter.x = 174;
        data.joyCenter.y = 174;
    }
} eeprom;

// ******************************************************************************
//		Sensor universal
// ******************************************************************************
class Sensor
{
public:
    unsigned char pino;
    bool invertido;
    unsigned short valor, minimo, maximo;
    int a, b; // = a*x + b
    enum eTipoSensor { SENSOR_ANALOGICO, SENSOR_PING, SENSOR_VIRTUAL } tipo;

    Sensor(unsigned char  pin=0, eTipoSensor t=SENSOR_ANALOGICO, bool inverso = false)
        { init(pin, t, inverso); }
    void init(unsigned char  pin=0, eTipoSensor t=SENSOR_ANALOGICO, bool inverso = false)
        {
            pino = pin;
            tipo = t;
            invertido = inverso;
            minimo = 65535;
            valor = maximo = 0;
            if(invertido) a = -1; else a = 1;
            b = 0;
        }
    unsigned short getValor()
        { return valor; }
    unsigned short setValor(unsigned short v)
        {
            valor = v;
            if (valor < minimo) minimo = valor;
            if (valor > maximo) maximo = valor;
            return valor;
        }
    unsigned short refresh()
        {
            switch(tipo)
            {
                case SENSOR_ANALOGICO:
                    setValor(analogRead(pino));
                break;
                case SENSOR_PING:
                    // manda pulso de 2ms pro ping))) pra acionar leitura
                    pinMode(pino, OUTPUT);
                    digitalWrite(pino, LOW);
                    delayMicroseconds(2);
                    digitalWrite(pino, HIGH);
                    delayMicroseconds(5);
                    digitalWrite(pino, LOW);

                    // duracao do pulso = distancia
                    pinMode(pino, INPUT);
                    setValor(pulseIn(pino, HIGH));
                default:
                break;
            }
            return valor;
        }
    bool ehMinimo(unsigned short margem = 0)
        { return ( invertido ? (maximo - valor) <= margem : (valor - minimo) <= margem ); }
    bool ehMaximo(unsigned short margem = 0)
        { return ( invertido ? (valor - minimo) <= margem : (maximo - valor) <= margem ); }
    void setReta(int aa, int bb)
        { a = aa; b = bb; }
    int getReta()
        { return ( a * valor + b ); }
};

Sensor sensores[6], *sensorFrente, *sensorEsquerda, *sensorDireita;

// ******************************************************************************
//		Controlador de motoree
// ******************************************************************************
// TODO (mbs#1#): Acabar com essa frescura de polimorfismo e deixar igual o Sensor: com tipo e switch.
class Motor // interface
{
public:
    virtual void stop();
    virtual void move(char percent);
    virtual void refresh();
    void write(int valor)
    {
        atual = valor;
        refresh();
    }
    short read()
    {
        return atual;
    }
    void setCenter(short valor)
    {
        centro = valor;
    }
    void setReverse(bool rev)
    {
        invertido = rev;
    }
    void setAceleracao(short acel)
    {
        aceleracao = acel;
    }
protected:
    short atual;
    short centro;
    short aceleracao;
    short meta;
    bool invertido;
};

#ifndef WHEEL_DC

class MotorServo : public Motor
{
public:
    void init(int pin, short centerAng=1500, bool reverseDirection=false)
    {
        servo.attach(pin);
        setCenter(centerAng);
        setReverse(reverseDirection);
        stop();
    }
    virtual void stop()
    {
        atual = centro;
        refresh();
    }
    virtual void move(char percent)
    {
        // typical servos use from 1000-2000us
        atual = invertido ? (centro - percent*5) : (centro + percent*5);
        refresh();
    }
    virtual void refresh()
    {
        if(eeprom.data.handBrake)
            atual = centro;
        // protect against out of range values
        else if (atual < 1000)
            atual = 1000;
        else if (atual > 2000)
            atual = 2000;

        servo.writeMicroseconds(atual);
    }
private:
    Servo servo;
}
motorEsq, motorDir;

#else

class MotorDC : public Motor
{
public:
    void init(int PWMpin, int DIRpin, short offsetZero=0, short acel=255, bool reverseDirection=false)
    {
        pwm = PWMpin;
        pinMode(pwm, OUTPUT);
        analogWrite(pwm, 0);

        dir = DIRpin;
        pinMode(dir, OUTPUT);
        digitalWrite(dir, 0);

        setCenter(offsetZero);
        setReverse(reverseDirection);
        aceleracao = acel;
        stop();
    }

    virtual void stop()
    {
        meta = atual = 0;
        refresh();
    }

    virtual void move(char potencia100)
    {
        /* onde:
            potencia100 = +/- 0-100 %
            centro = pwm a partir do qual o motor comeca a se mover
        */
        if ( potencia100 )
        {
            short c = potencia100 > 0 ? centro : -centro; // centro com sinal

            meta = c + ( potencia100 * 5 ) / 2; // converte % de potencia em pwm 8 bits

            meta = constrain(meta, -255, 255); // protecao de range
        }
        else
            meta = 0;

        refresh();
    }

    virtual void refresh()
    {
        // TODO (mbs#1#): Delimitar tempo entre os passos de aceleracao

        if ( abs(atual) < centro )
            atual = 0;

        if ( meta > atual)
        {
            if( ! atual ) // estava parado
                atual = centro;
            else
                atual += aceleracao;

            if( meta < atual) // passou do ponto
                atual = meta;
        }
        else if ( meta < atual)
        {
            if( ! atual ) // estava parado
                atual = -centro;
            else
                atual -= aceleracao;

            if ( meta > atual)
                atual = meta;
        }

        // uma ultima olhada no freio de mao
        if(eeprom.data.handBrake)
            meta = atual = 0;

        // direcao
        digitalWrite(dir, (atual < 0) ^ invertido ? HIGH : LOW);

        // potencia
        analogWrite(pwm, abs(atual));
    }
private:
    int pwm; // pino PWM ( no atmega328)
    int dir; // pino digital comum do Arduino
}
#ifdef WHEEL_DC_4WD
motorEsq, motorDir, motorEsqT, motorDirT;
#else
motorEsq, motorDir;
#endif

#endif

class Drive
{
public:
    Motor *motorEsq;
    Motor *motorDir;
    void forward(char percent=100, int period=0)
    {
        motorEsq->move(percent);
        motorDir->move(percent);
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
        motorEsq->move(-percent);
        motorDir->move(percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void leftSmooth(char percent=100, int period=0)
    {
        motorEsq->stop();
        motorDir->move(percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void right(char percent=100, int period=0)
    {
        motorEsq->move(percent);
        motorDir->move(-percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void rightSmooth(char percent=100, int period=0)
    {
        motorEsq->move(percent);
        motorDir->stop();
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void stop()
    {
        motorEsq->stop();
        motorDir->stop();
    }
    void refresh()
    {
        motorEsq->refresh();
        motorDir->refresh();
    }
    void inch(bool goForward=true)
    {
        forward((goForward ? 100 : -100), eeprom.data.mvDelay.inch);
    }
    void vetorial(int x, int y);
    void turnLeft()
    {
        left(100, eeprom.data.mvDelay.right);
    }
    void turnRight()
    {
        right(100, eeprom.data.mvDelay.right);
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
            motorEsq->move( max(x,y) );
            motorDir->move( y - x );
        }
        else        // Q2 (-,+)
        {
            motorDir->move( max(-x,y) );
            motorEsq->move( x + y );
        }
    }
    else // (y < 0)
    {
        if(x < 0)   // Q3(-,-)
        {

            motorEsq->move( min(x,y) );
            motorDir->move( y-x );
        }
        else        // Q4 (+,-)
        {
            motorDir->move( min(-x,y) );
            motorEsq->move( x+y );
        }
    }
}

// ******************************************************************************
//		FOTOVORO
// ******************************************************************************
void photovore()
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

    delay(50);
}

// ******************************************************************************
//		LINE FOLLOWER
// ******************************************************************************
class LineFollower
{
public:
    LineFollower() : lastError(0), accError(0), nextIter(0)
    {}
    void autoCalibrate();
    void loop();
    void readSensors(bool * isIRSensorOverLine);
    char calcError(bool * isIRSensorOverLine);
private:
    char lastError;
    int accError;
    int nextIter;
}
lineFollower;

void LineFollower::loop()
{
    bool IRSensorOverLine[NUM_IR_TRACK];		// true if sensor is over the line

    readSensors(IRSensorOverLine);

    // first handle special conditions

    if (IRSensorOverLine[0] && IRSensorOverLine[1] && IRSensorOverLine[2])	// end of line, stop
        drive.stop();
    else
    {
        // regular PID control

        int error = calcError(IRSensorOverLine);

        // Proportional
        int Pterm = eeprom.data.pid.Kp * error;

        // Integral
        accError += error;

        // TODO (mbs#1#): limitar acumulacao do componente integral
        /*
        if (accErr > MAX_ERROR)
        	accErr = MAX_ERROR;
        else if (accErr < MIN_ERROR)
        	accErr = MIN_ERROR;
        */

        int Iterm = eeprom.data.pid.Ki * accError;

        // Deritavive
        int Dterm = eeprom.data.pid.Kd * ( error - lastError );

        int MV = Pterm + Iterm + Dterm;

        drive.motorEsq->move ( (MV < 0) ? (100 + MV) : 100 );
        drive.motorDir->move( (MV > 0) ? (100 - MV) : 100 );

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

void LineFollower::readSensors(bool * isIRSensorOverLine)
{
    unsigned short IRSensor[NUM_IR_TRACK];

    // read IR sensor data and map into IRSensor[]
    for(int x = 0; x < NUM_IR_TRACK; x++)
    {
        IRSensor[x] = analogRead(PINO_FIRST_IR_SENSOR + x);
        isIRSensorOverLine[x] = (IRSensor[x] > eeprom.data.LF_threshold[x]) ^ eeprom.data.LF_reverseColor;
    }
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
        eeprom.data.LF_threshold[y] = ( sensorOut[y] + sensorTrack[y] ) / 2;

        Serial.print(eeprom.data.LF_threshold[y]);
        Serial.print(" ");
    }

    eeprom.data.LF_reverseColor = reverseSensor[0];

    if(eeprom.data.LF_reverseColor)
        Serial.println("\nReversed Track Color");
    else
        Serial.println("\nNormal Track Color");

    for(int x=0; x < NUM_IR_TRACK; x++)
    {
        // if one sensor is reversed then all others must also be!
        if (reverseSensor[x] ^ eeprom.data.LF_reverseColor)
        {
            Serial.print("BAD Reverse ");
            Serial.println(x);
            return;
        }

        if(eeprom.data.LF_reverseColor)
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

// ******************************************************************************
//		SHARP IR RANGE FINDER
// ******************************************************************************
#define RF_STEP_ANGLE ( 180 / RF_NUMBER_STEPS )
#define RF_SERVO_SAFE_ANGLE ( RF_STEP_ANGLE / 2 )

class RangeFinder
{
public:
    RangeFinder() : nextRead(0), currentStep(0), stepDir(1), currValue(0), lastValue(0)
    {}
    short readSensor()
    {
        lastValue = currValue;
        return currValue = analogRead(PINO_SHARP_RF);
    }
    bool stepUp();
    bool stepDown();
    void refreshServo()
    {
        pan.write( RF_SERVO_SAFE_ANGLE + currentStep * RF_STEP_ANGLE );
    }
    void chase();
    void fillArray();
    bool step()
    {
        return (stepDir < 0) ? stepDown() : stepUp();
    }
    void reverseDir()
    {
        stepDir = -stepDir;
    }
    bool lowerBound()
    {
        return currentStep <= 0;
    }
    bool upperBound()
    {
        return currentStep >= (RF_NUMBER_STEPS-1);
    }
    bool delayRead();
    bool collision();
    bool sentry();
private:
    unsigned long nextRead;
    short currentStep;
    char stepDir;
    short dataArray[RF_NUMBER_STEPS];
    short currValue;
    short lastValue;
} rangeFinder;

bool RangeFinder::delayRead()
{
    if ( millis() > nextRead )
    {
        nextRead = millis() + eeprom.data.RF_delay_reads;
        return true;
    }
    return false;
}

bool RangeFinder::stepUp()
{
    if(upperBound())
        currentStep = (RF_NUMBER_STEPS-1);
    else
        currentStep++;
    return upperBound();
}

bool RangeFinder::stepDown()
{
    if(lowerBound())
        currentStep = 0;
    else
        currentStep--;
    return lowerBound();
}

void RangeFinder::chase()
{
    if( delayRead() )
    {
        if( rangeFinder.readSensor() > SHARP_TRESHOLD )
        {
            if(stepDir < 0)
                rangeFinder.stepDown();
            else
                rangeFinder.stepUp();
        }
        else //no object detected
        {
            if(stepDir < 0)
                rangeFinder.stepUp();
            else
                rangeFinder.stepDown();
        }

        if(upperBound() || lowerBound())
            reverseDir();

        refreshServo();
    }

    /*
    if scanner is pointing far left
    	turns left
    else if scanner is pointing far right
    	turns right
    else //scanner pointing forward
    	drives straight
    */
}

void RangeFinder::fillArray()
{
    if( delayRead() )
    {
        // read sensor into array
        dataArray[currentStep] = readSensor();

        // calc next move and check boundaries
        if( step() )
        {
            // step() returns true if reached left or right most
            // reverse next move
            reverseDir();

            // array is complete, send debug values
            Serial.print("RFA ");
            for(int x = 0; x < RF_NUMBER_STEPS; x++)
            {
                Serial.print(dataArray[x]);
                Serial.print(" ");
            }
            Serial.println("");
        }

        // move
        refreshServo();
    }
}

bool RangeFinder::collision()
{
    const int minDist = 200;	// in sensor units
    const int timeAlarm = 3;	// seconds to collision
    const int s0 = 700;			// collision position

    if(delayRead())
    {
        readSensor();

        // ignore far objects
        if(currValue > minDist)
        {
            // calc delta-S
            int ds = currValue - lastValue;

            // calc velocity in sensor units / second
            int v = (ds * 1000) / eeprom.data.RF_delay_reads;

            // s = s0 + v * t;
            int timeToCollision = (s0 - currValue) / v;

            if(timeToCollision < timeAlarm)
            {
                BEEP(1000, 50);
                return true;
            }
        }
    }
    return false;
}

bool RangeFinder::sentry()
{
    if(delayRead())
    {
        readSensor();

        int threshold = 30;

        if( abs((lastValue - currValue)) > threshold )
            return true;
    }
    return false;
}

// ******************************************************************************
//		DEBUG / SENSORS INFORMATION
// ******************************************************************************
void enviaSensores(bool enviaComando = true)
{
    if(enviaComando)
    {
        Serial.print(VAR_AS);
        Serial.print(" ");
    }
    for (int x = 0; x < 6; x++)
    {
        if(x < PINO_ANALOG_CNT)
            Serial.print(sensores[x].refresh());
        else
            Serial.print("?");
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
    Serial.print(eeprom.data.programa);
    Serial.print(" ");
    Serial.print(lastError);
    Serial.print(" ");
    Serial.print((int)eeprom.data.handBrake);
    Serial.print(" ");
    Serial.print(drive.motorEsq->read());
    Serial.print(" ");
    Serial.print(drive.motorDir->read());
    Serial.print(" ");
    #ifdef WHEEL_DC_4WD
        Serial.print(drive2.motorEsq->read());
        Serial.print(" ");
        Serial.print(drive2.motorDir->read());
        Serial.print(" ");
    #else
        Serial.print("- - ");
    #endif
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

// ******************************************************************************
//		TELNET SERVER
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
} server;

bool Server::receive()
{
    while(Serial.available() > 0)
    {
        char c = Serial.read();

        if (pos == MAX_CMD)
        {
            pos = 0;
            Serial.println("ERR_MAX_CMD_SIZ");
            lastError = ERR_MAX_CMD_SIZ;
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
                            drive.motorEsq->write(valor);
                        else if(strcmp(dest, VAR_RODA_DIR) == 0)
                            drive.motorDir->write(valor);
                        else if(strcmp(dest, VAR_ZERO_ESQ) == 0)
                            drive.motorEsq->setCenter(eeprom.data.centroMotorEsq = valor);
                        else if(strcmp(dest, VAR_ZERO_DIR) == 0)
                            drive.motorDir->setCenter(eeprom.data.centroMotorDir = valor);
                        else if(strcmp(dest, VAR_PROGRAMA) == 0)
                        {
                            drive.stop();
                            eeprom.data.programa = valor;
                        }
                        else if(strcmp(dest, VAR_T_POL) == 0)
                            eeprom.data.mvDelay.inch = valor;
                        else if(strcmp(dest, VAR_T_90) == 0)
                            eeprom.data.mvDelay.right = valor;
                        else if(strcmp(dest, VAR_T_RF) == 0)
                            eeprom.data.RF_delay_reads = valor;
                        else if(strcmp(dest, VAR_SERVO_X) == 0)
                            pan.write(valor);
                        else if(strcmp(dest, VAR_SERVO_Y) == 0)
                            tilt.write(valor);
                        else if(strcmp(dest, VAR_SERVO_Z) == 0)
                            roll.write(valor);
                        else if(strcmp(dest, VAR_PID) == 0)
                        {
                            eeprom.data.pid.Kp = valor;		// P
                            if((tok = STRTOK(NULL, " ")))	// I
                                eeprom.data.pid.Ki = atoi(tok);
                            if((tok = STRTOK(NULL, " ")))	// D
                                eeprom.data.pid.Kd = atoi(tok);
                        }
                        else if(strcmp(dest, VAR_FREIO) == 0)
                            eeprom.data.handBrake = valor;
                        else if(strcmp(dest, VAR_ACEL_ESQ) == 0)
                            drive.motorEsq->setAceleracao(eeprom.data.acelMotorEsq = valor);
                        else if(strcmp(dest, VAR_ACEL_DIR) == 0)
                            drive.motorDir->setAceleracao(eeprom.data.acelMotorDir = valor);
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
                        Serial.println(drive.motorEsq->read());
                    else if(strcmp(tok, VAR_ZERO_ESQ) == 0)
                        Serial.println(eeprom.data.centroMotorEsq);
                    else if(strcmp(tok, VAR_RODA_DIR) == 0)
                        Serial.println(drive.motorDir->read());
                    else if(strcmp(tok, VAR_ZERO_DIR) == 0)
                        Serial.println(eeprom.data.centroMotorDir);
                    else if(strcmp(tok, VAR_PROGRAMA) == 0)
                        Serial.println(eeprom.data.programa);
                    else if(strcmp(tok, VAR_T_POL) == 0)
                        Serial.println(eeprom.data.mvDelay.inch);
                    else if(strcmp(tok, VAR_T_90) == 0)
                        Serial.println(eeprom.data.mvDelay.right);
                    else if(strcmp(tok, VAR_T_RF) == 0)
                        Serial.println(eeprom.data.RF_delay_reads);
                    else if(strcmp(tok, VAR_SERVO_X) == 0)
                        Serial.println(pan.read());
                    else if(strcmp(tok, VAR_SERVO_Y) == 0)
                        Serial.println(tilt.read());
                    else if(strcmp(tok, VAR_SERVO_Z) == 0)
                        Serial.println(roll.read());
                    else if(strcmp(tok, VAR_FREIO) == 0)
                        Serial.println((int)eeprom.data.handBrake);
                    else if(strcmp(tok, VAR_AS) == 0)
                        enviaSensores(false);
                    else if(strcmp(tok, VAR_PID) == 0)
                    {
                        Serial.print(eeprom.data.pid.Kp);
                        Serial.print(" ");
                        Serial.print(eeprom.data.pid.Ki);
                        Serial.print(" ");
                        Serial.println(eeprom.data.pid.Kd);
                    }
                    else if(strcmp(tok, VAR_ACEL_ESQ) == 0)
                        Serial.println((int)eeprom.data.acelMotorEsq);
                    else if(strcmp(tok, VAR_ACEL_DIR) == 0)
                        Serial.println((int)eeprom.data.acelMotorDir);
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
                eeprom.loadDefault();
                drive.stop();
            }
            else if(strcmp(tok, CMD_LF_CAL) == 0)	// re-calibrate line following IR sensors
                lineFollower.autoCalibrate();
            else if(strcmp(tok, CMD_MV_INCH) == 0)
                drive.inch();
            else if(strcmp(tok, CMD_MV_STOP) == 0)
            {
                drive.stop();
                drive2.stop();
                eeprom.data.programa = PRG_SHOW_SENSORS;
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
                        drive.motorEsq->move(lw);
                        drive.motorDir->move(atoi(tok));
                    }
                }
            }
            else if(strcmp(tok, CMD_MV_VECT) == 0)
            {
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
            {
                Serial.print("Mbsbot hw ");
                Serial.print(BOARD_VERSION);
                Serial.print(" sw ");
                Serial.println(PROTOCOL_VERSION);
            }
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
                lastError = SUCCESS;
            }
        }
    }
}

// ******************************************************************************
//		SETUP
// ******************************************************************************
void setup()
{
    Serial.begin(115200);
    Serial.println("MBSBOT");

    eeprom.load();

    // liga pull-up de pinos livres pra economizar energia
    int unused[] = PINO_UNUSED_ARRAY;
    for(int p=0; p < PINO_UNUSED_CNT; p++)
    {
        pinMode(unused[p], INPUT);
        digitalWrite(unused[p], HIGH);
    }

#ifndef WHEEL_DC
    motorEsq.init(PINO_MOTOR_ESQ, eeprom.data.centroMotorEsq);
    motorDir.init(PINO_MOTOR_DIR, eeprom.data.centroMotorDir, true);
#else
    motorEsq.init(PINO_MOTOR_ESQ_PWM, PINO_MOTOR_ESQ, eeprom.data.centroMotorEsq, eeprom.data.acelMotorEsq);
    motorDir.init(PINO_MOTOR_DIR_PWM, PINO_MOTOR_DIR, eeprom.data.centroMotorDir, eeprom.data.acelMotorDir);
    #ifdef WHEEL_DC_4WD
        motorEsqT.init(PINO_MOTOR_ESQ_T_PWM, PINO_MOTOR_ESQ_T, eeprom.data.centroMotorEsqT, eeprom.data.acelMotorEsqT);
        motorDirT.init(PINO_MOTOR_DIR_T_PWM, PINO_MOTOR_DIR_T, eeprom.data.centroMotorDirT, eeprom.data.acelMotorDirT);
    #endif
#endif
    // I dont wanna live in this planet anymore
    drive.motorEsq = &motorEsq;
    drive.motorDir = &motorDir;
    #ifdef WHEEL_DC_4WD
    drive2.motorEsq = &motorEsqT;
    drive2.motorDir = &motorDirT;
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

    if (eeprom.data.programa == PRG_LINEFOLLOWER)
        lineFollower.autoCalibrate();

    pinMode(PINO_LED, OUTPUT);
    digitalWrite(PINO_LED, LOW);

#ifdef PINO_LASER
    pinMode(PINO_LASER, OUTPUT);
    digitalWrite(PINO_LASER, LOW);
#endif

#ifdef WIICHUCK_POWER
    nunchuck_setpowerpins();
#endif

#ifdef WIICHUCK
    nunchuck_init();
#endif

    sensores[0].init(); // potenciometro
    sensores[1].init(15, Sensor::SENSOR_PING);
    sensores[2].init(2, Sensor::SENSOR_ANALOGICO, true);
    sensores[3].init(3, Sensor::SENSOR_ANALOGICO, true);

    sensores[1].minimo = 150;
    sensores[2].maximo = 630;
    sensores[3].maximo = 630;

    sensorFrente = &sensores[1];
    sensorDireita = &sensores[2];
    sensorEsquerda = &sensores[3];
}

// ******************************************************************************
//		LOOP PRINCIPAL
// ******************************************************************************

void loop()
{
    bool mandarStatus = true;
    unsigned short dorme_ms = 10;

    server.loop();

    switch(eeprom.data.programa)
    {
    case PRG_SHOW_SENSORS:
        enviaSensores();
        enviaStatus();
        dorme_ms = 50;
        //#ifdef WIICHUCK
        //nunchuck_print_data();
        //#endif

    case PRG_RC:
        drive.refresh();
        drive2.refresh();
        break;

    case PRG_PHOTOVORE:
        photovore();
        break;

    case PRG_LINEFOLLOWER:
        lineFollower.loop();
        break;

    case PRG_SHARP:
        rangeFinder.fillArray();
        break;

    case PRG_SHARP_CHASE:
        rangeFinder.chase();
        break;

    case PRG_COLLISION:
        rangeFinder.collision();
        break;

    case PRG_SENTINELA:
        if(rangeFinder.sentry())
            eeprom.data.programa = PRG_ALARME;
    break;

    #ifdef WIICHUCK
    case PRG_WIICHUCK:
        if(nunchuck_get_data() == 0)
            break;

        // nunchuck_print_data();

        if( ! nunchuck_zbutton())
        {
            //drive.vetorial(map(nunchuck_accelx(),200,700,-100,100),
            //                map(nunchuck_accely(),200,700,-100,100));
            int x = nunchuck_joyx() - eeprom.data.joyCenter.x;
            int y = nunchuck_joyy() - eeprom.data.joyCenter.y;

            // TODO: mapear 0-100 direito
            x *= 10; // joga x lah pra pqp
            y *= 10;

            //Serial.print("vetorial(");
            //Serial.print(x);
            //Serial.print(",");
            //Serial.print(y);
            //Serial.println(")");

            drive.vetorial(x, y);
        }
        else
        {
            drive.stop();

            const int VELOCIDADE_SERVO = 2;

            #ifdef PINO_SERVO_PAN
            if( nunchuck_joyx() < eeprom.data.joyCenter.x )
            {
                int angle = pan.read() + VELOCIDADE_SERVO;
                if(angle > 170) angle = 170;
                pan.write(angle);
            }
            else if( nunchuck_joyx() > eeprom.data.joyCenter.x )
            {
                int angle = pan.read() - VELOCIDADE_SERVO;
                if(angle < 10) angle = 10;
                pan.write(angle);
            }
            #endif

            #ifdef PINO_SERVO_TILT
            if( nunchuck_joyy() > eeprom.data.joyCenter.y )
            {
                int angle = tilt.read() + VELOCIDADE_SERVO;
                if(angle > 170) angle = 170;
                tilt.write(angle);
            }
            else if( nunchuck_joyy() < eeprom.data.joyCenter.y )
            {
                int angle = tilt.read() - VELOCIDADE_SERVO;
                if(angle < 10) angle = 10;
                tilt.write(angle);
            }
            #endif
        }

        if(nunchuck_cbutton())
        {
            eeprom.data.handBrake = 0;
            digitalWrite(PINO_LASER,HIGH);
        }
        else
            digitalWrite(PINO_LASER,LOW);

        dorme_ms = 100;
    break;
    #endif

    case PRG_SCOPE:
        // pra conseguir performance melhor descartamos os 2 bits menos significativos
        // e envia somente um byte
		Serial.write((analogRead(0) >> 2) & 0xFF);
        dorme_ms = eeprom.data.RF_delay_reads;
        mandarStatus = false;
    break;

    case PRG_KNOB:
        pan.write(map(analogRead(0), 0, 1023, 5, 174));
        dorme_ms = 50;
    break;

    case PRG_TEST:
    {
        sensorFrente->refresh();
        sensorEsquerda->refresh();
        sensorDireita->refresh();

        static char palpite = 0; // pra seguir girando pra um lado ateh encontrar um caminho livre

        #define MARGEM_SHARP 200
        #define MARGEM_PING 400
        if( !sensorEsquerda->ehMinimo(MARGEM_SHARP) &&
            !sensorDireita->ehMinimo(MARGEM_SHARP) &&
            !sensorFrente->ehMinimo(MARGEM_PING) ) // ambos livres
        {
            digitalWrite(PINO_LED, LOW);

//            if( sensorEsquerda->ehMinimo(MARGEM_SHARP*3) ||
//                sensorDireita->ehMinimo(MARGEM_SHARP*3) ||
//                sensorFrente->ehMinimo(MARGEM_PING+50) )
//                drive.forward(50);
//            else
                drive.forward(100);

            palpite = 0;
        }
        else
        {
            digitalWrite(PINO_LED, HIGH);

            if( sensorFrente->ehMinimo(MARGEM_PING) )
            {
                if( ! palpite )
                {
                    palpite = constrain((sensorDireita->getReta()-sensorEsquerda->getReta()), -1, 1);
                }
                if(palpite < 0)
                    drive.left(50);
                else
                    drive.right(50);
            }
            else if( sensorEsquerda->ehMinimo(MARGEM_SHARP) )
                drive.right(50);
            else if( sensorDireita->ehMinimo(MARGEM_SHARP) )
                drive.left(50);
        }

        dorme_ms = eeprom.data.RF_delay_reads;
    }
    break;

    case PRG_ALARME:
        digitalWrite(PINO_LED, HIGH);
        Serial.println("ALARM");

        #define SIRENE_TOM_MIN  1000
        #define SIRENE_TOM_MAX  3000
        #define SIRENE_PASSO    2
        #define SIRENE_COMPASSO 150

        for(int x=0; x<1; x++)
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

        //delay(1000);
        digitalWrite(PINO_LED, LOW);
        //delay(1000);
        eeprom.data.programa = PRG_SHOW_SENSORS;
    break;

    default:
        if( lastError != ERR_INVALID_PRG )
        {
            lastError = ERR_INVALID_PRG;
            Serial.println("ERR_INVALID_PRG");
        }
    break;
    }

    if(dorme_ms)
        delay(dorme_ms);

    static unsigned long nextSendStatus = 0;
    if(millis() >= nextSendStatus)
    {
        nextSendStatus += 10000;
        if(mandarStatus)
        {
            //enviaSensores();
            enviaStatus();
        }
    }
}
