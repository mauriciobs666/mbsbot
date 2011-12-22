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

#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

#define PROTOCOL_VERSION 1

/*
O protocolo do MBS-BOT imita uma sessao de console e consiste em mnemonicos numa string
ascii com quebra de linha como terminador.

Este arquivo eh compilado na IHM e no robo e define os mnemonicos usados pelo protocolo.

Formato geral do pacote:

<comando> [argumentos] CMD_EOL
*/

#define CMD_EOL '\n'
#define MAX_CMD 50 /* bytes, tamanho maximo de um comando */

/*
   Lista de <comando>
*/

#define CMD_READ	"get"
/* CMD_READ <variavel> */

#define CMD_WRITE	"set"
/* CMD_WRITE <variavel> = [int] */

#define CMD_SAVE	"sv"
/* CMD_SAVE : grava sConfigurationData na eeprom */

#define CMD_LOAD	"ld"
/* CMD_LOAD : descarta alteracoes e recarrega sConfigurationData da eeprom */

#define CMD_DEFAULT	"dft"
/* CMD_DEFAULT : carrega sConfigurationData com valores default */

#define CMD_LF_CAL	"cal"
/* CMD_LF_CAL : auto calibra sensores do line-follower */

#define CMD_MV_INCH	"pol"
/* CMD_MV_INCH : anda uma polegada pra frente (ajuste via VAR_T_INCH) */

#define CMD_MV_STOP "stop"
/* CMD_MV_STOP : para imediatamente */

#define CMD_MV_WHEELS "drv"
/* CMD_MV_WHEELS esquerda direita [duracao em ms]
        esquerda, direita: +/- 0-100%
*/

#define CMD_MV_VECT "vect"
/* CMD_MV_VECT eixo_X eixo_Y [duracao em ms]
    eixo_X, eixo_Y: +/- 0-100 %
*/

#define CMD_TURN_LEFT "left"
/* CMD_TURN_LEFT : 90 graus pra esquerda */

#define CMD_TURN_RIGHT "right"
/* CMD_TURN_RIGHT : 90 graus pra direita */

#define CMD_STATUS "status"
/* CMD_STATUS : pede status
formato da resposta:
S VAR_PROGRAMA ERRO VAR_RODA_ESQ VAR_RODA_DIR VAR_SERVO_X VAR_SERVO_Y VAR_SERVO_Z
*/

#define CMD_UNAME "uname"
/* CMD_UNAME */

#define CMD_BIP "bip"
/* CMD_BIP [Hz] [ms] */

#define CMD_CLEAR_ERR "clerr"

/*
   Lista de <variavel>
*/
#define VAR_PROGRAMA "p"    // enum ProgramID sendo executado

#define VAR_AS       "as"	// todas entradas analogicas
#define VAR_PID      "pid"  // coeficientes do algoritmo de PID
#define VAR_FREIO    "hb"   // handbrake, freio de mao

#define VAR_RODA_ESQ "l"    // motor da roda esquerda
#define VAR_RODA_DIR "r"    // motor da roda direita

#define VAR_ZERO_ESQ "lc"	// offset da posicao parada ("centro") da roda esquerda (pot do servo mal calibrado)
#define VAR_ZERO_DIR "rc"	// offset da posicao parada da roda direita (pot do servo mal calibrado)

#define VAR_ACEL_ESQ "la"   // aceleracao do motor da roda esquerda
#define VAR_ACEL_DIR "ra"   // aceleracao do motor da roda direita

#define VAR_SERVO_X  "sx"   // servo "X" (pan)
#define VAR_SERVO_Y  "sy"   // servo "Y" (tilt)
#define VAR_SERVO_Z  "sz"   // servo "Z" (roll)

#define VAR_T_POL    "di"   // delay do drive.inch(), anda 1 polegada pra frente
#define VAR_T_90     "d90"	// delay do drive.left() e drive.right(), vira 90 graus
#define VAR_T_RF     "drf"  // tempo minimo entre leituras do dispositivo de range finder (ou sonar)

enum ProgramID
{
    PRG_RC = 0,	            // Controle remoto
    PRG_SHOW_SENSORS = 1,   // Controle remoto com monitoramento das analogicas
    PRG_PHOTOVORE = 2,      //
    PRG_LINEFOLLOWER = 3,   //
    PRG_SHARP = 4,          // Sharp IR range finder
    PRG_SHARP_CHASE = 5,    //
    PRG_COLLISION = 6,      //
    PRG_SENTRY = 7,         //
    PRG_WIICHUCK = 8,       //
    PRG_SCOPE = 9,          //
    PRG_KNOB = 10,          // Movimenta o servo X de acordo com A0
    PRG_TEST = 11           // Dummy
};

enum Errors
{
    SUCCESS = 0,
    ERR_MAX_CMD_SIZ,
    ERR_INVALID_PRG
};

#endif // PROTOCOL_H_INCLUDED
