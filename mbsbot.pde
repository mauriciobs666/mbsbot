/*	Copyright (C) 2009-2010 - Mauricio Bieze Stefani
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

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/******************************************************************************
 *	DEFINES AND SETUP
 ******************************************************************************/

// eeprom.data.selectedProgram

#define PRG_RC				0x00
#define PRG_LINEFOLLOWER	0x01
#define PRG_PHOTOVORE 		0x02
#define PRG_SHOW_SENSORS	0x03

// LINE FOLLOWER SETUP

#define NUM_IR_TRACK 3
#define FIRST_IR_SENSOR_INDEX 0
// where NUM_IR_TRACK=3 and FIRST_IR_SENSOR_INDEX=2 means pins A2, A3 and A4 are connected

/******************************************************************************
 *	EEPROM - PERSISTENT CONFIGURATION
 ******************************************************************************/

#include <EEPROM.h>
class Eeprom
{
public:
	struct sConfigurationData
	{
		short selectedProgram;

		short leftWheelCenter;
		short rightWheelCenter;

		short inch;

		unsigned short LF_threshold[NUM_IR_TRACK];
		bool LF_reverseColor;
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
} eeprom;

/******************************************************************************
 *	WHEELS CONTROLER
 ******************************************************************************/

#include <Servo.h>
class Wheel
{
public:
	void init(int pin, short centerAng=1500, bool reverseDirection=false)
	{
		servo.attach(pin);
		setCenter(centerAng);
		setReverse(reverseDirection);
		stop();
	}
	void stop()
	{
		current = centerAngle;
		refresh();
	}
	void refresh()
	{
		servo.writeMicroseconds(current);
	}
	void move(char percent)
	{
		// typical servos use from 1000-2000us
		current = reverse ? (centerAngle - percent*5) : (centerAngle + percent*5);
		refresh();
	}
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
		centerAngle = value;
	}
	void setReverse(bool rev)
	{
		reverse = rev;
	}
private:
	Servo servo;
	bool reverse;
	short centerAngle;
	short current;
};

class Drive
{
public:
	Wheel leftWheel;
	Wheel rightWheel;
	void forward(char percent=100)
	{
		leftWheel.move(percent);
		rightWheel.move(percent);
	}
	void backward(char percent=100)
	{
		leftWheel.move(-percent);
		rightWheel.move(-percent);
	}
	void left(char percent=100)
	{
		leftWheel.move(-percent);
		rightWheel.move(percent);
	}
	void leftSmooth(char percent=100)
	{
		leftWheel.stop();
		rightWheel.move(percent);
	}
	void right(char percent=100)
	{
		leftWheel.move(percent);
		rightWheel.move(-percent);
	}
	void rightSmooth(char percent=100)
	{
		leftWheel.move(percent);
		rightWheel.stop();
	}
	void stop()
	{
		leftWheel.stop();
		rightWheel.stop();
	}
	void refresh()
	{
		leftWheel.refresh();
		rightWheel.refresh();
	}
	void pulse(int period, char percent=100)
	{
		forward(percent);
		delay(period);
		stop();
	}
	void inch(bool forward=true)
	{
		pulse(eeprom.data.inch, forward ? 100 : -100);
	}
} drive;

/******************************************************************************
 *	PHOTOVORE
 ******************************************************************************/

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

/******************************************************************************
 *	DEBUG SENSORS
 ******************************************************************************/

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

/******************************************************************************
 *	LINE FOLLOWER
 ******************************************************************************/

class LineFollower
{
public:
	void init();
	void autoCalibrate();
	void loop();
	void readSensors(bool * isIRSensorOverLine);
private:
	char lastCorrection;
} lineFollower;

void LineFollower::loop()
{
	bool IRSensorOverLine[NUM_IR_TRACK];		// true if sensor is over the line

	readSensors(IRSensorOverLine);

	if (IRSensorOverLine[0] && IRSensorOverLine[1] && IRSensorOverLine[2])	// end of line, stop
		drive.stop();
	else if( IRSensorOverLine[0] )	// turn right
	{
		lastCorrection=1;
		drive.right();
	}
	else if( IRSensorOverLine[2] )	// turn left
	{
		lastCorrection=-1;
		drive.left();
	}
	else if( IRSensorOverLine[1] )	// go ahead
		drive.forward();
	else 							// lost track, redo last correction in the hope the track is just a bit ahead
	{
		if(lastCorrection == 1)
			drive.right();
		else if (lastCorrection == -1)
			drive.left();
	}
}

void LineFollower::readSensors(bool * isIRSensorOverLine)
{
	unsigned short IRSensor[NUM_IR_TRACK];

	// read IR sensor data and map into IRSensor[]
	for(int x = 0; x < NUM_IR_TRACK; x++)
	{
		IRSensor[x] = analogRead(FIRST_IR_SENSOR_INDEX + x);
		isIRSensorOverLine[x] = (IRSensor[x] > eeprom.data.LF_threshold[x]) ^ eeprom.data.LF_reverseColor;
	}
}

void LineFollower::autoCalibrate()
{
	// configure number of redundant reads during autocalibration
	#define CAL_READS_NUM 5
	#define CAL_READS_INTERVAL 200

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
			sensorTrack[y] += sensorTemp[y] = analogRead(FIRST_IR_SENSOR_INDEX + y);
			sensorTrackMax[y] = max(sensorTrack[y],sensorTrackMax[y]);
			sensorTrackMin[y] = min(sensorTrack[y],sensorTrackMin[y]);
		}

		sprintf(linha," %04d %04d %04d", sensorTemp[0], sensorTemp[1], sensorTemp[2]);
		Serial.println(linha);

		delay(CAL_READS_INTERVAL);
	}

	// pause so the user can move the sensors outside the track

	Serial.println("Calibrate OUTSIDE");
