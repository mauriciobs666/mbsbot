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

#ifndef DADOS_H_INCLUDED
#define DADOS_H_INCLUDED

#include "protocolo.h"
#include "placa.h"
#include "matematica.h"

// ******************************************************************************
//		EEPROM - persistencia via CMD_GRAVA / CMD_CARREGA / CMD_DEFAULT
// ******************************************************************************

// estruturas de configuracao e parametros

typedef struct
{
    enum eTipoSensor { SENSOR_VIRTUAL, SENSOR_ANALOGICO, SENSOR_PING, SENSOR_RC, SENSOR_DIGITAL };
    char tipo;
    char pino;
    bool invertido;
    volatile unsigned short minimo, maximo, centro;
    volatile bool autoMinMax;

    void init(  eTipoSensor tipo_ = SENSOR_VIRTUAL,
                char pino_ = -1,
                bool invertido_ = false )
    {
        tipo = tipo_;
        pino = pino_;
        invertido = invertido_;

        if(SENSOR_RC == tipo)
        {
            minimo = 1200;
            centro = 1500;
            maximo = 1800;
        }
        else if(SENSOR_ANALOGICO == tipo)
        {
            minimo = 0;
            centro = 512;
            maximo = 1023;
        }
        else
        {
            minimo = 0;
            centro = 32767;
            maximo = 65535;
        }
        autoMinMax = false;
    }

    bool calibrado( int threshold = THRESHOLD_CAL )
    {
        return ( (maximo - minimo ) > threshold );
    }

    void print()
    {
        SERIALX.print("{");

        switch( (enum eTipoSensor) tipo )
        {
        case SENSOR_VIRTUAL:
            SERIALX.print("V");
            break;
        case SENSOR_ANALOGICO:
            SERIALX.print("A");
            break;
        case SENSOR_PING:
            SERIALX.print("A");
            break;
        case SENSOR_RC:
            SERIALX.print("A");
            break;
        case SENSOR_DIGITAL:
            SERIALX.print("A");
            break;
        default:
            SERIALX.print("?");
            break;
        }
        SERIALX.print(",");
        SERIALX.print((int)pino);
        if( invertido )
            SERIALX.print("! ");
        SERIALX.print("[");
        SERIALX.print(minimo);
        SERIALX.print(",");
        SERIALX.print(maximo);
        SERIALX.print("] C");
        SERIALX.print(centro);
        SERIALX.print(",");
        SERIALX.print( autoMinMax ? 'A' : 'F' );
        SERIALX.print(",");
        SERIALX.print( calibrado() ? 'C' : 'D' );
        SERIALX.print("}");
    }
}
ConfigSensor;

typedef struct
{
    enum eTipoGamepad { TIPO_RC, TIPO_PC, TIPO_WII };
    char tipo;
    ConfigSensor X, Y, Z, R;

    void init(  eTipoGamepad t,
                char pinoX = -1,
                char pinoY = -1,
                char pinoZ = -1,
                char pinoR = -1 )
    {
        tipo = t;

        if( tipo == TIPO_RC )
        {
            X.init( ConfigSensor::SENSOR_RC, pinoX );
            Y.init( ConfigSensor::SENSOR_RC, pinoY );
            Z.init( ConfigSensor::SENSOR_RC, pinoZ );
            R.init( ConfigSensor::SENSOR_RC, pinoR );
        }
        else
        {
            X.init( ConfigSensor::SENSOR_VIRTUAL );
            Y.init( ConfigSensor::SENSOR_VIRTUAL );
            Z.init( ConfigSensor::SENSOR_VIRTUAL );
            R.init( ConfigSensor::SENSOR_VIRTUAL );
        }
    }
}
ConfigGamepad;

typedef struct
{
    Fixo Kp;
    Fixo Ki;
    Fixo Kd;
    int minMV;
    int maxMV;
    int sampleTime;
    int zeraAcc;  // zera accumulador quando abs(erro) < zeraAcc
    bool dEntrada; // deriva entrada(true) ou erro(false)?
}
ConfigPID;

typedef struct
{
    char pino;
    bool invertido;
    char pinoDir;
    char pinoDirN;
    int deadband;   // pwm a partir do qual o motor comeca a se mover
    int aceleracao; // variacao abs de potencia aplicada => dp / eeprom.delays.motores
    char encoderA;
    char encoderB;

    ConfigPID pid;
}
ConfigMotor;

class Eeprom
{
public:
    struct sConfiguracao
    {
        char programa;  // enum Programas
        char balanco;   // % balanco motor esq / dir
        char velMax;    // trunca motores em +/- velMax %
        char velEscala; // tb %, multiplica
        char handBrake;

        ConfigMotor motorEsq;
        ConfigMotor motorDir;

        #ifdef RODAS_PWM_x4
            ConfigMotor motorEsqT;
            ConfigMotor motorDirT;
        #endif

