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

#ifndef PLACA_H_INCLUDED
#define PLACA_H_INCLUDED

#define VERSAO_PLACA 4

#define SERIALX Serial1

#define DFT_PROGRAMA  1 /* PRG_RC_SERIAL = 1 */
#define DFT_DELAY_ES  1
#define DFT_FREIO_MAO 0

#define MOTOR_ACEL      255
#define MOTOR_CENTRO     42

#define DFT_VEL_MAX     100
#define DFT_VEL_ESCALA  100
#define DFT_DELAY_MOTOR  10
#define DFT_BALANCO       0
#define DFT_LF_DEBOUNCE  35

#define CAL_PID_P (float)0.05000
#define CAL_PID_I (float)0.00100
#define CAL_PID_D (float)      0
#define CAL_PID_MAX_MV       100
#define CAL_PID_MIN_MV      -100
#define CAL_PID_ZACC           0
#define CAL_PID_DENTRADA    true
#define CAL_PID_SAMPLE        10

#define DFT_PID_P (float)0.15000
#define DFT_PID_I (float)      0
#define DFT_PID_D (float)0.00200
#define DFT_PID_MAX_MV       100
#define DFT_PID_MIN_MV      -100
#define DFT_PID_ZACC          50
#define DFT_PID_DENTRADA    true
#define DFT_PID_SAMPLE        10

#define DFT_PID_RETA_P           DFT_PID_P
#define DFT_PID_RETA_I           DFT_PID_I
#define DFT_PID_RETA_D           DFT_PID_D
#define DFT_PID_RETA_MAX_MV      DFT_PID_MAX_MV
#define DFT_PID_RETA_MIN_MV      DFT_PID_MIN_MV
#define DFT_PID_RETA_ZACC        DFT_PID_ZACC
#define DFT_PID_RETA_DENTRADA    DFT_PID_DENTRADA
#define DFT_PID_RETA_SAMPLE      DFT_PID_SAMPLE

#define PID_MOTOR_P  (float) 10
#define PID_MOTOR_I  (float)  1
#define PID_MOTOR_D  (float)  0

// inverte sentido dos motores
#define MOTOR_ESQ_INV 0
#define MOTOR_DIR_INV 0
#define MOTOR_E_T_INV 0
#define MOTOR_D_T_INV 0

// canal 1
#define PINO_MOTOR_ESQ  	7
#define PINO_MOTOR_ESQ_N	8
#define PINO_MOTOR_ESQ_PWM	5

// canal 2
#define PINO_MOTOR_DIR      9
#define PINO_MOTOR_DIR_N    12
#define PINO_MOTOR_DIR_PWM	6

// economia de energia
#define PINO_UNUSED_CNT 0
#define PINO_UNUSED_ARRAY { }

//#define PINO_SERVO_PAN		9
//#define PINO_SERVO_TILT		10
//#define PINO_SERVO_ROLL		8

#define NUM_SENSORES    16
#define THRESHOLD_CAL   200

#define LINE_FOLLOWER

#define LF_NUM_SENSORES NUM_SENSORES
#define LF_PINO_0       0
#define LF_PINO_N       ( LF_PINO_0 + LF_NUM_SENSORES )
#define LF_FATOR_S      100
#define LF_RANGE        ( (NUM_SENSORES-1) * LF_FATOR_S )
#define LF_SETPOINT     ( LF_RANGE / 2 )
#define LF_TIMEOUT      1000
#define LF_TIMEOUT_CAL  3000
// ex: LF_NUM_SENSORES = 3 e LF_PINO_0 = 2 significa que os pinos A2, A3 e A4 estao conectados a uma barra de sensores IR

// http://arduino.cc/en/Reference/Tone
// Use of the tone() function will interfere with PWM output on pins 3 and 11 (on boards other than the Mega).
//#define PINO_BIP 			11

//#define PINO_ARMA           10

#define PINO_LED            13

// RADIO CONTROLE
//#define PINO_JOY_X      2
//#define PINO_JOY_Y      3
//#define PINO_JOY_Z      17
//#define PINO_JOY_R      18
//#define PINO_JOY_SW1    4

#define PINO_BOTAO_CAL      37
#define PINO_BOTAO_PRG      39

#define ENCODER_RODAS
// https://github.com/GreyGnome/EnableInterrupt/wiki/Usage-HiSpeed
#define NEEDFORSPEED
#define INTERRUPT_FLAG_PIN10 encoderDir1
#define INTERRUPT_FLAG_PIN11 encoderDir2
#define INTERRUPT_FLAG_PIN14 encoderEsq1
#define INTERRUPT_FLAG_PIN15 encoderEsq2

#define ENCODER_MAX 13

#endif // PLACA_H_INCLUDED
