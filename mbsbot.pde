/*	Copyright (C) 2009 - Mauricio Bieze Stefani
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

/* LCD Display

* LCD RS pin to digital pin 7
* LCD Enable pin to digital pin 6
* LCD D4 pin 11 to digital pin 5
* LCD D5 pin 12 to digital pin 4
* LCD D6 pin 13 to digital pin 3
* LCD D7 pin 14 to digital pin 2
* ends to +5V and ground
* wiper to LCD VO pin (pin 3)

http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
char linha[32]; //string temp to use with sprintf

#include <Servo.h>

class Wheel
{
public:
	void init(int pin, short centerAng=90, bool reverseDirection=false)
	{
		servo.attach(pin);
		centerAngle=centerAng;
		reverse=reverseDirection;
		stop();
	}
	void stop()
	{
		current=centerAngle;
		refresh();
	}
	void refresh()
	{
		servo.write(current);
	}
	void move(char percent)
	{
		current = reverse ? (centerAngle - percent) : (centerAngle + percent);
		refresh();
	}
private:
	Servo servo;
	bool reverse;
	short centerAngle;
	short current;

} leftWheel, rightWheel;


class Server
{
public:
	Server() : pos(0) {}
private:
	char command[10];
	char pos;
} server;


// NUM_IR_TRACK=3 FIRST_IR_SENSOR_INDEX=0 means pins A0, A1 and A2 are connected
#define NUM_IR_TRACK 2
#define FIRST_IR_SENSOR_INDEX 0
unsigned short IRSensor[NUM_IR_TRACK];
unsigned short IRSensorThreshold[NUM_IR_TRACK];

// define number of redundant reads during autocalibration
#define CAL_READS_NUM 5
#define CAL_READS_INTERVAL 200

bool reverseTrackColor = false;

#define PRG_SEL_0 11
#define PRG_SEL_1 12
#define PRG_SEL_2 13

char selectedProgram=0;

#define PRG_LINEFOLLOWER	0x05
#define PRG_PHOTOVORE 		0x06
#define PRG_SHOW_SENSORS 	0x07

void setup()
{
	// program selection pins
	pinMode(PRG_SEL_0, INPUT);
	pinMode(PRG_SEL_1, INPUT);
	pinMode(PRG_SEL_2, INPUT);

	// enable pull-ups
	digitalWrite(PRG_SEL_0, HIGH);
	digitalWrite(PRG_SEL_1, HIGH);
	digitalWrite(PRG_SEL_2, HIGH);

	if(digitalRead(PRG_SEL_2))
		selectedProgram |= 0x04;

	if(digitalRead(PRG_SEL_1))
		selectedProgram |= 0x02;

	if(digitalRead(PRG_SEL_0))
		selectedProgram |= 0x01;

	Serial.begin(9600);

	lcd.begin(16, 2); // LCD's number of (columns, rows)

	leftWheel.init(8,86);
	rightWheel.init(9,83,true);

	if(selectedProgram == PRG_LINEFOLLOWER)
		autoCalibrateLineFollower();
}

void loop()
{
/*
	// read IR sensor data and map into IRSensor[]
	for(int x = 0; x < NUM_IR_TRACK; x++)
		IRSensor[x] = analogRead(FIRST_IR_SENSOR_INDEX + x);

	sprintf(linha, "%c%04d%c%04d%c%04d", 	(IRSensor[0] > IRSensorThreshold[0]) ^ reverseTrackColor ? '|' : '-', IRSensor[0],
											(IRSensor[1] > IRSensorThreshold[1]) ^ reverseTrackColor ? '|' : '-', IRSensor[1],
											(IRSensor[2] > IRSensorThreshold[2]) ^ reverseTrackColor ? '|' : '-', IRSensor[2]);
	lcd.setCursor(0, 1);
	lcd.print(linha);
*/

//	rightWheel.write();
//	leftWheel.write();

//	lcd.setCursor(0, 0);
//	lcd.print(val, DEC);

	if(selectedProgram == PRG_PHOTOVORE)
		photovore();

	// display all analog sensors
	if(selectedProgram == PRG_SHOW_SENSORS)
		displayAnalogSensors();

	delay(15);
}

