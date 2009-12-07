/*
 *	Copyright (C) 2009 - Mauricio Bieze Stefani
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

#include <LiquidCrystal.h>

/*
LCD connection:
* LCD RS pin to digital pin 12
* LCD Enable pin to digital pin 11
* LCD D4 pin 11 to digital pin 5
* LCD D5 pin 12 to digital pin 4
* LCD D6 pin 13 to digital pin 3
* LCD D7 pin 14 to digital pin 2
* ends to +5V and ground
* wiper to LCD VO pin (pin 3)

http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// NUM_IR_TRACK=3 FIRST_IR_SENSOR_INDEX=0 means pins A0, A1 and A2 are connected
#define NUM_IR_TRACK 3
#define FIRST_IR_SENSOR_INDEX 0
unsigned short IRSensor[NUM_IR_TRACK];
unsigned short IRSensorThreshold[NUM_IR_TRACK];

// define number of redundant reads during autocalibration
#define CAL_READS_NUM 5
#define CAL_READS_INTERVAL 200

bool reverseTrackColor = false;

char linha[32];

void setup()
{
	Serial.begin(9600);

	lcd.begin(16, 2); // LCD's number of (columns, rows)

	autoCalibrate();
}

void loop()
{
	// read IR sensor data and map into IRSensor[]
	for(int x = 0; x < NUM_IR_TRACK; x++)
	{
//		delay(1);
		IRSensor[x] = analogRead(FIRST_IR_SENSOR_INDEX + x);
	}

	sprintf(linha, "%c%04d%c%04d%c%04d", 	(IRSensor[0] > IRSensorThreshold[0]) ^ reverseTrackColor ? '|' : '-', IRSensor[0],
											(IRSensor[1] > IRSensorThreshold[1]) ^ reverseTrackColor ? '|' : '-', IRSensor[1],
											(IRSensor[2] > IRSensorThreshold[2]) ^ reverseTrackColor ? '|' : '-', IRSensor[2]);

	lcd.setCursor(0, 1);
	lcd.print(linha);

	// display all analog sensors
	//displayAnalogSensors();
}

void displayAnalogSensors()
{
	sprintf(linha, " %04d %04d %04d", analogRead(0), analogRead(1), analogRead(2));
	lcd.setCursor(0, 0);
	lcd.print(linha);

	Serial.println(linha);

	sprintf(linha, " %04d %04d %04d", analogRead(3), analogRead(4), analogRead(5));
	lcd.setCursor(0, 1);
	lcd.print(linha);

	Serial.println(linha);
}

void autoCalibrate()
{
	unsigned short sensorTrack[NUM_IR_TRACK];
	unsigned short sensorOut[NUM_IR_TRACK];

	lcd.setCursor(0, 0); // ( columm , line )

	lcd.print("Calibrate TRACK");
	Serial.println("Calibrating TRACK");

	// read all sensors at TRACK position several times

	memset(sensorTrack, 0, sizeof(sensorTrack));
	for(int x=0; x < CAL_READS_NUM; x++)
	{
		for(int y=0; y < NUM_IR_TRACK; y++)
			sensorTrack[y] += analogRead(FIRST_IR_SENSOR_INDEX + y);

		sprintf(linha," %04d %04d %04d", sensorTrack[0], sensorTrack[1], sensorTrack[2]);
		lcd.setCursor(0, 1);
		lcd.print(linha);

		Serial.println(linha);

		delay(CAL_READS_INTERVAL);
	}

	// pause so the user can move the sensors outside the track
	lcd.setCursor(0, 0);
	lcd.print("Calibrate OUTSIDE");
	Serial.println("Calibrate OUTSIDE");

	for(int x=5; x>0; x--)
	{
		lcd.setCursor(0, 1);
		sprintf(linha, "Start in %ds    ", x);
		lcd.print(linha);

		Serial.println(linha);

		delay(1000);
	}

	// read all sensors at OUTSIDE position several times

	memset(sensorOut, 0, sizeof(sensorOut));
	for(int x=0; x < CAL_READS_NUM; x++)
	{
		for(int y=0; y < NUM_IR_TRACK; y++)
			sensorOut[y] += analogRead(FIRST_IR_SENSOR_INDEX + y);

		sprintf(linha," %04d %04d %04d ", sensorOut[0], sensorOut[1], sensorOut[2]);
		lcd.setCursor(0, 1);
		lcd.print(linha);

		Serial.println(linha);

		delay(CAL_READS_INTERVAL);
	}

	for(int y=0; y < NUM_IR_TRACK; y++)
	{
		unsigned short mediumTrack = (sensorTrack[y] / CAL_READS_NUM);
		unsigned short mediumOut = (sensorOut[y] / CAL_READS_NUM);

		reverseTrackColor = ( mediumOut > mediumTrack );

		IRSensorThreshold[y] = ( mediumTrack + mediumOut ) / 2;
	}

	Serial.println("Thresholds:");
	sprintf(linha," %04d %04d %04d", IRSensorThreshold[0], IRSensorThreshold[1], IRSensorThreshold[2]);
	lcd.setCursor(0, 0);
	lcd.print(linha);

	Serial.println(linha);
}
