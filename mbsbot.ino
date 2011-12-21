/**	Copyright (C) 2010-2011 - Mauricio Bieze Stefani
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
#ifdef PIN_BIP
#define BEEP(freq, dur) tone(PIN_BIP,freq,dur)
#else
#define BEEP(freq, dur)
#endif

// range finder
#define RF_NUMBER_STEPS 30
#define SHARP_TRESHOLD 300

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

        short acelMotorEsq;
        short acelMotorDir;

        short balancoEsqDir;  // ajuste balanco rodas esquerda/direita

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
        for(int addr = 0; addr < sizeof(data); addr++, dest++ )
            *dest = eeprom_read_byte((unsigned char *) addr);
    }
    void save()
    {
        char * dest = (char*) &data;
        for(int addr = 0; addr < sizeof(data); addr++, dest++ )
            eeprom_write_byte((unsigned char *) addr, *dest);
    }
    void loadDefault()
    {
        data.programa = PRG_RC;
        data.handBrake = 1;
        data.centroMotorEsq = 1410;
        data.centroMotorDir = 1384;
        data.mvDelay.inch = 200;
        data.mvDelay.right = 400;
        data.balancoEsqDir = 0;
        for(int x = 0; x < NUM_IR_TRACK; x++)
        {
            data.LF_threshold[x] = 512;
            data.LF_reverseColor = false;
        }
        data.pid.Kp = 100;
        data.pid.Ki = 0;
        data.pid.Kd = 0;
        data.RF_delay_reads = 100;
        data.joyCenter.x = 124;
        data.joyCenter.y = 128;
    }
} eeprom;

class Sensor
{
    unsigned short read();

    unsigned short min, max;
    int pin;
    Sensor(int spin) : pin(spin) {}
};

class Distancia : Sensor
{

};

// ******************************************************************************
//		Controlador de motoree
// ******************************************************************************
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
protected:
    bool invertido;
    short atual;
    short centro;
    short aceleracao;
    short meta;
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
// TODO (mbs#1#): Configurar aceleracao na eeprom
public:
    void init(int PWMpin, int DIRpin, short offsetZero=0, short acel=2, bool reverseDirection=false)
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
        if( potencia100 > 0 )
            meta = centro + (potencia100*5)/2;
        else
            meta = -centro + (potencia100*5)/2;

        if (invertido) meta = -meta;

        refresh();
    }
    virtual void refresh()
    {
        // TODO (mbs#1#): Delimitar tempo entre os passos de aceleracao

        // protecao de range
        if(meta > 255)
            meta = 255;
        else if (meta < -255)
            meta = -255;

        if ( meta > atual)
        {
            atual += aceleracao;

            if( meta < atual) // inverteu sinal = passou do ponto
                atual = meta;
        }
        else if ( meta < atual)
        {
            atual -= aceleracao;

            if ( meta > atual)
                atual = meta;
        }

        // uma ultima olhada no freio de mao
        if(eeprom.data.handBrake)
            meta = atual = 0;

        // direcao
        digitalWrite(dir, (atual < 0) ? HIGH : LOW);

        // potencia
        analogWrite(pwm, abs(atual));
    }
private:
    int pwm; // pino PWM ( no atmega328)
    int dir; // pino digital comum do Arduino
}
motorEsq, motorDir;

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
    void vectorial(int x, int y);
    void turnLeft()
    {
        left(100, eeprom.data.mvDelay.right);
    }
    void turnRight()
    {
        right(100, eeprom.data.mvDelay.right);
    }
}
drive;

void Drive::vectorial(int x, int y)
{
    // protecao range
    if(y > 100)
        y = 100;
    else if(y < -100)
        y = -100;

    if(x > 100)
        x = 100;
    else if(x < -100)
        x = -100;

    // pra interpolar os movimentos de giro dividi td em 4 quadrantes
    if(y > 0)
    {
        if(x > 0)   // Q1
        {
            motorEsq->move( max(x,y) );
            if(y > x)
                motorDir->move( y-x );
            else
                motorDir->move( x-y );
        }
        else        // Q2
        {
            motorDir->move( max(-x,y) );
            if(y > -x)
                motorEsq->move( x+y );
            else
                motorEsq->move( x-y );
        }
    }
    else // (y <= 0)
    {
        if(x < 0)   // Q3
        {

            motorEsq->move( min(x,y) );
            if(x < y)
                motorDir->move( -x-y );
            else
                motorDir->move( y-x );
        }
        else        // Q4
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
        IRSensor[x] = analogRead(PIN_FIRST_IR_SENSOR + x);
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
        sensorTrack[y] = analogRead(PIN_FIRST_IR_SENSOR + y);

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
        sensorOut[y] = analogRead(PIN_FIRST_IR_SENSOR + y);

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
        return currValue = analogRead(PIN_SHARP_RF);
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
    short currentStep;
    char stepDir;
    short dataArray[RF_NUMBER_STEPS];
    unsigned long nextRead;
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
void displayAnalogSensors()
{
    Serial.print("AS ");
    for (int x = 0; x < 6; x++)
    {
        if(x < PIN_ANALOG_CNT)
            Serial.print(analogRead(x));
        else
            Serial.print("?");
        Serial.print(" ");
    }
    Serial.println("");
}

void sendStatus()
{
    Serial.print("S ");
    Serial.print(eeprom.data.programa);
    Serial.print(" ");
    Serial.print(lastError);
    Serial.print(" ");
    Serial.print(drive.motorEsq->read());
    Serial.print(" ");
    Serial.print(drive.motorDir->read());
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
    char pos;
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

        if (tok = STRTOK(command, " "))						// primeiro token eh o comando/acao
        {
            if(strcmp(tok, CMD_WRITE) == 0)					// atribui um valor a uma variavel
            {
                if (tok = STRTOK(NULL, " ="))				// segundo token eh o nome da variavel
                {
                    char dest[10];
                    strcpy(dest,tok);

                    if (tok = STRTOK(NULL, " "))			// terceiro token eh o valor a ser atribuido
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
                            eeprom.data.programa = valor;
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
                            if (tok = STRTOK(NULL, " "))	// I
                                eeprom.data.pid.Ki = atoi(tok);
                            if (tok = STRTOK(NULL, " "))	// D
                                eeprom.data.pid.Kd = atoi(tok);
                        }
                        else if(strcmp(dest, VAR_FREIO) == 0)
                            eeprom.data.handBrake = valor;
                    }
                }
            }
            else if(strcmp(tok, CMD_READ) == 0)	// le uma variavel
            {
                tok = STRTOK(NULL, " ");
                if (tok)			// second token is the VAR to be read
                {
                    if(strcmp(tok, VAR_RODA_ESQ) == 0)
                    {
                        Serial.print("L ");
                        Serial.println(drive.motorEsq->read());
                    }
                    else if(strcmp(tok, VAR_ZERO_ESQ) == 0)
                    {
                        Serial.print("LC ");
                        Serial.println(eeprom.data.centroMotorEsq);
                    }
                    else if(strcmp(tok, VAR_RODA_DIR) == 0)
                    {
                        Serial.print("R ");
                        Serial.println(drive.motorDir->read());
                    }
                    else if(strcmp(tok, VAR_ZERO_DIR) == 0)
                    {
                        Serial.print("RC ");
                        Serial.println(eeprom.data.centroMotorDir);
                    }
                    else if(strcmp(tok, VAR_PROGRAMA) == 0)
                    {
                        Serial.print("P ");
                        Serial.println(eeprom.data.programa);
                    }
                    else if(strcmp(tok, VAR_T_POL) == 0)
                    {
                        Serial.print("DI ");
                        Serial.println(eeprom.data.mvDelay.inch);
                    }
                    else if(strcmp(tok, VAR_T_90) == 0)
                    {
                        Serial.print("DR ");
                        Serial.println(eeprom.data.mvDelay.right);
                    }
                    else if(strcmp(tok, VAR_T_RF) == 0)
                    {
                        Serial.print("DRF ");
                        Serial.println(eeprom.data.RF_delay_reads);
                    }
                    else if(strcmp(tok, VAR_SERVO_X) == 0)
                    {
                        Serial.print("SX ");
                        Serial.println(pan.read());
                    }
                    else if(strcmp(tok, VAR_SERVO_Y) == 0)
                    {
                        Serial.print("SY ");
                        Serial.println(tilt.read());
                    }
                    else if(strcmp(tok, VAR_SERVO_Z) == 0)
                    {
                        Serial.print("SZ ");
                        Serial.println(roll.read());
                    }
                    else if(strcmp(tok, VAR_FREIO) == 0)
                    {
                        Serial.print("HB ");
                        Serial.println((int)eeprom.data.handBrake);
                    }
                    else if(strcmp(tok, VAR_AS) == 0)
                        displayAnalogSensors();
                    else if(strcmp(tok, VAR_PID) == 0)
                    {
                        Serial.print("PID ");
                        Serial.print(eeprom.data.pid.Kp);
                        Serial.print(" ");
                        Serial.print(eeprom.data.pid.Ki);
                        Serial.print(" ");
                        Serial.println(eeprom.data.pid.Kd);
                    }
                }
            }
            else if(strcmp(tok, CMD_SAVE) == 0)	// save stuff to eeprom
                eeprom.save();
            else if(strcmp(tok, CMD_LOAD) == 0)	// discard changes and reload from eeprom
            {
                eeprom.load();
                drive.stop();
            }
            else if(strcmp(tok, CMD_DEFAULT) == 0)
            {
                eeprom.loadDefault();
                drive.stop();
            }
            else if(strcmp(tok, CMD_LF_CAL) == 0)	// re-calibrate line following IR sensors
                lineFollower.autoCalibrate();
            else if(strcmp(tok, CMD_MV_INCH) == 0)
                drive.inch();
            else if(strcmp(tok, CMD_MV_STOP) == 0)
                drive.stop();
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
                tok = STRTOK(NULL, " ");
                if (tok)			// segundo token eh o percentual de potencia p/ eixo X
                {
                    int x = atoi(tok);
                    tok = STRTOK(NULL, " ");
                    if (tok)		// terceiro token eh o percentual de potencia p/ eixo Y
                    {
                        int y = atoi(tok);

                        if( x || y )
                            drive.vectorial(x, y);
                        else
                            drive.stop();
                    }
                }
            }
            else if(strcmp(tok, CMD_TURN_LEFT) == 0)
                drive.turnLeft();
            else if(strcmp(tok, CMD_TURN_RIGHT) == 0)
                drive.turnRight();
            else if(strcmp(tok, CMD_STATUS) == 0)
                sendStatus();
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

    // set all free pins as INPUT with PULL-UP to save power
    int unused[] = PIN_UNUSED_ARRAY;
    for(int p=0; p < PIN_UNUSED_CNT; p++)
    {
        pinMode(unused[p], INPUT);
        digitalWrite(unused[p], HIGH);
    }

#ifndef WHEEL_DC
    motorEsq.init(PIN_LEFTWHEEL, eeprom.data.centroMotorEsq);
    motorDir.init(PIN_RIGHTWHEEL, eeprom.data.centroMotorDir, true);
#else
    motorEsq.init(PIN_LEFTWHEEL_PWM, PIN_LEFTWHEEL, eeprom.data.centroMotorEsq);
    motorDir.init(PIN_RIGHTWHEEL_PWM ,PIN_RIGHTWHEEL, eeprom.data.centroMotorDir);
#endif
    // bellow is needed for polimorphism
    drive.motorEsq = &motorEsq;
    drive.motorDir = &motorDir;

#ifdef PIN_SERVO_PAN
    pan.attach(PIN_SERVO_PAN);
    pan.write(90);
#endif

#ifdef PIN_SERVO_TILT
    tilt.attach(PIN_SERVO_TILT);
    tilt.write(90);
#endif

#ifdef PIN_SERVO_ROLL
    roll.attach(PIN_SERVO_ROLL);
    roll.write(90);
#endif

    if (eeprom.data.programa == PRG_LINEFOLLOWER)
        lineFollower.autoCalibrate();

    pinMode(13, OUTPUT);    // LED onboard
    digitalWrite(13, LOW);

#ifdef PIN_LASER
    pinMode(PIN_LASER, OUTPUT);
    digitalWrite(PIN_LASER, LOW);
#endif

#ifdef WIICHUCK_POWER
    nunchuck_setpowerpins();
#endif

#ifdef WIICHUCK
    nunchuck_init();
#endif
}

// ******************************************************************************
//		MAIN LOOP
// ******************************************************************************

void loop()
{
    server.loop();

    static long nextSendStatus = 0;
    if(millis() > nextSendStatus && eeprom.data.programa != PRG_SCOPE)
    {
        nextSendStatus += 10000;
        //displayAnalogSensors();
        sendStatus();
    }

    int dorme_ms = 10;
    switch(eeprom.data.programa)
    {
    case PRG_SHOW_SENSORS:
        displayAnalogSensors();
        sendStatus();
        dorme_ms = 100;
        //#ifdef WIICHUCK
        //nunchuck_print_data();
        //#endif

    case PRG_RC:
        drive.refresh();
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

    case PRG_SENTRY:
    {
        if(rangeFinder.sentry())
        {
            digitalWrite(13, HIGH);
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
            digitalWrite(13, LOW);
            //delay(1000);
        }
    }
    break;

    #ifdef WIICHUCK
    case PRG_WIICHUCK:
        if(nunchuck_get_data() == 0)
            break;

        //nunchuck_print_data();

        if(nunchuck_zbutton())
        {
            //drive.vectorial(map(nunchuck_accelx(),200,700,-100,100),
            //                map(nunchuck_accely(),200,700,-100,100));
            int x = nunchuck_joyx() - eeprom.data.joyCenter.x;
            int y = nunchuck_joyy() - eeprom.data.joyCenter.y;

            // TODO: mapear 0-100

            drive.vectorial(x, y);
        }
        else
        {
            drive.stop();

            #ifdef PIN_SERVO_PAN
            if(nunchuck_joyx()<100)
            {
                int angle = pan.read() + 1;
                if(nunchuck_joyx() == 0)
                    angle += 4;
                if(angle > 170)
                    angle = 170;
                pan.write(angle);
            }
            else if(nunchuck_joyx()>150)
            {
                int angle = pan.read() - 1;
                if(nunchuck_joyx() == 255)
                    angle -= 4;
                if(angle < 10)
                    angle = 10;
                pan.write(angle);
            }
            #endif

            #ifdef PIN_SERVO_TILT
            if(nunchuck_joyy()>150)
            {
                int angle = tilt.read() + 1;
                if(nunchuck_joyy() == 255)
                    angle += 4;
                if(angle > 170)
                    angle = 170;
                tilt.write(angle);
            }
            else if(nunchuck_joyy()<100)
            {
                int angle = tilt.read() - 1;
                if(nunchuck_joyy() == 0)
                    angle -= 4;
                if(angle < 10)
                    angle = 10;
                tilt.write(angle);
            }
            #endif
        }

        if(nunchuck_cbutton())
            digitalWrite(PIN_LASER,HIGH);
        else
            digitalWrite(PIN_LASER,LOW);

        delay(10);
    break;
    #endif

    case PRG_SCOPE:
		Serial.write((analogRead(0) >> 2) & 0xFF);
        dorme_ms = eeprom.data.RF_delay_reads;
    break;

    case PRG_KNOB:
        pan.write(map(analogRead(0), 0, 1023, 5, 174));
        dorme_ms = 50;
    break;

    case PRG_TEST:
    {
        short sonar=analogRead(PIN_SONAR);
        if(sonar < 16)
        {
            drive.left(100);
            delay(100);
        }
        else if(sonar < 20)
            drive.leftSmooth(100);
        else
            drive.forward(100);
    }
    break;

    default:
        if( lastError != ERR_INVALID_PRG )
        {
            lastError = ERR_INVALID_PRG;
            Serial.println("ERR_INVALID_PRG");
        }
    break;
    }
    delay(dorme_ms);
}
