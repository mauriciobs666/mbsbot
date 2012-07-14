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

#define BOARD_VERSION 1

// Tipo de controle do motor das rodas
//#define RODAS_PWM 1 // PWM
#undef RODAS_PWM // servo (placa_v1.h)

#define PINO_MOTOR_ESQ		8
#define PINO_MOTOR_DIR		9

// economia de energia
#define PINO_UNUSED_CNT 4
#define PINO_UNUSED_ARRAY { 6,7,11,12 }

#define PINO_SERVO_PAN		10
//#define PINO_SERVO_TILT
//#define PINO_SERVO_ROLL

// LINE FOLLOWER (pinos analogicos!)
#define NUM_IR_TRACK		3
#define PINO_FIRST_IR_SENSOR 1
// onde NUM_IR_TRACK=3 e PINO_FIRST_IR_SENSOR=2 significa que os pinos A2, A3 e A4 estao conectados

#define PINO_BIP 			13

#define PINO_LED             13

// habilita wiichuck
//#define WIICHUCK
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
