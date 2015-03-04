/**	Copyright (C) 2010-2015 - Mauricio Bieze Stefani
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

// M2D2

#define VERSAO_PLACA 2

#define SERIALX Serial

#define DFT_PROGRAMA  1 /* PRG_RC_SERIAL = 1 */
#define DFT_DELAY_ES  1
#define DFT_FREIO_MAO 0

#define MOTOR_ACEL      255
#define MOTOR_CENTRO     42

#define DFT_VEL_MAX     100
#define DFT_VEL_ESCALA   80
#define DFT_VEL_REFRESH   1
#define DFT_BALANCO       0
#define DFT_PID_DEBOUNCE 35

#define DFT_PID_P             12
#define DFT_PID_I            500
#define DFT_PID_D           5000
#define DFT_PID_MAX_MV       100
#define DFT_PID_MIN_MV      -100
#define DFT_PID_ZACC        true
#define DFT_PID_DENTRADA    true
#define DFT_PID_SAMPLE         5

// Habilita controle PWM (se undef controla por largura de pulso/servo)
#define RODAS_PWM 1

// inverte sentido dos motores
#define MOTOR_ESQ_INV 0
#define MOTOR_DIR_INV 0
#define MOTOR_E_T_INV 0
#define MOTOR_D_T_INV 0

// canal 1
#define PINO_MOTOR_ESQ		4
#define PINO_MOTOR_ESQ_PWM	5
// canal 2
#define PINO_MOTOR_DIR		2
#define PINO_MOTOR_DIR_PWM	3

// economia de energia
#define PINO_UNUSED_CNT 2
#define PINO_UNUSED_ARRAY { 7,11 }

//#define PINO_SERVO_PAN		9
//#define PINO_SERVO_TILT		10
//#define PINO_SERVO_ROLL		8

#define NUM_SENSORES    6

#define LINE_FOLLOWER
#define NUM_IR_TRACK    NUM_SENSORES
#define PINO_TRACK_0    0
// ex: NUM_IR_TRACK=3 e PINO_TRACK_0=2 significa que os pinos A2, A3 e A4 estao conectados

// http://arduino.cc/en/Reference/Tone
// Use of the tone() function will interfere with PWM output on pins 3 and 11 (on boards other than the Mega).
//#define PINO_BIP 			18

#define PINO_ARMA           6

#define PINO_LED            13

// habilita wiichuck / energia nas I2C/analogicas
//#define WIICHUCK
//#define WIICHUCK_POWER

// RADIO CONTROLE
//#define PINO_JOY_X      2
//#define PINO_JOY_Y      3
//#define PINO_JOY_Z
//#define PINO_JOY_R
//#define PINO_JOY_SW1    4

#endif // BOARD_H_INCLUDED