void photovore()
{
	int left = analogRead(0);
	int right = analogRead(1);

	const int threshold = 25;
	const int power=100;

	//smaller numbers mean more light

	if ( (right - left) > threshold ) // turn left
	{
		leftWheel.move(-power);
		rightWheel.move(power);
	}
	else if ( (left - right) > threshold ) //turn right
	{
		leftWheel.move(power);
		rightWheel.move(-power);
	}
	else // go ahead
	{
		leftWheel.move(power);
		rightWheel.move(power);
	}
}

void displayAnalogSensors()
{
	sprintf(linha, " %04d %04d %04d", analogRead(0), analogRead(1), analogRead(2));
	lcd.setCursor(0, 0);
	lcd.print(linha);

	Serial.print(linha);

	sprintf(linha, " %04d %04d %04d", analogRead(3), analogRead(4), analogRead(5));
	lcd.setCursor(0, 1);
	lcd.print(linha);

	Serial.println(linha);
}

void autoCalibrateLineFollower()
{
	unsigned short sensorTemp[NUM_IR_TRACK];

	unsigned short sensorTrack[NUM_IR_TRACK];
	unsigned short sensorTrackMax[NUM_IR_TRACK];
	unsigned short sensorTrackMin[NUM_IR_TRACK];

	unsigned short sensorOut[NUM_IR_TRACK];
	unsigned short sensorOutMax[NUM_IR_TRACK];
	unsigned short sensorOutMin[NUM_IR_TRACK];

	lcd.clear();
	lcd.setCursor(0, 0); // ( columm , line )

	lcd.print("Calibrate TRACK");
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
		lcd.setCursor(0, 1);
		lcd.print(linha);

		Serial.println(linha);

		delay(CAL_READS_INTERVAL);
	}

	// pause so the user can move the sensors outside the track

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Cal. OUTSIDE");
	Serial.println("Calibrate OUTSIDE");

	for(int x=5; x>0; x--)
	{
		lcd.setCursor(0, 1);
		sprintf(linha, "Start in %ds", x);
		lcd.print(linha);

		//Serial.println(linha);

		delay(1000);
	}

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
		lcd.setCursor(0, 1);
		lcd.print(linha);

		Serial.println(linha);

		delay(CAL_READS_INTERVAL);
	}

	bool reverseSensor[NUM_IR_TRACK];

	for(int y=0; y < NUM_IR_TRACK; y++)
	{
		unsigned short mediumTrack = (sensorTrack[y] / CAL_READS_NUM);
		unsigned short mediumOut = (sensorOut[y] / CAL_READS_NUM);

		reverseSensor[y] = ( mediumOut > mediumTrack );

		IRSensorThreshold[y] = ( mediumTrack + mediumOut ) / 2;
	}

	// if one sensor is reversed then all others must also be!
	lcd.clear();
	reverseTrackColor = reverseSensor[0];

	if(reverseTrackColor)
		Serial.println("Reversed Track Color");
	else
		Serial.println("Normal Track Color");

	for(int x=1; x < NUM_IR_TRACK; x++)
		if (reverseSensor[x] ^ reverseTrackColor)
		{
			sprintf(linha, "BAD Reverse %d", x);
			lcd.setCursor(0, 0);
			lcd.print(linha);

			Serial.println(linha);
			return;
		}

	for(int x=0; x < NUM_IR_TRACK; x++)
	{
		if(reverseTrackColor)
		{
			if(sensorOutMin[x] <= sensorTrackMax[x])
			{
				sprintf(linha, "BAD limits %d", x);
				lcd.setCursor(0, 0);
				lcd.print(linha);

				Serial.println(linha);
				return;
			}
		}
		else
		{
			if(sensorOutMax[x] >= sensorTrackMin[x])
			{
				sprintf(linha, "BAD limits %d", x);
				lcd.setCursor(0, 0);
				lcd.print(linha);

				Serial.println(linha);
				return;
			}
		}
	}

	Serial.println("Thresholds:");
	sprintf(linha," %04d %04d %04d", IRSensorThreshold[0], IRSensorThreshold[1], IRSensorThreshold[2]);
	lcd.setCursor(0, 0);
	lcd.print(linha);

	Serial.println(linha);
}
