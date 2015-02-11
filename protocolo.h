/**	Copyright (C) 2010-2013 - Mauricio Bieze Stefani
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

#define VERSAO_PROTOCOLO 1

/* O protocolo
 do MBS-BOT imita uma sessao de console e consiste em mnemonicos numa string
ascii com quebra de linha como terminador.

Este arquivo eh compilado na IHM e no robo e define os mnemonicos usados pelo protocolo.

Formato geral do pacote:

<comando> [argumentos] CMD_EOL
*/

#define CMD_EOL '\n'
#define MAX_CMD 50      // bytes, tamanho maximo de um comando serial

#define TAM_TOKEN 10    // bytes, tamanho max de um token
#define TAM_NOME 5      // bytes, tamanho max de um nome de variavel
#define NUM_VARS 25     // array de variaveis do interpretador

/* Lista de <comandos>
*/

#define CMD_GET	"get"
/* CMD_GET <variavel> */

#define CMD_SET	"set"
/* CMD_SET <variavel> = [int] */

#define CMD_GRAVA	"sv"
/* CMD_GRAVA : grava sConfigurationData na eeprom */

#define CMD_CARREGA	"ld"
/* CMD_CARREGA : descarta alteracoes e recarrega sConfigurationData da eeprom */

#define CMD_DEFAULT	"dft"
/* CMD_DEFAULT : carrega sConfigurationData com valores default */

#define CMD_CAL	"cal"
/* CMD_CAL : auto calibra sensores joystick */

#define CMD_CENTRO "cen"
/* CMD_CENTRO : encerra auto calibracao e centra joystick */

#define CMD_LF_CAL	"lfcal"
/* CMD_LF_CAL : auto calibra sensores do line-follower */

#define CMD_MV_PARAR "p"
/* CMD_MV_PARAR : para imediatamente */

#define CMD_MV_RODAS "drv"
/* CMD_MV_RODAS esquerda direita
        esquerda, direita: +/- 0-100%
*/

#define CMD_MV_VET "vet"
/* CMD_MV_VET eixo_X eixo_Y [duracao em ms]
    eixo_X, eixo_Y: +/- 0-100 %
*/

#define CMD_STATUS "s"
/* CMD_STATUS : pede status
formato da resposta:
S NOME_PROGRAMA ERRO FREIO NOME_RODA_ESQ NOME_RODA_DIR NOME_RODA_ESQ_T NOME_RODA_DIR_T NOME_SERVO_X NOME_SERVO_Y NOME_SERVO_Z
*/

#define CMD_UNAME "uname"
/* CMD_UNAME */

#define CMD_BIP "bip"
/* CMD_BIP [Hz] [ms] */

#define CMD_LIMPA_ERRO "clerr"

#define CMD_JOYPAD "j"
/* CMD_JOYPAD BOTOES X Y Z R */

/* Lista de <variaveis>
*/
#define NOME_PROGRAMA "p"    // enum Programas sendo executado

#define NOME_AS       "as"   // todas entradas analogicas
#define NOME_FREIO    "fm"   // freio de mao

#define NOME_RODA_ESQ "me"    // motor da roda esquerda
#define NOME_RODA_DIR "md"    // motor da roda direita

#define NOME_ZERO_ESQ "mec"
#define NOME_ZERO_DIR "mdc"

#define NOME_ZERO_ESQ_T "metc"
#define NOME_ZERO_DIR_T "mdtc"

#define NOME_ACEL_ESQ "mea"   // aceleracao do motor da roda esquerda
#define NOME_ACEL_DIR "mda"   // aceleracao do motor da roda direita

#define NOME_SERVO_X  "sx"   // servo "X" (pan)
#define NOME_SERVO_Y  "sy"   // servo "Y" (tilt)
#define NOME_SERVO_Z  "sz"   // servo "Z" (roll)

