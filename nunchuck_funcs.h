/*
 * Nunchuck functions  -- Talk to a Wii Nunchuck
 *
 * This library is from the Bionic Arduino course :
 *                          http://todbot.com/blog/bionicarduino/
 *
 * 2007 Tod E. Kurt, http://todbot.com/blog/
 *
 * The Wii Nunchuck reading code originally from Windmeadow Labs
 *   http://www.windmeadow.com/node/42
 *
 * 2010 - MBS -  fixed with patch found at:
 *               http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1264805255/10
 */

#include <WProgram.h>

static uint8_t nunchuck_buf[6];   // array to store nunchuck data,

// Uses port C (analog in) pins as power & ground for Nunchuck
static void nunchuck_setpowerpins()
{
#define pwrpin PORTC3
#define gndpin PORTC2
    DDRC |= _BV(pwrpin) | _BV(gndpin);
    PORTC &=~ _BV(gndpin);
    PORTC |=  _BV(pwrpin);
    delay(100);  // wait for things to stabilize
}

// initialize the I2C system, join the I2C bus,
// and tell the nunchuck we're talking to it
static void nunchuck_init()
{
    /* MBS - old init code
    Wire.begin();                // join i2c bus as master
    Wire.beginTransmission(0x52);// transmit to device 0x52
    Wire.send(0x40);// sends memory address
    Wire.send(0x00);// sends sent a zero.
    Wire.endTransmission();// stop transmitting
    */
    byte cnt;

    Wire.begin();

// init controller
    delay(1);
    Wire.beginTransmission(0x52);   // device address
    Wire.send(0xF0);              // 1st initialisation register
    Wire.send(0x55);              // 1st initialisation value
    Wire.endTransmission();
    delay(1);
    Wire.beginTransmission(0x52);
    Wire.send(0xFB);              // 2nd initialisation register
    Wire.send(0x00);              // 2nd initialisation value
    Wire.endTransmission();
    delay(1);

// read the extension type from the register block
    Wire.beginTransmission(0x52);
    Wire.send(0xFA);              // extension type register
    Wire.endTransmission();
    Wire.beginTransmission(0x52);
    Wire.requestFrom(0x52, 6);            // request data from controller
    for (cnt = 0; cnt < 6; cnt++)
    {
        if (Wire.available())
        {
            nunchuck_buf[cnt] = Wire.receive(); // Should be 0x0000 A420 0101 for Classic Controller, 0x0000 A420 0000 for nunchuck
        }
    }
    Wire.endTransmission();
    delay(1);

// send the crypto key (zeros), in 3 blocks of 6, 6 & 4.
    Wire.beginTransmission(0x52);
    Wire.send(0xF0);              // crypto key command register
    Wire.send(0xAA);              // sends crypto enable notice
    Wire.endTransmission();
    delay(1);
    Wire.beginTransmission(0x52);
    Wire.send(0x40);              // crypto key data address
    for (cnt = 0; cnt < 6; cnt++)
    {
        Wire.send(0x00);              // sends 1st key block (zeros)
    }
    Wire.endTransmission();
    Wire.beginTransmission(0x52);
    Wire.send(0x40);              // sends memory address
    for (cnt = 6; cnt < 12; cnt++)
    {
        Wire.send(0x00);              // sends 2nd key block (zeros)
    }
    Wire.endTransmission();
    Wire.beginTransmission(0x52);
    Wire.send(0x40);              // sends memory address
    for (cnt = 12; cnt < 16; cnt++)
    {
        Wire.send(0x00);              // sends 3rd key block (zeros)
    }
    Wire.endTransmission();
    delay(1);
// end device init
}

// Send a request for data to the nunchuck
// was "send_zero()"
static void nunchuck_send_request()
{
    Wire.beginTransmission(0x52);// transmit to device 0x52
    Wire.send(0x00);// sends one byte
    Wire.endTransmission();// stop transmitting
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
static char nunchuk_decode_byte (char x)
{
    x = (x ^ 0x17) + 0x17;
    return x;
}

// Receive data back from the nunchuck,
// returns 1 on successful read. returns 0 on failure
static int nunchuck_get_data()
{
    int cnt=0;
    Wire.requestFrom (0x52, 6);// request data from nunchuck
    while (Wire.available ())
    {
        // receive byte as an integer
        nunchuck_buf[cnt] = nunchuk_decode_byte(Wire.receive());
        cnt++;
    }
    nunchuck_send_request();  // send request for next data payload
    // If we recieved the 6 bytes, then go print them
    if (cnt >= 5)
    {
        return 1;   // success
    }
    return 0; //failure
}

// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_zbutton()
{
    return ((nunchuck_buf[5] >> 0) & 1) ? 0 : 1;
}

// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_cbutton()
{
    return ((nunchuck_buf[5] >> 1) & 1) ? 0 : 1;
}

// returns value of x-axis joystick
static int nunchuck_joyx()
{
    return nunchuck_buf[0];
}

// returns value of y-axis joystick
static int nunchuck_joyy()
{
    return nunchuck_buf[1];
}

// returns value of x-axis accelerometer
static int nunchuck_accelx()
{
    // accel data is 10 bits long

    int accel_x_axis = nunchuck_buf[2] << 2;

    if ((nunchuck_buf[5] >> 2) & 1)
        accel_x_axis += 2;

    if ((nunchuck_buf[5] >> 3) & 1)
        accel_x_axis += 1;

    return accel_x_axis;

    //return (nunchuck_buf[2] << 2) | ((nunchuck_buf[5] >> 1) & 2) | ((nunchuck_buf[5] >> 3) & 1);
}

// returns value of y-axis accelerometer
static int nunchuck_accely()
{
    int accel_y_axis = nunchuck_buf[3] * 2 * 2;
    if ((nunchuck_buf[5] >> 4) & 1)
        accel_y_axis += 2;
    if ((nunchuck_buf[5] >> 5) & 1)
        accel_y_axis += 1;

    return accel_y_axis;

    //return (nunchuck_buf[3] << 2) | ((nunchuck_buf[5] >> 3) & 2) | ((nunchuck_buf[5] >> 5) & 1);
}

// returns value of z-axis accelerometer
static int nunchuck_accelz()
{
    int accel_z_axis = nunchuck_buf[4] * 2 * 2;
    if ((nunchuck_buf[5] >> 6) & 1)
        accel_z_axis += 2;
    if ((nunchuck_buf[5] >> 7) & 1)
        accel_z_axis += 1;

    return accel_z_axis;

    //return (nunchuck_buf[4] << 2) | ((nunchuck_buf[5] >> 5) & 2) | ((nunchuck_buf[5] >> 7) & 1);
}

// Print the input data we have recieved
static void nunchuck_print_data()
{
    Serial.print("joy:");
    Serial.print(nunchuck_joyx(),DEC);
    Serial.print(",");
    Serial.print(nunchuck_joyy(), DEC);
    Serial.print("  \t");

    Serial.print("acc:");
    Serial.print(nunchuck_accelx(), DEC);
    Serial.print(",");
    Serial.print(nunchuck_accely(), DEC);
    Serial.print(",");
    Serial.print(nunchuck_accelz(), DEC);
    Serial.print("\t");

    Serial.print("but:");
    Serial.print(nunchuck_zbutton(), DEC);
    Serial.print(",");
    Serial.println(nunchuck_cbutton(), DEC);
}