        struct sDelays  // duracao (ms) de movimentos pra animacao
        {
            int ES; // intervalo de entrada/saida, leitura de sensores etc
            int debounce; // debounce de cruzamento / marcaEsq / marcaDir
        } delays;

        ConfigPID pid[PID_N];

        ConfigGamepad joyPC;

        #ifdef PINO_JOY_X
            ConfigGamepad joyRC;
        #endif

        ConfigSensor sensores[NUM_SENSORES];
    }
    dados;

    void load()
    {
        char * dest = (char*) &dados;
        for( unsigned int addr = 0; addr < sizeof(dados); addr++, dest++ )
            *dest = eeprom_read_byte(( unsigned char * ) addr );
    }

    void save()
    {
        char * dest = (char*) &dados;
        for( unsigned int addr = 0; addr < sizeof(dados); addr++, dest++ )
            eeprom_write_byte(( unsigned char  *) addr, *dest);
    }

    void defaultPidMotor( ConfigPID* pid )
    {
        pid->Kp.setFloat( PID_MOTOR_P );
        pid->Ki.setFloat( PID_MOTOR_I );
        pid->Kd.setFloat( PID_MOTOR_D );
        pid->maxMV      = PID_MOTOR_MAX_MV;
        pid->minMV      = PID_MOTOR_MIN_MV;
        pid->zeraAcc    = PID_MOTOR_ZACC;
        pid->dEntrada   = PID_MOTOR_DENTRADA;
        pid->sampleTime = DFT_DELAY_MOTOR;
    }