#define NOME_T_RF     "drf"  // tempo minimo entre leituras do dispositivo de range finder (ou sonar)
#define NOME_T_SE     "dse"  // tempo minimo entre envio de SEnsores
#define NOME_T_ST     "dst"  // tempo minimo entre envio de STatus
#define NOME_T_MOTOR  "dm"   // intervalo de refresh dos motores
#define NOME_T_DEB    "deb"  // debounce do linefollower pra cruzamentos e marcas especiais

#define NOME_PID_KP    "kp"   // coeficientes do algoritmo de PID
#define NOME_PID_KI    "ki"   //  "
#define NOME_PID_KD    "kd"   //  "
#define NOME_PID_LIM_P "lmp"  // limite proporcional
#define NOME_PID_LIM_I "lmi"  // limite acumulador
#define NOME_PID_LIM_D "lmd"  // limite derivada
#define NOME_PID_MMV   "mmv"  // valor maximo MV (100-200)
#define NOME_PID_MDT   "mdt"  // delta T maximo pra calculo do componente D
#define NOME_PID_ZAC   "zac"  // zera acumulador

#define NOME_VEL_MAX     "vm" // limite de velocidade em %
#define NOME_VEL_ESCALA  "ve" // escala a ser aplicada nas velocidades
#define NOME_BALANCO     "bal" // balanco rodas esq/dir

#define NOME_TRACE       "trc"

enum Programas
{
    PRG_RC = 0,	            // Controle remoto
    PRG_RC_SERIAL = 1,      // Controle remoto via porta serial
    PRG_FOTOVORO = 2,       //
    PRG_LINE_FOLLOW = 3,    //
    PRG_SCANNER = 4,        // Scanner com rangefinder
    PRG_CHASE = 5,          //
    PRG_COLISAO = 6,        //
    PRG_SENTINELA = 7,      //
    PRG_WIICHUCK = 8,       //
    PRG_SCOPE = 9,          //
    PRG_KNOB = 10,          // Movimenta o servo X de acordo com A0
    PRG_NAV_3S = 11,        // Auto navegacao usando 1 sonar + 2 IR (M2D2)
    PRG_ALARME = 12
};

enum Erros
{
    SUCESSO = 0,
    ERRO_TAM_MAX_CMD,
    ERRO_PRG_INVALIDO,
    ERRO_VAR_INVALIDA,
    ERRO_INTERPRETADOR,
    ERRO_LF_CALIBRA,
    ERRO_LF_TRILHO
};

// Mapeamento dos botoes de Joypad (API antiga)
/*
#define BT_X   0x001 //    1 - X ou quadrado
#define BT_A   0x002 //    2 - A ou xis
#define BT_B   0x004 //    4 - B ou bola
#define BT_Y   0x008 //    8 - Y ou triangulo
#define BT_LB  0x010 //   16 - LB ou L1
#define BT_RB  0x020 //   32 - RB ou R1
#define BT_LT  0x040 //   64 - LT ou L2
#define BT_RT  0x080 //  128 - RT ou R2
#define BT_SEL 0x100 //  256 - Select
#define BT_STR 0x200 //  512 - Start
#define BT_L3  0x400 // 1024 - L3
#define BT_R3  0x800 // 2048 - R3
*/

// Mapeamento dos botoes de Joypad (API nova)

#define BT_A   0x001 //   1 - A ou xis
#define BT_B   0x002 //   2 - B ou bola
#define BT_X   0x004 //   4 - X ou quadrado
#define BT_Y   0x008 //   8 - Y ou triangulo
#define BT_LB  0x010 //  16 - LB ou L1
#define BT_RB  0x020 //  32 - RB ou R1
#define BT_SEL 0x040 //  64 - Select
#define BT_STR 0x080 // 128 - Start
#define BT_L3  0x100 // 256 - L3
#define BT_R3  0x200 // 512 - R3


enum Pids
{
    PID_CALIBRA  = 0,
    PID_CORRIDA  = 1,
    PID_RETA     = 2,
    PID_COTOVELO = 3,
    PID_N
};

#endif // PROTOCOL_H_INCLUDED
