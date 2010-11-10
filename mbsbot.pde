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
    if(x < 0)			// turn left
    {
        if(y == 0)		// hard turn left
            left(-x);
        else if(y > 0)	// smooth left-forward
            leftSmooth(-x);
        else if(y < 0)	// smooth left-backward
            leftSmooth(x);
    }
    else if(x > 0)		// turn right
    {
        if(y == 0)		// hard turn right
            right(x);
        else if(y > 0)	// smooth right-forward
            rightSmooth(x);
        else if(y < 0)	// smooth right-backward
            rightSmooth(-x);
    }
    else if(y != 0)		// y > 0 = forward / y < 0 = backward
        forward(y);
    else
        stop();
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

// Auto Calibration parameters
#define CAL_READS_NUM 5
#define CAL_READS_INTERVAL 200

void LineFollower::autoCalibrate()
{
    unsigned short sensorTemp[NUM_IR_TRACK];

    unsigned short sensorTrack[NUM_IR_TRACK];
    unsigned short sensorTrackMax[NUM_IR_TRACK];
    unsigned short sensorTrackMin[NUM_IR_TRACK];

    unsigned short sensorOut[NUM_IR_TRACK];
    unsigned short sensorOutMax[NUM_IR_TRACK];
    unsigned short sensorOutMin[NUM_IR_TRACK];

    char linha[32]; // string temp to use with sprintf

    Serial.println("Calibrating TRACK");

    // read all sensors at TRACK position several times

    memset(sensorTrack, 0, sizeof(sensorTrack));
    memset(sensorTrackMax, 0, sizeof(sensorTrackMax));
    memset(sensorTrackMin, 1023, sizeof(sensorTrackMin));

    for(int x=0; x < CAL_READS_NUM; x++)
    {
        for(int y=0; y < NUM_IR_TRACK; y++)
        {
            sensorTrack[y] += sensorTemp[y] = analogRead(PIN_FIRST_IR_SENSOR + y);
            sensorTrackMax[y] = max(sensorTrack[y],sensorTrackMax[y]);
            sensorTrackMin[y] = min(sensorTrack[y],sensorTrackMin[y]);
        }

        sprintf(linha," %04d %04d %04d", sensorTemp[0], sensorTemp[1], sensorTemp[2]);
        Serial.println(linha);

        delay(CAL_READS_INTERVAL);
    }

    Serial.println("Calibrate OUTSIDE");

    drive.inch();

    // read all sensors at OUTSIDE position several times

    memset(sensorOut, 0, sizeof(sensorOut));
    memset(sensorOutMax, 0, sizeof(sensorOut));
    memset(sensorOutMin, 1023, sizeof(sensorOut));
    for(int x=0; x < CAL_READS_NUM; x++)
    {
        for(int y=0; y < NUM_IR_TRACK; y++)
        {
            sensorOut[y] += sensorTemp[y] = analogRead(PIN_FIRST_IR_SENSOR + y);
            sensorOutMax[y] = max(sensorOut[y], sensorOutMax[y]);
            sensorOutMin[y] = min(sensorOut[y], sensorOutMin[y]);
        }

        sprintf(linha," %04d %04d %04d", sensorTemp[0], sensorTemp[1], sensorTemp[2]);
        Serial.println(linha);

        delay(CAL_READS_INTERVAL);
    }

    bool reverseSensor[NUM_IR_TRACK];

    for(int y=0; y < NUM_IR_TRACK; y++)
    {
        unsigned short mediumTrack = (sensorTrack[y] / CAL_READS_NUM);
        unsigned short mediumOut = (sensorOut[y] / CAL_READS_NUM);

        reverseSensor[y] = ( mediumOut > mediumTrack );

        eeprom.data.LF_threshold[y] = ( mediumTrack + mediumOut ) / 2;
    }

    // if one sensor is reversed then all others must also be!
    eeprom.data.LF_reverseColor = reverseSensor[0];

    if(eeprom.data.LF_reverseColor)
        Serial.println("Reversed Track Color");
    else
        Serial.println("Normal Track Color");

    for(int x=1; x < NUM_IR_TRACK; x++)
        if (reverseSensor[x] ^ eeprom.data.LF_reverseColor)
        {
            sprintf(linha, "BAD Reverse %d", x);
            Serial.println(linha);
            return;
        }

    for(int x=0; x < NUM_IR_TRACK; x++)
    {
        if(eeprom.data.LF_reverseColor)
        {
            if(sensorOutMin[x] <= sensorTrackMax[x])
            {
                sprintf(linha, "BAD limits %d", x);
                Serial.println(linha);
                return;
            }
        }
        else
        {
            if(sensorOutMax[x] >= sensorTrackMin[x])
            {
                sprintf(linha, "BAD limits %d", x);
                Serial.println(linha);
                return;
            }
        }
    }

    Serial.println("Thresholds:");
    sprintf(linha," %04d %04d %04d", eeprom.data.LF_threshold[0], eeprom.data.LF_threshold[1], eeprom.data.LF_threshold[2]);
    Serial.println(linha);
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
    Serial.print("AS ");
    for (int x = 0; x < 6; x++)
    {
        Serial.print(analogRead(x));
        Serial.print(" ");
    }
    Serial.println("");
}

void sendStatus()
{
    Serial.print("S ");
    Serial.print(eeprom.data.selectedProgram);
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
    char * receive();
    void loop();
    char *getCommand()
    {
        return command;
    }
private:
    char command[MAX_COMMAND_SIZE];
    char pos;
} server;

char * Server::receive()
{
    while(Serial.available() > 0)
    {
        char c = Serial.read();

        if (pos == MAX_COMMAND_SIZE)
        {
            pos=0;
            Serial.println("ERR: MAX_CMD_SIZ");
        }
        else if(c == COMMAND_END)
        {
            command[pos]=0;
            pos=0;
            return command;
        }
        else
        {
            command[pos]=c;
            pos++;
        }
    }
    return NULL;
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
long lastSendStatus = 0;

void loop()
{
    server.loop();

    if(millis() > lastSendStatus + 10000)
    {
        lastSendStatus += 10000;
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
    }
    delay(15);
}
