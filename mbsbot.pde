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
	lcd.setCursor(0, 0);
	lcd.print(analogRead(0), DEC);
}