/*
	for(int x=10; x>0; x--)
	{
		sprintf(linha, "Start in %ds", x);
		Serial.println(linha);

		delay(1000);
	}
*/
	// no no don't pause... move!
	drive.inch();

	// read all sensors at OUTSIDE position several times

	memset(sensorOut, 0, sizeof(sensorOut));
	memset(sensorOutMax, 0, sizeof(sensorOut));
	memset(sensorOutMin, 1023, sizeof(sensorOut));
	for(int x=0; x < CAL_READS_NUM; x++)
	{
		for(int y=0; y < NUM_IR_TRACK; y++)
		{
			sensorOut[y] += sensorTemp[y] = analogRead(FIRST_IR_SENSOR_INDEX + y);
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

/******************************************************************************
 *	TELNET SERVER
 ******************************************************************************/

#define MAX_COMMAND_SIZE 20
#define COMMAND_END '\n'

class Server
{
public:
	Server() : pos(0) {}
	void send(char *data)
		{ Serial.print(data); }
	char * receive();
	void loop();
	char *getCommand()
		{ return command; }
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
		char *pqp; // not all avr-libc versions contain strtok() ...
		char * tok = strtok_r(command, " ", &pqp);

		if (tok)	// first token is the ACTION
		{
			if(strcmp(tok, "set") == 0)	// action SET is meant to assign a value to some variable
			{
				tok = strtok_r(NULL, " =", &pqp);
				if (tok)			// second token is the DESTINATION VAR
				{
					char dest[10];
					strcpy(dest,tok);

					tok = strtok_r(NULL, " ", &pqp);
					if (tok)		// third token is the actual value
					{
						int value = atoi(tok);

						if(strcmp(dest, "l") == 0)			// left motor
							drive.leftWheel.write(value);
						else if(strcmp(dest, "lc") == 0)	// left motor center
						{
							drive.leftWheel.setCenter(value);
							eeprom.data.leftWheelCenter = value;
						}
						else if(strcmp(dest, "r") == 0)		// right motor
							drive.rightWheel.write(value);
						else if(strcmp(dest, "rc") == 0)	// right motor center
						{
							drive.rightWheel.setCenter(value);
							eeprom.data.rightWheelCenter = value;
						}
						else if(strcmp(dest,"p") == 0)		// program
							eeprom.data.selectedProgram = value;
						else if(strcmp(dest,"i") == 0)		// inch delay
							eeprom.data.inch = value;
					}
				}
			}
			else if(strcmp(tok, "get") == 0)	// action GET reads a variable
			{
				tok = strtok_r(NULL, " ", &pqp);
				if (tok)			// second token is the VAR to be read
				{
					if(strcmp(tok, "l") == 0)				// left motor
					{
						Serial.print("L ");
						Serial.println(drive.leftWheel.read());
					}
					else if(strcmp(tok, "lc") == 0)			// left motor center
					{
						Serial.print("LC ");
						Serial.println(eeprom.data.leftWheelCenter);
					}
					else if(strcmp(tok, "r") == 0)			// right motor
					{
						Serial.print("R ");
						Serial.println(drive.rightWheel.read());
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
					else if(strcmp(tok,"i") == 0)			// inch delay
					{
						Serial.print("I ");
						Serial.println(eeprom.data.inch);
					}
					else if(strcmp(tok,"as") == 0)			// all analog sensors
						displayAnalogSensors();
				}
			}
			else if(strcmp(tok, "save") == 0)	// save stuff to eeprom
				eeprom.save();
			else if(strcmp(tok, "load") == 0)	// discard changes and reload from eeprom
				eeprom.load();
			else if(strcmp(tok, "cal") == 0)	// re-calibrate line following IR sensors
				lineFollower.autoCalibrate();
			else if(strcmp(tok, "default") == 0)
			{
				eeprom.data.selectedProgram = 0;
				eeprom.data.leftWheelCenter = 1410;
				eeprom.data.rightWheelCenter = 1384;
				eeprom.data.inch = 500; // perhaps half a second?
				for(int x = 0; x < NUM_IR_TRACK; x++)
				{
					eeprom.data.LF_threshold[x] = 512;
					eeprom.data.LF_reverseColor = false;
				}
				eeprom.save();
			}
			else if(strcmp(tok, "inch") == 0)
			{
				drive.inch();
			}
			else if(strcmp(tok, "stop") == 0)
			{
				drive.stop();
			}
		}
	}
}

/******************************************************************************
 *	SETUP
 ******************************************************************************/

void setup()
{
	Serial.begin(115200);

	eeprom.load();

	drive.leftWheel.init(8, eeprom.data.leftWheelCenter);
	drive.rightWheel.init(9, eeprom.data.rightWheelCenter, true);

	if (eeprom.data.selectedProgram == PRG_LINEFOLLOWER)
		lineFollower.autoCalibrate();
}

/******************************************************************************
 *	MAIN LOOP
 ******************************************************************************/

void loop()
{
	server.loop();

	switch(eeprom.data.selectedProgram)
	{
		case PRG_RC:
			drive.refresh();
		break;
		case PRG_LINEFOLLOWER:
			lineFollower.loop();
		break;
		case PRG_PHOTOVORE:
			photovore();
		break;
		case PRG_SHOW_SENSORS:
			displayAnalogSensors();
			delay(500);
		break;
	}
	delay(15);
}
