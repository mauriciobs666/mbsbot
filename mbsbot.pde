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

#define NUM_IR_TRACK 3
unsigned short IRSensorThreshold[NUM_IR_TRACK];

#define CAL_READS_NUM 10
#define CAL_READS_INTERVAL 200

bool reverseTrackColor = false;

char linha[17];

void setup()
{
	unsigned short sensorTrack[NUM_IR_TRACK];
	unsigned short sensorOut[NUM_IR_TRACK];

	for(int y=0; y < NUM_IR_TRACK; y++)
		sensorTrack[y] = sensorOut[y] = 0;

	Serial.begin(9600);

	lcd.begin(16, 2); // LCD's number of rows and columns

	lcd.setCursor(0, 0); // ( columm , line )

	lcd.print("Calibra TRILHA  ");
	Serial.println("Calibra TRILHA");

	// read all sensors at TRACK position several times
	for(int x=0; x < CAL_READS_NUM; x++)
	{
		for(int y=0; y < NUM_IR_TRACK; y++)
			sensorTrack[y] += analogRead(y);

		sprintf(linha," %04d %04d %04d", sensorTrack[0], sensorTrack[1], sensorTrack[2]);
		lcd.setCursor(0, 1);
		lcd.print(linha);
		Serial.println(linha);

		delay(CAL_READS_INTERVAL);
	}

	// pause so the user can move the sensors outside the track
	lcd.setCursor(0, 0);
	lcd.print("Calibra FORA    ");
	Serial.println("Calibra FORA");

	for(int x=5; x>0; x--)
	{
		lcd.setCursor(0, 1);
		sprintf(linha, "Inicio em %ds    ", x);
		lcd.print(linha);
		Serial.println(linha);

		delay(1000);
	}

	// read all sensors at OUTSIDE position 10 times
	for(int x=0; x < CAL_READS_NUM; x++)
	{
		for(int y=0; y < NUM_IR_TRACK; y++)
			sensorOut[y] += analogRead(y);

		sprintf(linha," %04d %04d %04d", sensorOut[0], sensorOut[1], sensorOut[2]);
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

void loop()
{
	unsigned short analogSensor[6];	// analog sensors cache

	// read sensor data
	for(int x=0; x<6; x++)
		analogSensor[x]=analogRead(x);

	sprintf(linha, "%c%04d%c%04d%c%04d", 	(analogSensor[0] > IRSensorThreshold[0]) ^ reverseTrackColor ? '|' : '-', analogSensor[0],
											(analogSensor[1] > IRSensorThreshold[1]) ^ reverseTrackColor ? '|' : '-', analogSensor[1],
											(analogSensor[2] > IRSensorThreshold[2]) ^ reverseTrackColor ? '|' : '-', analogSensor[2]);
	lcd.setCursor(0, 1);
	lcd.print(linha);

	// display them
	//displayAnalogSensors(analogSensor);
}

void displayAnalogSensors(unsigned short sensors[])
{
	sprintf(linha, " %04d %04d %04d", sensors[0], sensors[1], sensors[2]);
	lcd.setCursor(0, 0);
	lcd.print(linha);

	sprintf(linha, " %04d %04d %04d", sensors[3], sensors[4], sensors[5]);
	lcd.setCursor(0, 1);
	lcd.print(linha);
}
