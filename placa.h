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

#define VERSAO_PLACA 3

// Habilita controle PWM (se undef controla por largura de pulso/servo)
#define RODAS_PWM 1

// 4 rodas mechanum
#define RODAS_PWM_x4 1

// inverte direcao de giro dos motores
#define MOTOR_ESQ_INV 1
#define MOTOR_DIR_INV 1
#define MOTOR_E_T_INV 0
#define MOTOR_D_T_INV 0

// canal 1 - roda esquerda traseira
#define PINO_MOTOR_ESQ_T        23
#define PINO_MOTOR_ESQ_T_PWM    5
// canal 2 - roda direita traseira
#define PINO_MOTOR_DIR_T        24
#define PINO_MOTOR_DIR_T_PWM    4
// canal 3 - roda esquerda dianteira
#define PINO_MOTOR_ESQ          25
#define PINO_MOTOR_ESQ_PWM      3
// canal 4 - roda direita dianteira
#define PINO_MOTOR_DIR          26
#define PINO_MOTOR_DIR_PWM      2

// economia de energia
#define PINO_UNUSED_CNT 0
#define PINO_UNUSED_ARRAY { }

//#define PINO_SERVO_PAN		9
//#define PINO_SERVO_TILT		10
//#define PINO_SERVO_ROLL		8

// LINE FOLLOWER (pinos analogicos!)
#define NUM_IR_TRACK		5
#define PINO_FIRST_IR_SENSOR 1
// onde NUM_IR_TRACK=3 e PINO_FIRST_IR_SENSOR=2 significa que os pinos A2, A3 e A4 estao conectados

#define PINO_BIP 			12

#define PINO_ARMA           13

#define PINO_LED            13

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

// RADIO CONTROLE
#define PINO_JOY_X      18
#define PINO_JOY_Y      19
#define PINO_JOY_Z      20
//#define PINO_JOY_R
#define PINO_JOY_SW1    21

// int de hardware externa (so 0 e 1 no 328 e 2, 3, 4 e 5 no 1280)
#define INT_JOY_X      5
#define INT_JOY_Y      4
#define INT_JOY_Z      3
//#define INT_JOY_R
#define INT_JOY_SW1    2

#endif // BOARD_H_INCLUDED
