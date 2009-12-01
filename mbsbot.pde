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

void setup()
{
	lcd.begin(16, 2); // LCD's number of rows and columns
	//Serial.begin(9600);
}

void loop()
{
	unsigned short analogSensor[6];	// analog sensors cache

	// read sensor data
	for(int x=0; x<6; x++)
		analogSensor[x]=analogRead(x);

	// display them
	displayAnalogSensors(analogSensor);
}

void displayAnalogSensors(unsigned short sensors[])
{
	char linha[17];

	sprintf(linha," %04d %04d %04d", sensors[0], sensors[1], sensors[2]);
	lcd.setCursor(0, 0);
	lcd.print(linha);

	sprintf(linha," %04d %04d %04d", sensors[3], sensors[4], sensors[5]);
	lcd.setCursor(0, 1);

	lcd.print(linha);
}
