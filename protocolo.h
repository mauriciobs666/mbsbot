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

#ifndef PROTOCOLO_H_INCLUDED
#define PROTOCOLO_H_INCLUDED

#define VERSAO_PROTOCOLO 1

/* O protocolo
 do MBS-BOT imita uma sessao de console e consiste em mnemonicos numa string
ascii com quebra de linha como terminador.

Este arquivo eh compilado na IHM e no robo e define os mnemonicos usados pelo protocolo.

Formato geral do pacote:

<comando> [argumentos] CMD_EOL
*/

#define CMD_EOL ';'
#define MAX_CMD 50      // bytes, tamanho maximo de um comando serial

#define TAM_TOKEN 10    // bytes, tamanho max de um token
#define TAM_NOME 5      // bytes, tamanho max de um nome de variavel
#define NUM_VARS 66     // array de variaveis do interpretador

/* Lista de <comandos>
*/

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

#define CMD_LF "lf"
/* CMD_LF : inicia corrida */

#define CMD_MV_PARAR "x"
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

#define CMD_JOYPAD "j"
/* CMD_JOYPAD BOTOES X Y Z R */

#define CMD_WHO "who"
/* mostra lista de variaveis */

/* Lista de <variaveis> */
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
#define NOME_T_DEB    "deb"  // debounce do linefollower pra cruzamentos e marcas especiais

// Parametros pra uso durante calibragem (giraPID)
#define NOME_PID_CAL_KP   "kpc"   // coeficientes do algoritmo de PID
#define NOME_PID_CAL_KI   "kic"   //  "
#define NOME_PID_CAL_KD   "kdc"   //  "
#define NOME_PID_CAL_MVX  "mvxc"  // valor maximo MV
#define NOME_PID_CAL_MVN  "mvnc"  // valor minimo MV
#define NOME_PID_CAL_ZAC  "zacc"  // zera acumulador quando erro for 0

// LF seguro pra curvas
#define NOME_PID_KP    "kp"   // coeficientes do algoritmo de PID
#define NOME_PID_KI    "ki"   //  "
#define NOME_PID_KD    "kd"   //  "
#define NOME_PID_MVX   "mvx"  // valor maximo MV
#define NOME_PID_MVN   "mvn"  // valor minimo MV
#define NOME_PID_ZAC   "zac"  // zera acumulador quando erro for 0

// LF otimizado pra retas (intervencao minima)
#define NOME_PID_RETA_KP    "kpr"   // coeficientes do algoritmo de PID
#define NOME_PID_RETA_KI    "kir"   //  "
#define NOME_PID_RETA_KD    "kdr"   //  "
#define NOME_PID_RETA_MVX   "mvxr"  // valor maximo MV
#define NOME_PID_RETA_MVN   "mvnr"  // valor minimo MV
#define NOME_PID_RETA_ZAC   "zacr"  // zera acumulador quando erro for 0

// motor esquerdo
#define NOME_PID_ME_KP    "kpe"   // coeficientes do algoritmo de PID
#define NOME_PID_ME_KI    "kie"   //  "
#define NOME_PID_ME_KD    "kde"   //  "
#define NOME_PID_ME_ZAC   "zace"  // zera acumulador quando erro for 0

// motor direito
#define NOME_PID_MD_KP    "kpd"   // coeficientes do algoritmo de PID
#define NOME_PID_MD_KI    "kid"   //  "
#define NOME_PID_MD_KD    "kdd"   //  "
#define NOME_PID_MD_ZAC   "zacd"  // zera acumulador quando erro for 0

#define NOME_VEL_MAX     "vm" // limite de velocidade em %
#define NOME_VEL_ESCALA  "ve" // escala a ser aplicada nas velocidades
#define NOME_BALANCO     "bal" // balanco rodas esq/dir

#define NOME_TRACE       "trc"
#define NOME_TRACE_MSK   "trcm"
#define NOME_T_TRC       "trcd" // intervalo envio trace

#define NOME_ERRNO       "err"

#define NOME_TIMESTAMP   "t"

#define TAM_PILHA_PRG

enum Programas
{
    PRG_IDLE = 0,	        //
    PRG_RC_SERIAL = 1,      // Controle remoto via porta serial / gamepad / hobby rc
    PRG_FOTOVORO = 2,       //
    PRG_LINE_FOLLOW = 3,    //
    PRG_SCANNER = 4,        // Obsoleto: scanner com rangefinder e servo
    PRG_CHASE = 5,          // Persegue um alvo
    PRG_COLISAO = 6,        // Detecta tempo para colisao em s
    PRG_SENTINELA = 7,      //
    PRG_WIICHUCK = 8,       //
    PRG_SCOPE = 9,          //
    PRG_KNOB = 10,          // Movimenta o servo X de acordo com A0
    PRG_NAV_3S = 11,        // Auto navegacao usando 1 sonar + 2 IR (M2D2)
    PRG_ALARME = 12,
    PRG_LF_CAL = 13
};

enum Erros
{
    SUCESSO = 0,
    TODO,
    ERRO_TAM_MAX_CMD,
    ERRO_PRG_INVALIDO,
    ERRO_VAR_INVALIDA,
    ERRO_VAR_EXISTE,
    ERRO_VAR_ARRAY,
    ERRO_INTERPRETADOR,
    ERRO_LF_CALIBRA,
    ERRO_LF_TRILHO,
    ERRO_LF_TIMEOUT,
    SKIP
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
    PID_N
};

enum Prioridades
{
    PRI_AUTOPRESERVACAO = 0,
    PRI_SERIAL,
    PRI_RC,
    PRI_LINE_FOLLOW,
    PRI_IDLE
};

// mascara de trace
#define TRC_STATUS   0x01
#define TRC_SENSORES 0x02
#define TRC_LF       0x04
#define TRC_JOYSTICK 0x08
#define TRC_PID      0x10

#endif // PROTOCOLO_H_INCLUDED