    void defaults()
    {
        dados.programa  = DFT_PROGRAMA;
        dados.handBrake = DFT_FREIO_MAO;
        dados.velMax    = DFT_VEL_MAX;
        dados.velEscala = DFT_VEL_ESCALA;
        dados.balanco   = DFT_BALANCO;

        dados.motorEsq.pino         = PINO_MOTOR_ESQ_PWM;
        dados.motorEsq.pinoDir      = PINO_MOTOR_ESQ;
        dados.motorEsq.pinoDirN     = PINO_MOTOR_ESQ_N;
        dados.motorEsq.invertido    = MOTOR_ESQ_INV;
        dados.motorEsq.encoderA     = PINO_MOTOR_ESQ_ENC_A;
        dados.motorEsq.encoderB     = PINO_MOTOR_ESQ_ENC_B;
        dados.motorEsq.aceleracao   = MOTOR_ACEL;
        dados.motorEsq.deadband     = MOTOR_CENTRO;

        dados.motorDir.pino         = PINO_MOTOR_DIR_PWM;
        dados.motorDir.pinoDir      = PINO_MOTOR_DIR;
        dados.motorDir.pinoDirN     = PINO_MOTOR_DIR_N;
        dados.motorDir.invertido    = MOTOR_DIR_INV;
        dados.motorDir.encoderA     = PINO_MOTOR_DIR_ENC_A;
        dados.motorDir.encoderB     = PINO_MOTOR_DIR_ENC_B;
        dados.motorDir.aceleracao   = MOTOR_ACEL;
        dados.motorDir.deadband     = MOTOR_CENTRO;

        defaultPidMotor( &dados.motorEsq.pid );
        defaultPidMotor( &dados.motorDir.pid );

        #ifdef RODAS_PWM_x4
            dados.motorEsqT.pino         = PINO_MOTOR_ESQ_T_PWM;
            dados.motorEsqT.pinoDir      = PINO_MOTOR_ESQ_T;
            dados.motorEsqT.pinoDirN     = PINO_MOTOR_ESQ_T_N;
            dados.motorEsqT.invertido    = MOTOR_E_T_INV;
            dados.motorEsqT.encoderA     = PINO_MOTOR_ESQ_T_ENC_A;
            dados.motorEsqT.encoderB     = PINO_MOTOR_ESQ_T_ENC_B;
            dados.motorEsqT.aceleracao   = MOTOR_ACEL;
            dados.motorEsqT.deadband     = MOTOR_CENTRO;

            dados.motorDirT.pino         = PINO_MOTOR_DIR_T_PWM;
            dados.motorDirT.pinoDir      = PINO_MOTOR_DIR_T;
            dados.motorDirT.pinoDirN     = PINO_MOTOR_DIR_T_N;
            dados.motorDirT.invertido    = MOTOR_D_T_INV;
            dados.motorDirT.encoderA     = PINO_MOTOR_DIR_T_ENC_A;
            dados.motorDirT.encoderB     = PINO_MOTOR_DIR_T_ENC_B;
            dados.motorDirT.aceleracao   = MOTOR_ACEL;
            dados.motorDirT.deadband     = MOTOR_CENTRO;

            defaultPidMotor( &dados.motorEsqT.pid );
            defaultPidMotor( &dados.motorDirT.pid );
        #endif

        dados.delays.ES = DFT_DELAY_ES;
        dados.delays.debounce = DFT_LF_DEBOUNCE;

        dados.pid[ PID_CALIBRA ].Kp.setFloat( CAL_PID_P );
        dados.pid[ PID_CALIBRA ].Ki.setFloat( CAL_PID_I );
        dados.pid[ PID_CALIBRA ].Kd.setFloat( CAL_PID_D );
        dados.pid[ PID_CALIBRA ].maxMV      = CAL_PID_MAX_MV;
        dados.pid[ PID_CALIBRA ].minMV      = CAL_PID_MIN_MV;
        dados.pid[ PID_CALIBRA ].zeraAcc    = CAL_PID_ZACC;
        dados.pid[ PID_CALIBRA ].dEntrada   = CAL_PID_DENTRADA;
        dados.pid[ PID_CALIBRA ].sampleTime = CAL_PID_SAMPLE;

        dados.pid[ PID_CORRIDA ].Kp.setFloat( DFT_PID_P );
        dados.pid[ PID_CORRIDA ].Ki.setFloat( DFT_PID_I );
        dados.pid[ PID_CORRIDA ].Kd.setFloat( DFT_PID_D );
        dados.pid[ PID_CORRIDA ].maxMV      = DFT_PID_MAX_MV;
        dados.pid[ PID_CORRIDA ].minMV      = DFT_PID_MIN_MV;
        dados.pid[ PID_CORRIDA ].zeraAcc    = DFT_PID_ZACC;
        dados.pid[ PID_CORRIDA ].dEntrada   = DFT_PID_DENTRADA;
        dados.pid[ PID_CORRIDA ].sampleTime = DFT_PID_SAMPLE;

        dados.pid[ PID_RETA ].Kp.setFloat( DFT_PID_RETA_P );
        dados.pid[ PID_RETA ].Ki.setFloat( DFT_PID_RETA_I );
        dados.pid[ PID_RETA ].Kd.setFloat( DFT_PID_RETA_D );
        dados.pid[ PID_RETA ].maxMV      = DFT_PID_RETA_MAX_MV;
        dados.pid[ PID_RETA ].minMV      = DFT_PID_RETA_MIN_MV;
        dados.pid[ PID_RETA ].zeraAcc    = DFT_PID_RETA_ZACC;
        dados.pid[ PID_RETA ].dEntrada   = DFT_PID_RETA_DENTRADA;
        dados.pid[ PID_RETA ].sampleTime = DFT_PID_RETA_SAMPLE;

        // TODO (mbs#1#): permitir config dos sensores via serial

#ifndef SEM_LINE_FOLLOWER
        for( int p = LF_PINO_0;
                 p < ( LF_PINO_0 +LF_NUM_SENSORES );
                 p++ )
        {
            dados.sensores[p].init( ConfigSensor::SENSOR_ANALOGICO, p );
        }
#endif

#if VERSAO_PLACA == 22
        dados.sensores[0].init( ConfigSensor::SENSOR_ANALOGICO, 0 ); // bateria

        dados.sensores[1].init( ConfigSensor::SENSOR_PING, 15 );
        dados.sensores[1].minimo = 1000;
        dados.sensores[1].centro = 4000;
        dados.sensores[1].maximo = 4000;

        dados.sensores[2].init( ConfigSensor::SENSOR_PING, 16 );
        dados.sensores[2].minimo = 1000;
        dados.sensores[2].centro = 4000;
        dados.sensores[2].maximo = 4000;
#endif

#if VERSAO_PLACA == 2
        dados.sensores[1].init( ConfigSensor::SENSOR_ANALOGICO, 1, true );
        dados.sensores[1].minimo = 100;
        dados.sensores[1].maximo = 630;

        dados.sensores[2].init( ConfigSensor::SENSOR_ANALOGICO, 2, true );
        dados.sensores[2].minimo = 100;
        dados.sensores[2].maximo = 630;

        dados.sensores[3].init( ConfigSensor::SENSOR_ANALOGICO, 3, true );
        dados.sensores[3].minimo = 100;
        dados.sensores[3].maximo = 630;
#endif

        dados.joyPC.init( ConfigGamepad::TIPO_PC );

        #ifdef PINO_JOY_X
            #ifdef PINO_JOY_Y
                dados.joyRC.init( ConfigGamepad::TIPO_RC, PINO_JOY_X, PINO_JOY_Y );
                #ifdef PINO_JOY_Z
                    dados.joyRC.Z.init( ConfigSensor::SENSOR_RC, PINO_JOY_Z );
                #endif
                #ifdef PINO_JOY_R
                    dados.joyRC.R.init( ConfigSensor::SENSOR_RC, PINO_JOY_R );
                #endif
            #endif
        #endif
    }
};

void uname()
{
    SERIALX.print( "MBSBOT hw " );
    SERIALX.print( VERSAO_PLACA );
    SERIALX.print( " sw " );
    SERIALX.print( VERSAO_PROTOCOLO );
    SERIALX.print( " e2 " );
    SERIALX.print( sizeof( Eeprom::dados ) );
    SERIALX.println( "b" );
}

#endif // DADOS_H_INCLUDED
