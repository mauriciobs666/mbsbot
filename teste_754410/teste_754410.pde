const int PWMA= 10;
const int PWMB= 9;
const int DIRA= 13;
const int DIRB= 12;

void setup()
{
    pinMode(DIRA, OUTPUT);
    pinMode(DIRB, OUTPUT);

    Serial.begin(115200);
}

void loop()
{
    int pot=analogRead(0);

    int dir;
    int value;

    if(pot < 512)
    {
        dir=LOW;
        value=(511-pot)/2;
    }
    else
    {
        dir=HIGH;
        value=(pot-512)/2;
    }

    digitalWrite(DIRA, dir);
    analogWrite(PWMA, value);

    digitalWrite(DIRB, dir);
    analogWrite(PWMB, value);

	Serial.print("DIR = ");
	Serial.print(dir);
	Serial.print(" VAL = ");
	Serial.println(value);

	delay(50);
}

