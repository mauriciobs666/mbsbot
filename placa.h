/**	Copyright (C) 2010-2011 - Mauricio Bieze Stefani
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

#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#define BOARD_VERSION 2

// WHEELS - are them conected to a Servo or a DC motor? Pick one:
#define WHEEL_DC 1 // DC motor through 754410 driver
//#undef WHEEL_DC // servo (old board)

#define PINO_MOTOR_ESQ		4
#define PINO_MOTOR_ESQ_PWM	5
#define PINO_MOTOR_DIR		2
#define PINO_MOTOR_DIR_PWM	3

// number of unused pins (to save power)
#define PINO_UNUSED_CNT 2
#define PINO_UNUSED_ARRAY { 7,11 }

// SHARP RANGE FINDER (analog pin!)
#define PINO_SHARP_RF		3

// SONAR
#define PINO_SONAR           1

#define PINO_SERVO_PAN		9
#define PINO_SERVO_TILT		10
#define PINO_SERVO_ROLL		8

// LINE FOLLOWER (analog pins!)
#define NUM_IR_TRACK		5
#define PINO_FIRST_IR_SENSOR 1
// where NUM_IR_TRACK=3 and PINO_FIRST_IR_SENSOR=2 means pins A2, A3 and A4 are connected

#define PINO_BIP 			12

#define PINO_LASER           6

#define PINO_LED             13

// enable/disable wiichuck stuff
#define WIICHUCK
//#define WIICHUCK_POWER

#ifdef WIICHUCK
    #ifdef WIICHUCK_POWER
        #define PINO_ANALOG_CNT 2
    #else
        #define PINO_ANALOG_CNT 4
    #endif
#else
    #define PINO_ANALOG_CNT 6
#endif

#endif // BOARD_H_INCLUDED
