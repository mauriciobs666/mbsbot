/**	Copyright (C) 2010 - Mauricio Bieze Stefani
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

// Arduino /libraries
#include <Servo.h>

// I2C and wiichuck
#include <Wire.h>
#include "nunchuck_funcs.h"

// ******************************************************************************
//		DEFINES AND SETUP
// ******************************************************************************

#include "Protocol.h"
#include "Board.h"

#ifdef PIN_BEEP
#define BEEP(freq, dur) tone(PIN_BEEP,freq,dur)
#else
#define BEEP(freq, dur)
#endif

// RANGE FINDER
#define RF_NUMBER_STEPS 30
#define SHARP_TRESHOLD 300

Servo pan;
Servo tilt;
Servo roll;

enum Errors lastError = SUCCESS;

// ******************************************************************************
//		EEPROM - PERSISTENT CONFIGURATION
// ******************************************************************************
#include <EEPROM.h>
class Eeprom
{
public:
    struct sConfigurationData
    {
        short selectedProgram;

        short leftWheelCenter;
        short rightWheelCenter;

        struct sMoveDelays // delay in ms used to make it move (+/-)
        {
            short inch;		//an inch
            short right;	//right angle turn (90 degrees)
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
    } data;
    void load()
    {
        char * dest = (char*) &data;
        for(int addr = 0; addr < sizeof(data); addr++, dest++ )
            *dest = EEPROM.read(addr);
    }
    void save()
    {
        char * dest = (char*) &data;
        for(int addr = 0; addr < sizeof(data); addr++, dest++ )
            EEPROM.write(addr, *dest);
    }
    void loadDefault()
    {
        data.selectedProgram = PRG_RC;
        data.leftWheelCenter = 1410;
        data.rightWheelCenter = 1384;
        data.mvDelay.inch = 200;
        data.mvDelay.right = 400;
        for(int x = 0; x < NUM_IR_TRACK; x++)
        {
            data.LF_threshold[x] = 512;
            data.LF_reverseColor = false;
        }
        data.pid.Kp = 100;
        data.pid.Ki = 0;
        data.pid.Kd = 0;
        data.RF_delay_reads = 100;
    }
} eeprom;

// ******************************************************************************
//		WHEELS CONTROLER
// ******************************************************************************
class Wheel
{
public:
    void stop()
    {
        current = center;
        refresh();
    }
    virtual void move(char percent);
    virtual void refresh();
    void write(int value)
    {
        current = value;
        refresh();
    }
    short read()
    {
        return current;
    }
    void setCenter(short value)
    {
        center = value;
    }
    void setReverse(bool rev)
    {
        reverse = rev;
    }
protected:
    bool reverse;
    short current;
    short center;
};

#ifndef WHEEL_DC

class WheelServo : public Wheel
{
public:
    void init(int pin, short centerAng=1500, bool reverseDirection=false)
    {
        servo.attach(pin);
        setCenter(centerAng);
        setReverse(reverseDirection);
        stop();
    }
    virtual void move(char percent)
    {
        // typical servos use from 1000-2000us
        current = reverse ? (center - percent*5) : (center + percent*5);
        refresh();
    }
    virtual void refresh()
    {
        // protect against out of range values
        if (current < 1000)
            current = 1000;
        else if (current > 2000)
            current = 2000;

        servo.writeMicroseconds(current);
    }
private:
    Servo servo;
}
leftWheel, rightWheel;

#else

class WheelDC : public Wheel
{
public:
    void init(int PWMpin, int DIRpin, bool reverseDirection=false)
    {
        pwm = PWMpin;
        pinMode(pwm, OUTPUT);
        analogWrite(pwm, 0);

        dir = DIRpin;
        pinMode(dir, OUTPUT);
        digitalWrite(dir, 0);

        setCenter(0);
        setReverse(reverseDirection);
        stop();
    }
    virtual void move(char percent)
    {
        current = reverse ? ( center - (percent*5)/2 ) : ( center + (percent*5)/2 );
        refresh();
    }
    virtual void refresh()
    {
        // protect against out of range values
        if(current > 255)
            current = 255;
        else if (current < -255)
            current = -255;

        if(current < center)
            digitalWrite(dir, HIGH);
        else
            digitalWrite(dir, LOW);

        analogWrite(pwm, abs(current));
    }
private:
    int pwm;
    int dir;
}
leftWheel, rightWheel;

#endif

class Drive
{
public:
    Wheel *leftWheel;
    Wheel *rightWheel;
    void forward(char percent=100, int period=0)
    {
        leftWheel->move(percent);
        rightWheel->move(percent);
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
        leftWheel->move(-percent);
        rightWheel->move(percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void leftSmooth(char percent=100, int period=0)
    {
        leftWheel->stop();
        rightWheel->move(percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void right(char percent=100, int period=0)
    {
        leftWheel->move(percent);
        rightWheel->move(-percent);
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void rightSmooth(char percent=100, int period=0)
    {
        leftWheel->move(percent);
        rightWheel->stop();
        if(period > 0)
        {
            delay(period);
            stop();
        }
    }
    void stop()
    {
        leftWheel->stop();
        rightWheel->stop();
    }
    void refresh()
    {
        leftWheel->refresh();
        rightWheel->refresh();
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
    if(y > 0)
    {
        if(x > 0)   // Q1
        {
            leftWheel->move( max(x,y) );
            if(y > x)
                rightWheel->move( y-x );
            else
                rightWheel->move( x-y );
        }
        else        // Q2
        {
            rightWheel->move( max(-x,y) );
            if(y > -x)
                leftWheel->move( x+y );
            else
                leftWheel->move( x-y );
        }
    }
    else // (y <= 0)
    {
        if(x < 0)   // Q3
        {
            leftWheel->move( min(x,y) );
            if(x < y)
                rightWheel->move( -x-y );
            else
                rightWheel->move( y-x );
        }
        else        // Q4
        {
            rightWheel->move( min(-x,y) );
            leftWheel->move( x+y );
        }
    }
}

// ******************************************************************************
//		PHOTOVORE
// ******************************************************************************
void photovore()
{
    const int threshold = 25;

    int left = analogRead(0);
    int right = analogRead(1);

    // smaller numbers mean more light

    if ( (right - left) > threshold )		// turn left
        drive.left();
    else if ( (left - right) > threshold )	// turn right
        drive.right();
    else 									// go ahead
        drive.forward();
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

        // TODO (mbs#1#): limit accumulation
        /* TODO: limit accumulation
        if (accErr > MAX_ERROR)
        	accErr = MAX_ERROR;
        else if (accErr < MIN_ERROR)
        	accErr = MIN_ERROR;
        */

        int Iterm = eeprom.data.pid.Ki * accError;

        // Deritavive
        int Dterm = eeprom.data.pid.Kd * ( error - lastError );

        int MV = Pterm + Iterm + Dterm;

        drive.leftWheel->move ( (MV < 0) ? (100 + MV) : 100 );
        drive.rightWheel->move( (MV > 0) ? (100 - MV) : 100 );

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

    /* TODO:
    if scanner is pointing far left
    	robot turns left
    else if scanner is pointing far right
    	robot turns right
    else //scanner pointing forward
    	robot drives straight
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
    #ifdef WIICHUCK
        #define MAX_ANALOG 2
    #else
        #define MAX_ANALOG 6
    #endif

    Serial.print("AS ");
    for (int x = 0; x < 6; x++)
    {
        if(x < MAX_ANALOG)
            Serial.print(analogRead(x));
        else
            Serial.print("I2C");
        Serial.print(" ");
    }
    Serial.println("");
}

void sendStatus()
{
    Serial.print("S ");
    Serial.print(eeprom.data.selectedProgram);
    Serial.print(" ");
    Serial.print(lastError);
    Serial.print(" ");
    Serial.print(drive.leftWheel->read());
    Serial.print(" ");
    Serial.print(drive.rightWheel->read());
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
    char command[MAX_COMMAND_SIZE];
    char pos;
} server;

bool Server::receive()
{
    while(Serial.available() > 0)
    {
        char c = Serial.read();

        if (pos == MAX_COMMAND_SIZE)
        {
            pos=0;
            lastError = ERR_MAX_CMD_SIZ;
            Serial.println("ERR_MAX_CMD_SIZ");
        }
        else if(c == COMMAND_END)
        {
            command[pos]=0;
            pos=0;
            return true;
        }
        else
        {
            command[pos]=c;
            pos++;
        }
    }
    return false;
}

void Server::loop()
{
    if( server.receive() )
    {
        char *tok;	// temp

        char *pqp;	// not all avr-libc versions contain strtok() so we need this crap bellow:
        #define STRTOK(a, b) strtok_r(a, b, &pqp)

        if (tok = STRTOK(command, " "))						// first token is the ACTION
        {
            if(strcmp(tok, CMD_WRITE) == 0)					// assign a value to some variable
            {
                if (tok = STRTOK(NULL, " ="))				// second token is the DESTINATION VAR
                {
                    char dest[10];
                    strcpy(dest,tok);

                    if (tok = STRTOK(NULL, " "))			// third token is the actual value
                    {
                        int value = atoi(tok);

                        if(isdigit(dest[0]))                // digital pin number
                            digitalWrite(atoi(dest), value ? HIGH : LOW);
                        else if(strcmp(dest, "l") == 0)		// left motor
                            drive.leftWheel->write(value);
                        else if(strcmp(dest, "lc") == 0)	// left motor center
                        {
                            drive.leftWheel->setCenter(value);
                            eeprom.data.leftWheelCenter = value;
                        }
                        else if(strcmp(dest, "r") == 0)		// right motor
                            drive.rightWheel->write(value);
                        else if(strcmp(dest, "rc") == 0)	// right motor center
                        {
                            drive.rightWheel->setCenter(value);
                            eeprom.data.rightWheelCenter = value;
                        }
                        else if(strcmp(dest,"p") == 0)		// program
                            eeprom.data.selectedProgram = value;
                        else if(strcmp(dest,"di") == 0)		// inch delay
                            eeprom.data.mvDelay.inch = value;
                        else if(strcmp(dest,"dr") == 0)		// right-angle turn delay
                            eeprom.data.mvDelay.right = value;
                        else if(strcmp(dest,"drf") == 0)	// range finder delay
                            eeprom.data.RF_delay_reads = value;
                        else if(strcmp(dest,"sx") == 0)		// servo x position
                            pan.write(value);
                        else if(strcmp(dest,"sy") == 0)		// servo x position
                            tilt.write(value);
                        else if(strcmp(dest,"sz") == 0)		// servo x position
                            roll.write(value);
                        else if(strcmp(dest,"pid") == 0)	// PID parameters
                        {
                            eeprom.data.pid.Kp = value;		// P
                            if (tok = STRTOK(NULL, " "))	// I
                                eeprom.data.pid.Ki = atoi(tok);
                            if (tok = STRTOK(NULL, " "))	// D
                                eeprom.data.pid.Kd = atoi(tok);
                        }
                    }
                }
            }
            else if(strcmp(tok, CMD_READ) == 0)	// action GET reads a variable
            {
                tok = STRTOK(NULL, " ");
                if (tok)			// second token is the VAR to be read
                {
                    if(strcmp(tok, "l") == 0)				// left motor
                    {
                        Serial.print("L ");
                        Serial.println(drive.leftWheel->read());
                    }
                    else if(strcmp(tok, "lc") == 0)			// left motor center
                    {
                        Serial.print("LC ");
                        Serial.println(eeprom.data.leftWheelCenter);
                    }
                    else if(strcmp(tok, "r") == 0)			// right motor
                    {
                        Serial.print("R ");
                        Serial.println(drive.rightWheel->read());
                    }
                    else if(strcmp(tok, "rc") == 0)			// right motor center
                    {
                        Serial.print("RC ");
                        Serial.println(eeprom.data.rightWheelCenter);
                    }
                    else if(strcmp(tok,"p") == 0)			// program
                    {
                        Serial.print("P ");
                        Serial.println(eeprom.data.selectedProgram);
                    }
                    else if(strcmp(tok,"di") == 0)			// inch delay
                    {
                        Serial.print("DI ");
                        Serial.println(eeprom.data.mvDelay.inch);
                    }
                    else if(strcmp(tok,"dr") == 0)			// right-angle turn delay
                    {
                        Serial.print("DR ");
                        Serial.println(eeprom.data.mvDelay.right);
                    }
                    else if(strcmp(tok,"drf") == 0)			// range finder delay
                    {
                        Serial.print("DRF ");
                        Serial.println(eeprom.data.RF_delay_reads);
                    }
                    else if(strcmp(tok,"sx") == 0)			// servo x
                    {
                        Serial.print("SX ");
                        Serial.println(pan.read());
                    }
                    else if(strcmp(tok,"sy") == 0)			// servo y
                    {
                        Serial.print("SY ");
                        Serial.println(tilt.read());
                    }
                    else if(strcmp(tok,"sz") == 0)			// servo z
                    {
                        Serial.print("SZ ");
                        Serial.println(roll.read());
                    }
                    else if(strcmp(tok,"as") == 0)			// all analog sensors
                        displayAnalogSensors();
                    else if(strcmp(tok,"pid") == 0)			// PID
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
                eeprom.load();
            else if(strcmp(tok, CMD_DEFAULT) == 0)
                eeprom.loadDefault();
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
                        drive.leftWheel->move(lw);
                        drive.rightWheel->move(atoi(tok));
                    }
                }
            }
            else if(strcmp(tok, CMD_MV_VECT) == 0)
            {
                tok = STRTOK(NULL, " ");
                if (tok)			// second token is the X-axis power percent
                {
                    int x = atoi(tok);
                    tok = STRTOK(NULL, " ");
                    if (tok)		// third token is the Y-axis power percent
                        drive.vectorial(x, atoi(tok));
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
            else if(strcmp(tok, CMD_BEEP) == 0)
            {
                tok = STRTOK(NULL, " ");
                if (tok)			        // frequency
                    BEEP(atoi(tok), 200);   // duration = 200ms
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
    leftWheel.init(PIN_LEFTWHEEL, eeprom.data.leftWheelCenter);
    rightWheel.init(PIN_RIGHTWHEEL, eeprom.data.rightWheelCenter, true);
#else
    leftWheel.init(PIN_LEFTWHEEL_PWM, PIN_LEFTWHEEL);
    rightWheel.init(PIN_RIGHTWHEEL_PWM ,PIN_RIGHTWHEEL);
#endif
    // bellow is needed for polimorphism
    drive.leftWheel = &leftWheel;
    drive.rightWheel = &rightWheel;

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

    if (eeprom.data.selectedProgram == PRG_LINEFOLLOWER)
        lineFollower.autoCalibrate();

    pinMode(13, OUTPUT);    // Arduino onboard LED
    digitalWrite(13, LOW);

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
    if(millis() > nextSendStatus)
    {
        nextSendStatus += 10000;
        displayAnalogSensors();
        sendStatus();
    }

    switch(eeprom.data.selectedProgram)
    {
    case PRG_SHOW_SENSORS:
        delay(100);
        displayAnalogSensors();
        sendStatus();

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
            delay(1000);
            digitalWrite(13, LOW);
            delay(1000);
        }
    }
    break;

    #ifdef WIICHUCK
    case PRG_WIICHUCK:
        nunchuck_get_data();

        nunchuck_print_data();

        #ifdef PIN_SERVO_PAN
            pan.write(map(nunchuck_joyx(),0,255,5,175));
        #endif
        #ifdef PIN_SERVO_TILT
            tilt.write(map(nunchuck_joyy(),0,255,5,175));
        #endif

        if(nunchuck_zbutton())
        {
            drive.vectorial(map(nunchuck_accelx(),200,700,-100,100),
                            map(nunchuck_accely(),200,700,-100,100));
        }
        else
            drive.stop();

        delay(10);
    break;
    #endif

    case PRG_PROCESSING:
        Serial.print((analogRead(0) / 4), BYTE);
        //delay(10);
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
    delay(10);
}
