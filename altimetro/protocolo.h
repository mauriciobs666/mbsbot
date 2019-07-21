
// (c) 2017-2019 MBS - Mauricio Bieze Stefani

#ifndef PROTOCOLO_H_INCLUDED
#define PROTOCOLO_H_INCLUDED

#define VERSAO_PROTOCOLO 2

/* O protocolo
 do MBS-BOT imita uma sessao de console e consiste em mnemonicos numa string
ascii com quebra de linha como terminador.

Este arquivo eh compilado na IHM e no robo e define os mnemonicos usados pelo protocolo.

Formato geral do pacote:

<comando> [argumentos] CMD_EOL
*/

#define CMD_EOL       '\n'
#define MAX_CMD 20      // bytes, tamanho maximo de um comando serial

#define TAM_TOKEN 10    // bytes, tamanho max de um token
#define TAM_NOME 5      // bytes, tamanho max de um nome de variavel
#define NUM_VARS 10     // array de variaveis do interpretador

/* Lista de <comandos>
*/

#define CMD_GRAVA	"sv"
/* CMD_GRAVA : grava sConfigurationData na eeprom */

#define CMD_CARREGA	"ld"
/* CMD_CARREGA : descarta alteracoes e recarrega sConfigurationData da eeprom */

#define CMD_DEFAULT	"dft"
/* CMD_DEFAULT : carrega sConfigurationData com valores default */

#define CMD_STATUS "s"
/* CMD_STATUS : pede status
formato da resposta:
S NOME_PROGRAMA ERRO FREIO NOME_RODA_ESQ NOME_RODA_DIR NOME_RODA_ESQ_T NOME_RODA_DIR_T NOME_SERVO_X NOME_SERVO_Y NOME_SERVO_Z
*/

#define CMD_UNAME   "uname"
/* CMD_UNAME */

#define CMD_BIP     "bip"
/* CMD_BIP [Hz] [ms] */

#define CMD_DUMP    "dmp"
#define CMD_CLEAR   "clr"
#define CMD_RESET   "rst"

#define CMD_WHO     "who" /* mostra lista de variaveis */

#define CMD_AVISOS  "avs"

// cartao de memoria

#define CMD_LST     "ls"
#define CMD_REC     "rec"
#define CMD_STOP    "stp"

#define NOME_TRACE      "trc"
#define NOME_T_TRC      "trcd" // intervalo envio trace
#define NOME_ALPHA      "alf"
#define NOME_BETA       "bet"

#define NOME_LOOPS      "lps"
#define NOME_BAT        "bat"
#define NOME_BAT_OFF    "off"
#define NOME_BAT_SLP    "slp"

#define NOME_ERRNO      "err"
#define NOME_TIMESTAMP  "t"
#define NOME_ESTADO     "std"

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
    SKIP
};

// mascara bitmap
#define TRACE_MASTER_EN             0x0001  //    1
#define TRACE_SENSOR                0x0002  //    2
#define TRACE_ALTURA                0x0004  //    4
#define TRACE_VELOCIDADE            0x0008  //    8
#define TRACE_ALTITUDE              0x0010  //   16
#define TRACE_AVISOS                0x0020  //   32
#define TRACE_ANDROID_PLOTTER       0x0040  //   64
#define TRACE_BATERIA               0x0080  //  128
#define TRACE_TEMPERATURA           0x0100  //  256

/*
    master     = 1
    sensor     = 2
    altura     = 4
    velocidade = 8
    altitude   = 16
    avisos     = 32
    android    = 64
    bateria    = 128
*/


#define ESTADO_DZ           0x00
#define ESTADO_SUBIDA       0x01
#define ESTADO_QUEDA        0x02
#define ESTADO_NAVEGACAO    0x04

#endif // PROTOCOLO_H_INCLUDED
