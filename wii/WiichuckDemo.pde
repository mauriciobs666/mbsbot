/*
 * WiiChuckDemo --
 *
 * 2008 Tod E. Kurt, http://thingm.com/
 *
 */

#include <Wire.h>
#include "nunchuck_funcs.h"

int loop_cnt=0;

int accx,accy,accz;
int zbut,cbut;
int joyx, joyy;

void setup()
{
    Serial.begin(115200);

    nunchuck_setpowerpins();

    nunchuck_init(); // send the initilization handshake
}

void loop()
{
    if( loop_cnt > 100 )   // every 100 msecs get new data
    {
        loop_cnt = 0;

        nunchuck_get_data();

        //nunchuck_print_data();

        accx  = nunchuck_accelx(); // ranges from approx 70 - 182
        accy  = nunchuck_accely(); // ranges from approx 65 - 173
        accz  = nunchuck_accelz(); // ranges from approx 65 - 173

        zbut = nunchuck_zbutton();
        cbut = nunchuck_cbutton();

        joyx = nunchuck_joyx();
        joyy = nunchuck_joyy();

        Serial.print("accel ( ");
        Serial.print((byte)accx,DEC);
        Serial.print(" , ");
        Serial.print((byte)accy,DEC);
        Serial.print(" , ");
        Serial.print((byte)accz,DEC);
        Serial.print(" ) \tjoy ( ");
        Serial.print((byte)joyx,DEC);
        Serial.print(" , ");
        Serial.print((byte)joyy,DEC);
        Serial.print(" ) \tzbut: ");
        Serial.print((byte)zbut,DEC);
        Serial.print("\tcbut: ");
        Serial.println((byte)cbut,DEC);
    }
    loop_cnt++;
    delay(10);
}

