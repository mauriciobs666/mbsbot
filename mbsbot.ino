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

/*
4/5/15 - Arduino 1.6.3 - ATMEGA1280 - placa_v4.h

Sketch uses 26,006 bytes (20%) of program storage space. Maximum is 126,976 bytes.
Global variables use 2,025 bytes (24%) of dynamic memory, leaving 6,167 bytes for local variables. Maximum is 8,192 bytes.

17/3/15 - Arduino 1.6.1 - ATMEGA328 - placa_v23.h

Sketch uses 23,524 bytes (76%) of program storage space. Maximum is 30,720 bytes.
Global variables use 1,429 bytes (69%) of dynamic memory, leaving 619 bytes for local variables. Maximum is 2,048 bytes.
*/

// ******************************************************************************
//		INCLUDEs e CONFIGURACAO
// ******************************************************************************

#include "Arduino.h"

#include "protocolo.h"
#include "placa.h"
#include "matematica.h"
#include "dados.h"

// ANSI
#include <stdio.h>
#include <ctype.h>
#include <string.h>

// AVR / Arduino
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

// https://github.com/GreyGnome/EnableInterrupt/
#include <EnableInterrupt.h>

// speaker
#ifdef PINO_BIP
#define BIP(freq, dur) tone(PINO_BIP,freq,dur)
#else
#define BIP(freq, dur)
#endif

// ******************************************************************************
//		Plin-plin
// ******************************************************************************

Eeprom eeprom;

#ifdef PINO_SERVO_PAN
Servo pan;
#endif
#ifdef PINO_SERVO_TILT
Servo tilt;
#endif
#ifdef PINO_SERVO_ROLL
Servo roll;
#endif
#ifdef SCANNER
Scanner scanner;
#endif

unsigned long agora = 0;

// timeout de envios pela serial de status e sensores
unsigned long ultimoSensores = 0;
unsigned long ultimoStatus = 0;
// intervalo de envio
int delaySensores = -1;
int delayStatus = -1;

bool trc = false;

bool resetPrg = true;

int erro = SUCESSO;

#define MAX_STR_ERRO 20            // "01234567890123456789"
const char ErroSucesso[]    PROGMEM = "SUCESSO";
const char ErroTodo[]       PROGMEM = "INFO: TODO";
const char ErroTamMaxCmd[]  PROGMEM = "ERRO: TAM_MAX_CMD";
const char ErroPrgInval[]   PROGMEM = "ERRO: PRG_INVALIDO";
const char ErroVarInval[]   PROGMEM = "ERRO: VAR_INVALIDA";
const char ErroVarExiste[]  PROGMEM = "ERRO: VAR_EXISTE";
const char ErroVarArray[]   PROGMEM = "ERRO: VAR_ARRAY";
const char ErroInpretador[] PROGMEM = "ERRO: INTERPRETADOR";
const char ErroLFCalibra[]  PROGMEM = "ERRO: LF_CALIBRA";
const char ErroLFTrilho[]   PROGMEM = "ERRO: LF_TRILHO";
const char ErroSkip[]       PROGMEM = "INFO: SKIP";

const char* const tabErros[] PROGMEM =
{
    ErroSucesso,
    ErroTodo,
    ErroTamMaxCmd,
    ErroPrgInval,
    ErroVarInval,
    ErroVarExiste,
    ErroVarArray,
    ErroInpretador,
    ErroLFCalibra,
    ErroLFTrilho,
    ErroSkip
};

// ******************************************************************************
//		PRINTERS
// ******************************************************************************

void printErro( enum Erros err, char* detalhes = NULL )
{
    char temp[MAX_STR_ERRO+1];
    strcpy_P( temp, (char*) pgm_read_word( &tabErros[err] ) );
    temp[MAX_STR_ERRO] = 0;
    SERIALX.print( temp );

    if( detalhes )
    {
        SERIALX.print( ": " );
        SERIALX.print( detalhes );
    }

    SERIALX.println();

    if( ! erro )
        erro = err;
}

void printVetor2i( Vetor2i* v )
{
    SERIALX.print( "(" );
    SERIALX.print( v->x );
    SERIALX.print( "," );
    SERIALX.print( v->y );
    SERIALX.print( ")" );
}

// ******************************************************************************
//		DELAY SEM BLOCK (nao cumulativo)
// ******************************************************************************
bool delaySemBlock(unsigned long *ultimaVez, long ms)
{
    if( ms >= 0 && agora >= ( *ultimaVez + ms ) )
    {
        *ultimaVez = agora; // += ms;
        return true;
    }
    return false;
}

// ******************************************************************************
//		SENSOR UNIVERSAL
// ******************************************************************************
class Sensor
{
public:
    volatile unsigned short valor, anterior;

    ConfigSensor *cfg;

    Sensor() : valor(0), anterior(0), cfg(NULL)
        {}
    unsigned short getValor()
        { return valor; }
    void centrar()
        { if( cfg ) cfg->centro = valor; }
    int delta()
        { return valor - anterior; }
    bool ehMinimo(unsigned short margem = 0)
        { return ( cfg->invertido ? (cfg->maximo - valor) <= margem : (valor - cfg->minimo) <= margem ); }
    bool ehMaximo(unsigned short margem = 0)
        { return ( cfg->invertido ? (valor - cfg->minimo) <= margem : (cfg->maximo - valor) <= margem ); }

    void setConfig(ConfigSensor *c)
    {
        cfg=c;
        valor = anterior = cfg->centro;
    }

    void setValor(unsigned short v)
    {

        if( cfg )
        {
            if( cfg->autoMinMax )
            {
                if (v < cfg->minimo) cfg->minimo = v;
                if (v > cfg->maximo) cfg->maximo = v;
            }
        }
        anterior = valor;
        valor = v;
    }
    unsigned short calibrar()
    {
        refresh();
        if( cfg )
        {
            cfg->minimo = cfg->maximo = cfg->centro = valor;
            cfg->autoMinMax = true;
        }
        return valor;
    }
    Sensor& refresh()
    {
        if( cfg )
        {
            switch(cfg->tipo)
            {
                case ConfigSensor::SENSOR_ANALOGICO:
                    setValor( analogRead( cfg->pino ) );
                break;
                case ConfigSensor::SENSOR_PING:
                    // manda pulso de 2ms pro ping))) pra acionar leitura
                    pinMode( cfg->pino, OUTPUT );
                    digitalWrite( cfg->pino, LOW );
                    delayMicroseconds( 2 );
                    digitalWrite( cfg->pino, HIGH );
                    delayMicroseconds( 5 );
                    digitalWrite( cfg->pino, LOW );

                    // duracao do pulso = distancia
                    pinMode( cfg->pino, INPUT );
                    setValor( pulseIn( cfg->pino, HIGH ) );
                break;
                case ConfigSensor::SENSOR_DIGITAL:
                    setValor( digitalRead( cfg->pino ) );
                break;
                default:
                break;
            }
        }
        return *this;
    }
    int getPorcento()
    {
        /*
            distancia da borda inferior ( min ou max de acordo com invertido )
            /
            range min/max
            *
            100%
        */
        return ( cfg->maximo != cfg->minimo )
                    ? cfg->invertido
                        ? constrain( (((long)(cfg->maximo - valor) * 100) / ( cfg->maximo - cfg->minimo ) ), 0, 100 )
                        : constrain( (((long)(valor - cfg->minimo) * 100) / ( cfg->maximo - cfg->minimo ) ), 0, 100 )
                    : 0;
    }
    int getPorcento( int arredonda )
    {
        int x = getPorcento();

            // arredonda no centro e pontas
        if( 100 + x < arredonda) return -100;
        if(  abs(x) < arredonda) return 0;
        if( 100 - x < arredonda) return 100;

        return x;
    }
    int getPorcentoCentro( int arredonda=10 )
    {
        long x = (long)valor - (long)cfg->centro;

        // calcula o range de 0 a min/max
        long r = ( x > 0 ) ? (cfg->maximo - cfg->centro) : (cfg->centro - cfg->minimo);

        // x%
        if(r)
            x = constrain( ((x * 100) / r ) , -100, 100 );
        else
            x = 0;

        // arredonda no centro e pontas
        if( 100 + x < arredonda) return -100;
        if(  abs(x) < arredonda) return 0;
        if( 100 - x < arredonda) return 100;

        return x;
    }
    bool getBool()
    {
        return ( getPorcento() > 50 );
    }
}
sensores[NUM_SENSORES];

class Botao
{
public:
    Botao( unsigned char pino, bool invertido = true ) : debounce(0), antes(false), estado(false), trocou(false)
    {
         init( pino, invertido );
    }
    void init( unsigned char pino, bool invertido = true )
    {
        pinMode( pino, invertido ? INPUT_PULLUP : INPUT );
        cfg.init( ConfigSensor::SENSOR_DIGITAL, pino, invertido );
        s.setConfig( &cfg );
        s.calibrar();
    }
    Botao& refresh()
    {
        s.refresh();

        bool atual = s.getBool();

        if( atual != antes )
            debounce = agora;

        if( ( agora - debounce ) > 50 )
        {
            if( estado != atual )
            {
                estado = atual;
                trocou = true;
            }
        }
        antes = atual;
        return *this;
    }
    bool getEstado()
    {
        return estado;
    }
    bool trocouEstado()
    {
        if( trocou )
        {
            trocou = false;
            return true;
        }
        return false;
    }
private:
    Sensor s;
    ConfigSensor cfg;
    unsigned long debounce;
    bool antes, estado, trocou;
};

#ifdef PINO_BOTAO_CAL
    Botao botaoCal( PINO_BOTAO_CAL );
#endif

#ifdef PINO_BOTAO_PRG
    Botao botaoPrg( PINO_BOTAO_PRG );
#endif

// ******************************************************************************
//		GAMEPAD E R/C
// ******************************************************************************
class MbsGamePad
{
public:
    char tipo;
    Sensor x, y, z, r;
    volatile unsigned int botoesAntes, botoesAgora, botoesEdgeF, botoesEdgeR;
    MbsGamePad() : botoesAntes(0), botoesAgora(0), botoesEdgeF(0), botoesEdgeR(0)
        {}
    unsigned int refreshBotoes(unsigned int novo)
    {
        botoesAntes = botoesAgora;
        botoesEdgeR = (novo ^ botoesAntes) & novo;
        botoesEdgeF = (novo ^ botoesAntes) & ~novo;
        return botoesAgora = novo;
    }
    void setConfig( ConfigGamepad *cfg )
    {
        tipo = cfg->tipo;
        x.setConfig( &cfg->X );
        y.setConfig( &cfg->Y );
        z.setConfig( &cfg->Z );
        r.setConfig( &cfg->R );
    }
    void calibrar()
    {
        x.calibrar();
        y.calibrar();
        z.calibrar();
        r.calibrar();
    }
    void centrar()
    {
        x.centrar();
        y.centrar();
        z.centrar();
        r.centrar();
    }
};
MbsGamePad gamepad;

#ifdef PINO_JOY_X
    MbsGamePad rcpad;
#endif // PINO_JOY_X


class PID
{
public:
    ConfigPID *cfg;

    int proporcional;
    int integral;
    int derivada;

    Fixo acumulador;

    int erro;
    int erroAnterior;
    int entAnterior;

    int setPoint;   // leitura de entrada desejada = meta
    int MV;         // variavel manipulada = valor de saida
    int sampleTime; // ms entre as amostras

    PID()
    {
        cfg = NULL;
        zera();
    }

    int executaSample( int entrada, bool reinicia = false )
    {
        if( ( ( agora - tUltimoLoop ) > sampleTime )
           || reinicia )
        {
            executa( entrada, reinicia );
            tUltimoLoop = agora;
        }
        return MV;
    }

    void setConfig( ConfigPID *config )
    {
        cfg = config;
    }

    void zera( )
    {
        proporcional = integral = derivada = 0;
        setPoint = MV = erro = erroAnterior = entAnterior = 0;
        tUltimoLoop = 0;
        acumulador.setInt(0);
    }

    int executa( int entrada, bool reinicia = false )
    {
        if( ! cfg )
            return 0;

//        SERIALX.print( "executa( " );
//        SERIALX.print( entrada );
//        SERIALX.println( " )" );

        erro = entrada - setPoint;

        // P
        proporcional = cfg->Kp * erro;

        // I
        if( reinicia )
        {
            // apenas mantem curso, ignora acumulador anterior
            acumulador = MV;
        }
        else if( erro > cfg->zeraAcc || erro < -cfg->zeraAcc )
        {
            Fixo temp = cfg->Ki;
            temp *= Fixo(erro);
            acumulador += temp;
            acumulador.Constrain( cfg->minMV, cfg->maxMV );
        }
        else
        {
            acumulador = 0;
        }

        integral = acumulador.getInt();

        // D
        if( reinicia )
        {
            // anterior nao eh confiavel
            derivada = 0;
        }
        else if( cfg->dEntrada )
        {
            // deriva entrada pra evitar spike qdo muda setPoint
            derivada = cfg->Kd * ( - ( entrada - entAnterior ) );
        }
        else
        {
            // deriva erro ( setPoint - entrada )
            derivada = cfg->Kd * ( erro - erroAnterior );
        }

        entAnterior = entrada;
        erroAnterior = erro;

        MV = constrain( proporcional + integral + derivada,
                        cfg->minMV,
                        cfg->maxMV );

        return MV;
    }

    void print()
    {
        //SERIALX.print( "i=" );
        //SERIALX.print( entAnterior );
        SERIALX.print( " e=" );
        SERIALX.print( erro );
        SERIALX.print( " [" );
        SERIALX.print( proporcional );
        SERIALX.print( " " );
        SERIALX.print( integral );
        SERIALX.print( " " );
        SERIALX.print( derivada );
        SERIALX.print( "] => " );
        SERIALX.print( MV );
    }

protected:
    unsigned long tUltimoLoop;  // timestamp da iteracao anterior de executa()
};

// ******************************************************************************
//		CONTROLADOR DE MOTORES
// ******************************************************************************
class Motor
{
public:
    PID pid;
    ConfigMotor *cfg;

    Motor() : cfg(NULL), atual(0), ultimoAcel(0), meta(0), encoder(0), enc1(NULL), enc2(NULL) { }

    void init( ConfigMotor *cfgm )
    {
        cfg = cfgm;

        pid.setConfig( &cfg->pid );
        pid.zera();

        pinMode( cfg->pino, OUTPUT );
        pinMode( cfg->pinoDir, OUTPUT );

        if ( cfg->pinoDirN >= 0 )
            pinMode( cfg->pinoDirN, OUTPUT);

        parar();
        refresh(true);
    }

    void setEncoder( volatile uint8_t *encoderVar1, volatile uint8_t *encoderVar2 )
    {
        enc1 = encoderVar1;
        enc2 = encoderVar2;
    }

    void parar()
    {
        write(0);
    }

    void move( char potencia100 )
    {
        meta100 = potencia100;
    }

    short getAtual()
    {
        return atual;
    }

    short getEncoder()
    {
        return encoder;
    }

    void write(int valor)
    {
        meta100 = meta = atual = valor;
    }

    void refresh( bool imediato = false )
    {
        if( delaySemBlock( &ultimoAcel, cfg->pid.sampleTime ) || imediato )
        {
            // atualiza encoder
            if( enc1 && enc2 )
            {
                encoder = *enc1 + *enc2;
                *enc1 = *enc2 = 0;
            }
            else
                encoder = 0;

            if( eeprom.dados.programa == PRG_LINE_FOLLOW && enc1 )
            {
                // usa encoder e controlador PID

                meta = meta100 * ENCODER_MAX;
                meta /= 100;

                pid.setPoint = meta;
                atual = -pid.executa( encoder );
            }
            else
            {
                // marreta saida sem feedback encoder

                if ( meta100 )
                {
                    // converte % de potencia em PWM 8 bits

                    short c = meta100 > 0 ? cfg->deadband : -cfg->deadband; // c = deadband com sinal
                    short fator = 255 - cfg->deadband;                          // faixa de controle linear
                    meta = c + ( meta100 * fator ) / 100;
                    meta = constrain( meta, -255, 255 );                        // range de saida: +/- [deadband ... 255]
                }
                else
                    meta = 0;

                // acelerador: v = v0 + at
                if ( meta > atual)
                {
                    if( atual == 0 && cfg->deadband ) // estava parado
                        atual = cfg->deadband;
                    else if( cfg->aceleracao )
                        atual += cfg->aceleracao;
                    else
                        atual = meta;

                    if( meta < atual) // passou do ponto
                        atual = meta;
                }
                else if ( meta < atual)
                {
                    if( atual == 0 && cfg->deadband ) // estava parado
                        atual = -cfg->deadband;
                    else if( cfg->aceleracao )
                        atual -= cfg->aceleracao;
                    else
                        atual = meta;

                    if ( meta > atual)
                        atual = meta;
                }

                if ( -cfg->deadband < atual && atual < cfg->deadband )
                    atual = 0;
            }

            // I/O

            atual = constrain( atual, -255, 255 ); // protecao de range

            // uma ultima olhada no freio de mao
            if( eeprom.dados.handBrake )
                meta = atual = 0;

            digitalWrite( cfg->pinoDir, (atual < 0) ^ cfg->invertido ? HIGH : LOW); // 1/2 ponte H

            if( cfg->pinoDirN >= 0 ) // pino de direcao invertido
            {
                if( atual ) // movendo, dirN = !dir
                    digitalWrite( cfg->pinoDirN, (atual < 0) ^ cfg->invertido ? LOW : HIGH); // outra 1/2 ponte H
                else        // freio, dirN = dir
                    digitalWrite( cfg->pinoDirN, (atual < 0) ^ cfg->invertido ? HIGH : LOW);
            }

            if( atual == 0 && cfg->pinoDirN >= 0 )  // conduz 100% pra freiar
                analogWrite( cfg->pino, 255 );
            else                                    // operacao normal
                analogWrite( cfg->pino, abs( atual ) );
        }
    }
protected:
    char  meta100;      // meta de saida em +/- %
    short meta;         // meta de saida raw -255 a 255
    short atual;        // saida raw -255 a 255
    short encoder;      // leitura encoder, raw
    unsigned long ultimoAcel;

    volatile uint8_t *enc1;
    volatile uint8_t *enc2;

    //char prioMeta;    // TODO (Mauricio#1#): prioridade processo que setou a meta
};

class Drive
{
public:
    Sensor *sensorEsq, *sensorFre, *sensorDir;
    Motor motorEsq, motorDir;

    void parar()
    {
        motorEsq.parar();
        motorDir.parar();
    }

    void refresh( bool imediato = false )
    {
        motorEsq.refresh( imediato );
        motorDir.refresh( imediato );
    }

    void move( char esq100, char dir100 )
    {
        /*
        esq100 = map( esq100, -100, 100, -eeprom.dados.velEscala, eeprom.dados.velEscala);
        esq100 = constrain( esq100, -eeprom.dados.velMax, eeprom.dados.velMax );
        esq100 += eeprom.dados.balanco;
        */
        motorEsq.move( esq100 );

        /*
        dir100 = map( dir100, -100, 100, -eeprom.dados.velEscala, eeprom.dados.velEscala);
        dir100 = constrain( dir100, -eeprom.dados.velMax, eeprom.dados.velMax );
        dir100 -= eeprom.dados.balanco;
        */
        motorDir.move( dir100 );
    }

    void vetorial(Vetor2i direcao)
    {
        direcao.Constrain( /* -100, 100 */ ); // entrada em %

        /*
            as equacoes de interpolacao dos movimentos de giro a esquerda
               e a direita estao divididas em 4 quadrantes:

                      y
                      |
             Q2 (-,+) | Q1 (+,+)
            __________|__________ x
                      |
             Q3 (-,-) | Q4 (+,-)
                      |

            a ideia eh controlar os motores como se uma reta a 45o dividisse cada quadrante em 2:

            y
            | *1
            |  /
            | / *2
            |/____x

            * 1 - curva suave, ex: roda direita > 0
            * 2 - rotacao, ex: roda direita < 0
        */

        if( direcao.y >= 0)
        {
            if( direcao.x >= 0)   // Q1 (+,+)
            {
                motorEsq.move( max( direcao.x, direcao.y ) );
                motorDir.move( direcao.y - direcao.x );
            }
            else        // Q2 (-,+)
            {
                motorEsq.move( direcao.x + direcao.y );
                motorDir.move( max( -direcao.x, direcao.y ) );
            }
        }
        else // (y < 0)
        {
            if( direcao.x < 0)    // Q3(-,-)
            {

                motorEsq.move( min( direcao.x, direcao.y ) );
                motorDir.move( direcao.y - direcao.x );
            }
            else        // Q4 (+,-)
            {
                motorEsq.move( direcao.x + direcao.y );
                motorDir.move( min( -direcao.x, direcao.y ) );
            }
        }
    }

    void vetorialSensor(Vetor2i direcao)
    {
        direcao.Constrain();

        //#define TRACE
        #ifdef TRACE
            SERIALX.print(" dir");
            direcao.print();
        #endif

        Vetor2i resultante = direcao;

        if( direcao.y > 0 ) // ainda nao ha sensores atras :. soh trata sensores se estiver indo pra frente
        {
            // int angulo = 45;    // em relacao ao eixo Y / direcao "frente", sen=cos=0,707
            int seno = 70;      // %
            int cosseno = 70;   // %

            // pra evitar interferencias, atualiza um sensor de cada vez
            static char turno = 0;

            ( turno ^= 1 )
                ? sensorEsq->refresh()
                : sensorDir->refresh();

            int s_esq = sensorEsq->getPorcentoCentro();
            int s_dir = sensorDir->getPorcentoCentro();

            #ifdef TRACE
                SERIALX.print(" se=");
                SERIALX.print(s_esq);
                SERIALX.print(" sd=");
                SERIALX.print(s_dir);
            #endif

            /*        y
                ______|______ x
                 dir /|\ esq
                    / | \
                   V  |  V
                      V
                  obstaculos
            */

            Vetor2i esq( -( ( seno * s_esq ) / 100 ) , ( ( cosseno * s_esq ) / 100 ) );

            #ifdef TRACE
                SERIALX.print(" esq");
                esq.print();
            #endif

            Vetor2i dir( ( ( seno * s_dir ) / 100 ) , ( ( cosseno * s_dir ) / 100 ) );

            #ifdef TRACE
                SERIALX.print(" dir");
                dir.print();
            #endif

            Vetor2i obstaculos = esq + dir;

            #ifdef TRACE
                SERIALX.print(" obs" );
                obstaculos.print();
                SERIALX.print(" |obs| ");
                SERIALX.print(obstaculos.norma());
            #endif

            //obstaculos.normalizar();
            obstaculos.Constrain();

            #ifdef TRACE
                SERIALX.print(" obs");
                obstaculos.print();
            #endif

            resultante += obstaculos ;

            #ifdef TRACE
                SERIALX.print(" |res| ");
                SERIALX.print(resultante.norma());
            #endif

            //resultante.normalizar();

            //resultante.y = constrain( resultante.y, 0, 100 ); // never come back !
        }

        #ifdef TRACE
            SERIALX.print(" res");
            resultante.print();
        #endif

        vetorial( resultante );

        #ifdef TRACE
            SERIALX.println("\n");
        #endif

        #ifdef TRACE
            printRodas();
        #endif
        //#undef TRACE
    }

    void printRodas()
    {
        SERIALX.print( NOME_RODA_ESQ );
        SERIALX.print(" ");
        SERIALX.println( motorEsq.getAtual() );

        SERIALX.print( NOME_RODA_DIR );
        SERIALX.print(" ");
        SERIALX.println( motorDir.getAtual() );
    }

    void gira( char porc = 0 )
    {
        move( porc, -porc );
    }

    void giraEsq( char porc = 100 )
    {
        gira( -porc );
    }

    void giraDir( char porc = 100 )
    {
        gira( porc );
    }

    void praFrente( char porc = 100 )
    {
        move( porc, porc );
    }
}
drive;

#ifdef RODAS_PWM_x4
	Drive drive2;
#endif

void enviaSensores(bool enviaComando = true)
{
    if(enviaComando)
    {
        SERIALX.print( NOME_AS );
        SERIALX.print( " " );
    }
    for (int x = 0; x < NUM_SENSORES; x++)
    {
        SERIALX.print( sensores[x].refresh().getValor() );
        SERIALX.print( " " );
    }
    SERIALX.println( "" );
}

// ******************************************************************************
//   LINE FOLLOWER
// ******************************************************************************

#define TRACE_LF

class LineFollower
{
public:
    PID pid;

    unsigned long tInicio;
    unsigned long tFim;
    unsigned long timeout;
    unsigned long debounce;
    unsigned long tPiscaLed;

    bool sensoresBool[ LF_NUM_SENSORES ];   // retorno do getBool()
    bool debounceArray[ LF_NUM_SENSORES ];  // debounce pra determinar se uma marca esq/dir eh na verdade um cruzamneto
    bool marcaEsq;
    bool marcaDir;
    bool buscaInicioVolta;
    bool estadoLed;

    int nGrupos;
    int eleito;

    void loop();
    bool calibrar();

    LineFollower()
    {
        zeraTudo();
    }

    void zeraTudo()
    {
        pid.zera();

        nGrupos = 0;
        tInicio = tFim = timeout = tPiscaLed = debounce = 0;
        marcaEsq = marcaDir = estadoLed = false;
        buscaInicioVolta = true;
        eleito = -1;

        for(int sb = 0; sb < LF_NUM_SENSORES; sb++)
            sensoresBool[sb] = debounceArray[sb] = false;
    }

    bool calibrado()
    {
        for( int pino = LF_PINO_0; pino < LF_PINO_N; pino++ )
            if( ! sensores[ pino ].cfg->calibrado() )
                return false;
        return true;
    }

    void iniciarCorrida()
    {
        if( ! calibrado() )
        {
            calibrar();
            if( ! calibrado() )
                return;
        }

        digitalWrite( PINO_IR_EN, HIGH );
        delay(8);

        pid.setConfig( &eeprom.dados.pid[ PID_CORRIDA ] );
        zeraTudo();

        refresh();

        if( nGrupos == 1 )
        {
            eleito = 0;
            trilho = grupos[0];
            eeprom.dados.programa = PRG_LINE_FOLLOW;

            if( trc )
            {
                delaySensores = 10;
                //delayStatus = 100;
            }
        }
        else
        {
            print();
            printErro(ERRO_LF_TRILHO);
            eeprom.dados.programa = DFT_PROGRAMA;
        }
    }

    void finalizarCorrida()
    {
        digitalWrite(PINO_IR_EN, LOW);

        eeprom.dados.programa = DFT_PROGRAMA;
        drive.parar();
        drive.refresh( true );

        if( trc )
        {
            delaySensores = -1;
            delayStatus = -1;
        }
    }

    class Grupo
    {
    public:
        int pontoMedio;
        int pontoMin;
        int pontoMax;
        int tamanho;
        Grupo()
        {}
        bool intersecciona( Grupo &b )
        {
            // expande um sensor pra cada lado
            int pmin = pontoMin > 1 ? pontoMin - 2 : 0;
            int pmax = pontoMax < LF_NUM_SENSORES - 2 ? pontoMax + 2 : LF_NUM_SENSORES;

            // se minimo ou maximo de b estiverem dentro do range
            if( ( b.pontoMin >= pmin && b.pontoMin <= pmax ) || ( b.pontoMax >= pmin && b.pontoMax <= pmax ) )
                return true;

            return false;
        }
        void print()
        {
            SERIALX.print("[");
            SERIALX.print((int)pontoMin);
            SERIALX.print(" ");
            SERIALX.print((int)pontoMedio);
            SERIALX.print(" ");
            SERIALX.print((int)pontoMax);
            SERIALX.print("]");
        }
    }
    grupos[LF_NUM_SENSORES/2], trilho;

    void print()
    {
        SERIALX.print("LF ");

        for( int y = LF_NUM_SENSORES-1; y >= 0 ; y-- )
        {
            if( eleito >=0 && y >= trilho.pontoMin && y <= trilho.pontoMax )
                SERIALX.print( sensoresBool[y] ? "T" : "?" );
            else
                SERIALX.print( sensoresBool[y] ? "A" : "_" );
        }
        SERIALX.print(" ");

/*
        if( eleito < 0 )
        {
            SERIALX.print("?");
            trilho.print();
            SERIALX.print("? ");
        }
*/
        for( int x = 0; x < nGrupos; x++ )
        {
            if( x == eleito ) SERIALX.print("|");

            grupos[ x ].print();

            if( x == eleito ) SERIALX.print("|");
            SERIALX.print(" ");
        }

        pid.print();

        SERIALX.println();

        //enviaSensores();
    }

    void refresh()
    {
        int s;

        nGrupos = 0;

        for( s = 0; s < LF_NUM_SENSORES; s++ )
            sensores[ LF_PINO_0 + s ].refresh();

        for( s = 0; s < LF_NUM_SENSORES; s++ )
            sensoresBool[s] = sensores[ LF_PINO_0 + s ].getBool();

        for( s = 0; s < LF_NUM_SENSORES; s++ )
        {
            if( sensoresBool[s] )
            {
                Grupo grp;

                grp.pontoMin = grp.pontoMax = s;
                grp.tamanho = 1;

                // https://www.pololu.com/docs/0J18/16
                unsigned short sensor = sensores[ LF_PINO_0 + s ].getPorcento();
                long num = (long)s * (long)sensor * 100;
                long den = sensor;

                // agrupa enquanto o proximo for true
                while( ( s < LF_NUM_SENSORES-1 ) && sensoresBool[ s + 1 ] )
                {
                    s++;
                    grp.pontoMax = s;
                    grp.tamanho++;

                    sensor = sensores[ LF_PINO_0 + s ].getPorcento();
                    num += (long)s * (long)sensor * 100;
                    den += sensor;
                }

                grp.pontoMedio = (int) ( num / den );

                grupos[ nGrupos ] = grp;
                nGrupos++;
            }
        }
    }

    bool timedout( unsigned long* pAgora )
    {
        *pAgora = millis();

//        SERIALX.print( "timeout = " );
//        SERIALX.print( timeout );
//        SERIALX.print( " agora = " );
//        SERIALX.print( *pAgora );

        if( timeout < *pAgora )
        {
//            SERIALX.println( " TIMEDOUT" );
            return true;
        }
//        SERIALX.println();
        return false;
    }

    int giraP( int setPoint = LF_SETPOINT, int aprox = 20 )
    {
        #ifdef TRACE_LF
//        SERIALX.print( "giraP( " );
//        SERIALX.print( setPoint );
//        SERIALX.println( " )" );
        #endif

        refresh();
        elegeTrilho();

        pid.setPoint = setPoint;

        drive.gira( -pid.executaSample( trilho.pontoMedio ) );

        drive.refresh();

        #ifdef TRACE_LF
            print();
        #endif

        if( abs ( pid.erro ) < aprox )
            return 0;

        return pid.erro;
    }

    void elegeTrilho()
    {
        // busca provavel trilho entre os grupos

        if( nGrupos )
        {
            int distEleito = 300; // distancia maxima do trilho anterior

            for( int ig = 0; ig < nGrupos; ig++ )
            {
                if( grupos[ig].tamanho < (LF_RANGE/2) ) // ignora cruzamentos e marcacoes
                {
                    if( trilho.intersecciona( grupos[ig] ) ) // apenas se houver intersecao
                    {
                        int distancia = abs( grupos[ig].pontoMedio - trilho.pontoMedio );

                        if( distancia < distEleito )
                        {
                            // close enough :-)
                            eleito = ig;
                            trilho = grupos[ eleito ];
                            distEleito = distancia;
                        }
                    }
                }
            }
        }
        else
            eleito = -1;

        #ifdef TRACE_LF
//        SERIALX.print( "trilho " );
//        trilho.print();
//        SERIALX.println();
        #endif
    }
}
lineFollower;

bool LineFollower::calibrar()
{
    drive.parar();
    drive.refresh( true );

    refresh();

    SERIALX.println( "Calibrando..." );

    digitalWrite(PINO_IR_EN, HIGH);

    // primeira rodada de leitura, estima valores maximo, minimo e medio

    unsigned short valores[LF_NUM_SENSORES];
    unsigned short minimo = 1023;
    unsigned short maximo = 0;

    for(int x = 0; x < LF_NUM_SENSORES; x++)
    {
        valores[x] = sensores[ LF_PINO_0 + x ].calibrar();

        if( valores[x] < minimo ) minimo = valores[x];
        if( valores[x] > maximo ) maximo = valores[x];
    }

    unsigned short medio = ( minimo + maximo ) >> 1;

    for(int x = 0; x < LF_NUM_SENSORES; x++)
    {
        valores[x] = sensores[ LF_PINO_0 + x ].calibrar();

        if( valores[x] < minimo ) minimo = valores[x];
        if( valores[x] > maximo ) maximo = valores[x];
    }

    // usa media como threshold pra tentar classificar em fita/piso

    int cntPiso = 0;
    int cntFita = 0;

    for( int x = 0; x < LF_NUM_SENSORES; x++ )
        if( valores[x] > medio )
            cntFita++;
        else
            cntPiso++;

    // se tem mais "trilho" que "brita" inverte os sensores
    bool invertido = ( cntFita > cntPiso );

    for( int x = LF_PINO_0; x < ( LF_PINO_0 + LF_NUM_SENSORES ); x++ )
    {
        sensores[x].cfg->invertido = invertido;
        sensores[x].cfg->minimo = medio-100;
        sensores[x].cfg->centro = medio;
        sensores[x].cfg->maximo = medio+100;
    }

    SERIALX.print( "minimo = " );
    SERIALX.println( minimo );
    SERIALX.print( "maximo = " );
    SERIALX.println( maximo );
    SERIALX.print( "medio = " );
    SERIALX.println( medio );

    // alerta que vai entrar em modo auto

    for( int l = 0; l < 15; l ++ )
    {
        delay(50);
        digitalWrite( PINO_LED, false );
        delay(100);
        digitalWrite( PINO_LED, true );
    }

    pid.cfg = &eeprom.dados.pid[ PID_CALIBRA ];

    zeraTudo();

    refresh();
    trilho = grupos[0];

    SERIALX.print( "trilho = " );
    trilho.print();
    SERIALX.println();

    timeout = millis() + LF_TIMEOUT_CAL;

    // gira tudo pro lado alto
    SERIALX.println("gira horario...");
    while( ( ! timedout( &agora) ) && giraP( LF_RANGE ) );

//    drive.parar();
//    drive.refresh( true );
//    delay( 100 );

    print();
    trilho.print();
    SERIALX.println();

    pid.zera();

    // gira tudo pro outro lado
    SERIALX.println("gira anti horario...");
    while( ( ! timedout( &agora) ) && giraP( 0 ) );

//    drive.parar();
//    drive.refresh( true );

    trilho.print();
    SERIALX.println();
    SERIALX.println("centrando...");

//    delay( 100 );
    pid.zera();

    // centra
    while( ( ! timedout( &agora) ) && giraP( LF_SETPOINT ) );

    drive.parar();
    drive.refresh( true );

    digitalWrite( PINO_LED, false );
    digitalWrite( PINO_IR_EN , LOW);

    // imprime sensores
    for( int x = 0; x < LF_NUM_SENSORES; x++ )
    {
        sensores[LF_PINO_0 + x].cfg->print();
        SERIALX.println();
    }

    if( agora > timeout )
        printErro( ERRO_LF_CALIBRA, "timeout" );

    if( ! calibrado() )
    {
        printErro( ERRO_LF_CALIBRA, "NAO calibrado" );
        return false;
    }

    SERIALX.println( "Calibragem OK" );

    return true;
}


void LineFollower::loop()
{
    bool traceLF = false;

    if( tFim && agora > tFim )
    {
        tFim = 0;

        finalizarCorrida();

        SERIALX.print("Lap: ");
        SERIALX.print(int((agora-tInicio)/1000));
        SERIALX.println("s");

        // pisca LED por 2,25s
        for( int l = 0; l < 15; l ++ )
        {
            drive.refresh();
            digitalWrite( PINO_LED, false );
            delay(50);
            drive.refresh();
            digitalWrite( PINO_LED, true );
            delay(50);
//            drive.refresh();
//            delay(50);
        }

        // enganei um bobo da casca do ovo :-P
        //iniciarCorrida();
        //buscaInicioVolta = false;
        return;
    }

    refresh();
    elegeTrilho();

    if( eleito >= 0 )
    {
        timeout = 0;

        // busca marcas esquerda/direita
        for( int ig = 0 ; ig < nGrupos ; ig++ )
        {
            if( ig != eleito )
            {
                // minimo de 2 sensores de distancia i.e 2 * peso
                if( ( grupos[ig].pontoMax + 2*2 ) < trilho.pontoMin )
                {
                    marcaDir = true;
                    traceLF = true;
                }

                if( ( trilho.pontoMax + 2*2 ) < grupos[ig].pontoMin )
                {
                    marcaEsq = true;
                    traceLF = true;
                }
            }
        }

        if( nGrupos == 1 )
        {
            if( debounce && agora > debounce )
            {
                debounce = 0;

                int conta1s = 0;
                for( int s = 0; s < LF_NUM_SENSORES; s++ )
                {
                    if( debounceArray[s] )
                    {
                        conta1s++;
                        debounceArray[s] = false;
                    }
                }

                if( ( marcaEsq && marcaDir ) || ( conta1s > (  LF_NUM_SENSORES / 2 ) ) )
                {
                    #ifdef TRACE_LF
                    if( trc )
                    {
                        SERIALX.println("C");

                    }
                    #endif
                }
                else if( marcaEsq )
                {
                    #ifdef TRACE_LF
                    if( trc )
                        SERIALX.println("E");
                    #endif
                }
                else if( marcaDir )
                {
                    #ifdef TRACE_LF
                    if( trc )
                        SERIALX.println("D");
                    #endif
                    if( buscaInicioVolta )
                    {
                        tInicio = agora;
                        buscaInicioVolta = false;
                    }
                    else
                    {
                        tFim = agora + 500;
                    }
                }
                marcaEsq = marcaDir = false;

                traceLF = true;
            }
        }
        else // ( nGrupos > 1 ) || grupos[0].tamanho < (LF_NUM_SENSORES/3)
        {
            traceLF = ( 0 == debounce );

            debounce = agora + eeprom.dados.delays.debounce;

            for( int s = 0; s < LF_NUM_SENSORES; s++ )
                debounceArray[s] |= sensoresBool[s];
        }
    }
    else
    {
        // perdeu trilho, continua tentando ate timeout

        if( 0 == timeout )
        {
            timeout = agora + LF_TIMEOUT;
            traceLF = true;
        }
        else if( agora < timeout )
        {
            if( delaySemBlock( &tPiscaLed, 500 ) )
            {
                traceLF = true;
                estadoLed = ! estadoLed;
            }
        }
        else // timeout
        {
            finalizarCorrida();

            eeprom.dados.programa = DFT_PROGRAMA;
            print();
            printErro( ERRO_LF_TRILHO, "timeout" );

            return;
        }
    }

    pid.setPoint = LF_SETPOINT;  // meio da barra de sensores
    pid.executaSample( trilho.pontoMedio );

    drive.move( (pid.MV > 0) ? (100 - pid.MV) : 100,
                (pid.MV < 0) ? (100 + pid.MV) : 100 );

    drive.refresh();

    digitalWrite( PINO_LED, estadoLed );

    if( trc )
    {
        if( traceLF )
        {
            traceLF = false;
            print();
        }
    }
}

// ******************************************************************************
//		SERVIDOR SERIAL
// ******************************************************************************

void enviaStatus(bool enviaComando = true)
{
    if(enviaComando)
    {
        SERIALX.print(CMD_STATUS);
        SERIALX.print(" ");
    }
    SERIALX.print( (int)eeprom.dados.programa );
    SERIALX.print( " " );
    SERIALX.print( (int)erro );
    SERIALX.print( " " );
    SERIALX.print( (int)eeprom.dados.handBrake );
    SERIALX.print( " " );
    SERIALX.print( drive.motorEsq.getAtual() );
    SERIALX.print( " " );
    SERIALX.print( drive.motorDir.getAtual() );

    SERIALX.print( " " );
    SERIALX.print( drive.motorEsq.getEncoder() );
    SERIALX.print( " " );
    SERIALX.print( drive.motorDir.getEncoder() );

    SERIALX.print( " " );
    drive.motorEsq.pid.print();
    SERIALX.print( " " );
    drive.motorDir.pid.print();


    #ifdef RODAS_PWM_x4
        SERIALX.print(" ");
        SERIALX.print(drive2.motorEsq.getAtual());
        SERIALX.print(" ");
        SERIALX.print(drive2.motorDir.getAtual());
    #endif
    #ifdef PINO_SERVO_PAN
        SERIALX.print(" ");
        SERIALX.print(pan.read());
    #endif
    #ifdef PINO_SERVO_TILT
        SERIALX.print(" ");
        SERIALX.print(tilt.read());
    #endif
    #ifdef PINO_SERVO_ROLL
        SERIALX.print(" ");
        SERIALX.print(roll.read());
    #endif
    SERIALX.println(" ");
}

void enviaJoystick()
{
    SERIALX.print(CMD_JOYPAD);
    SERIALX.print(" X ");
    SERIALX.print(gamepad.x.getPorcentoCentro());

    SERIALX.print(" (");
    SERIALX.print(gamepad.x.cfg->minimo);
    SERIALX.print(",");
    SERIALX.print(gamepad.x.cfg->centro);
    SERIALX.print(",");
    SERIALX.print(gamepad.x.cfg->maximo);
    SERIALX.print(") ");

    SERIALX.println(gamepad.x.valor);

    SERIALX.print(CMD_JOYPAD);
    SERIALX.print(" Y ");
    SERIALX.print(gamepad.y.getPorcentoCentro());

    SERIALX.print(" (");
    SERIALX.print(gamepad.y.cfg->minimo);
    SERIALX.print(",");
    SERIALX.print(gamepad.y.cfg->centro);
    SERIALX.print(",");
    SERIALX.print(gamepad.y.cfg->maximo);
    SERIALX.print(") ");

    SERIALX.println(gamepad.y.valor);
/*
    SERIALX.print(" Z ");
    SERIALX.print(gamepad.z.getPorcentoCentro(0));
    SERIALX.print(" R ");
    SERIALX.println(gamepad.r.getPorcentoCentro(0));
*/
}


typedef enum
{
    VAR_NULO = 0,
    VAR_CHAR,   // signed  8
    VAR_INT,    // signed 16
    VAR_LONG,   // signed 32
    VAR_FIXO,   // 16.16
    VAR_BOOL,

    VAR_PID,    // ConfigPID
    VAR_SENSOR, // ConfigSensor
    VAR_MOTOR,  // ConfigMotor
    VAR_GAMEPAD // ConfigGamepad
}
TipoVariavel;

class Variavel
{
public:
    char nome[TAM_NOME];
    char tipo;
    int linhas;
    int colunas;
    void *dados;

    Variavel( TipoVariavel tipo_=VAR_NULO, const char *nome_=NULL, void *dados_=NULL, int linhas_=1, int colunas_=1 )
    {
        declara( tipo_, nome_, dados_, linhas_, colunas_ );
    }

    void declara( TipoVariavel tipo_, const char *nome_, void *dados_, int linhas_=1, int colunas_=1 )
    {
        tipo = tipo_;
        strncpy( nome, nome_, TAM_NOME );
        dados = dados_;
        linhas = linhas_;
        colunas = colunas_;
    }

    Variavel& operator[]( size_t i )
    {

    }

    int getInt() const
    {
        switch( tipo )
        {
        case VAR_INT:
            return *((int*)dados);
        case VAR_LONG:
            return *((long*)dados);
        case VAR_FIXO:
            return ((Fixo*)dados)->getInt();
        case VAR_BOOL:
        default:
            break;
        }
        return 0;
    }

    long getLong() const
    {
        switch( tipo )
        {
        case VAR_INT:
            return *((int*)dados);
        case VAR_LONG:
            return *((long*)dados);
        case VAR_FIXO:
            return ((Fixo*)dados)->getLong();
        default:
            break;
        }
        return 0;
    }

    void* getMembro( char* membro )
    {
        return dados;
    }

    Fixo getFixo( char* membro = NULL ) const
    {
        void* var = dados; //getMembro( membro );

        switch( tipo )
        {
        case VAR_INT:
            return *((int*)var);
        case VAR_LONG:
            return (int)*((long*)var);
        case VAR_FIXO:
            return *((Fixo*)var);
        default:
            break;
        }

        return 0;
    }

    Variavel& operator+=( const Variavel& var )
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) += var.getInt();
            break;
        case VAR_LONG:
            *((long*)dados) += var.getLong();
            break;
        case VAR_FIXO:
            ((Fixo*)dados)->operator+=(var.getFixo());
            break;
        default:
            break;
        }
        return *this;
    }

    Variavel& operator-=( const Variavel& var )
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) -= var.getInt();
            break;
        case VAR_LONG:
            *((long*)dados) -= var.getLong();
            break;
        case VAR_FIXO:
            ((Fixo*)dados)->operator-=(var.getFixo());
            break;
        default:
            break;
        }
        return *this;
    }

    Variavel& operator*=( const Variavel& var )
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) *= var.getInt();
            break;
        case VAR_LONG:
            *((long*)dados) *= var.getLong();
            break;
        case VAR_FIXO:
            ((Fixo*)dados)->operator*=(var.getFixo());
            break;
        default:
            break;
        }
        return *this;
    }

    Variavel& operator/=( const Variavel& var )
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) /= var.getInt();
            break;
        case VAR_LONG:
            *((long*)dados) /= var.getLong();
            break;
        case VAR_FIXO:
            ((Fixo*)dados)->operator/=(var.getFixo());
            break;
        default:
            break;
        }
        return *this;
    }

    Variavel& inv()
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) = - (*((int*)dados));
            break;
        case VAR_LONG:
            *((long*)dados) = - (*((long*)dados));
            break;
        case VAR_BOOL:
            *((bool*)dados) = ! (*((bool*)dados));
            break;
        case VAR_FIXO:
            ((Fixo*)dados)->setLong( - ((Fixo*)dados)->getLong() );
            break;
        default:
            break;
        }
        return *this;
    }

    Erros converteAtribui( const Variavel& v )
    {
        switch( tipo )
        {
        case VAR_CHAR:
            *( (char*) dados ) = v.getInt();
            return SUCESSO;
        case VAR_INT:
            *( (int* ) dados ) = v.getInt();
            return SUCESSO;
        case VAR_LONG:
            *( (long*) dados ) = v.getLong();
            return SUCESSO;
        case VAR_FIXO:
            *( (Fixo*) dados ) = v.getFixo();
            return SUCESSO;
        case VAR_BOOL:
            *( (bool*) dados ) = v.getInt();
            return SUCESSO;
        default:
            break;
        }
        return ERRO_INTERPRETADOR;
    }

    Erros parse( const char *str )
    {
        switch( tipo )
        {
        case VAR_CHAR:
            *( (char*) dados ) = atoi( str );
            break;
        case VAR_INT:
            *( (int* ) dados ) = atoi( str );
            break;
        case VAR_LONG:
            *( (long*) dados ) = atol( str );
            break;
        case VAR_FIXO:
             ( (Fixo*) dados )->setFloat( atof( str ) );
            break;
        case VAR_BOOL:
            *( (bool*) dados ) = atoi( str );
            break;
        default:
            return ERRO_INTERPRETADOR;
        }
        return SUCESSO;
    }

    void print()
    {
        switch( tipo )
        {
        case VAR_CHAR:
            SERIALX.print( (int) *( (char*) dados ) );
            break;
        case VAR_INT:
            SERIALX.print( (int) *( (int* ) dados ) );
            break;
        case VAR_LONG:
            SERIALX.print( (long) *( (long*) dados ) );
            break;
        case VAR_FIXO:
            SERIALX.print( ( (Fixo*) dados )->getFloat(), 5 );
            break;
        case VAR_BOOL:
            SERIALX.print( (bool) *( (bool*) dados ) );
            break;
        default:
            break;
        }
    }
};

int compVar( const void *a, const void *b )
{
    return strncmp( ((Variavel*)a)->nome, ((Variavel*)b)->nome, TAM_NOME );
}

//#define TRACE_INTERPRETADOR

class Interpretador
{
public:
    Variavel var[NUM_VARS];
    char nvars;

    Interpretador() : nvars(0), linha(NULL) {}

    Variavel* buscaVar( const char *nome )
    {
        Variavel v( VAR_NULO, nome );
        return buscaVar( &v );
    }

    Variavel* buscaVar( const Variavel* chave )
    {
        return nvars
                ? ( Variavel* ) bsearch( chave, var, nvars, sizeof( Variavel ), compVar )
                : NULL ;
    }

    Variavel* declaraVar( TipoVariavel tipo, char *nome, void *dados, int linhas=1, int colunas=1 )
    {
        Variavel nova( tipo, nome, dados, linhas, colunas );

        #ifdef TRACE_INTERPRETADOR
            //SERIALX.print( "declara " );
            //SERIALX.println( nome );
        #endif

        if( buscaVar( nome ) )
        {
            SERIALX.print( nome );
            printErro( ERRO_VAR_EXISTE );
            return NULL;
        }

        if( ! ( nvars < NUM_VARS-1 ) )
        {
            printErro( ERRO_VAR_ARRAY );
            return NULL;
        }

        int elem = 0;
        if( nvars )
        {
            // insere ordenado no array
            for( elem = nvars; elem > 0 ; elem-- )
                if( compVar( &nova, &var[elem-1] ) < 0 )
                    var[elem] = var[elem-1];
                else
                    break;
        }
        var[elem] = nova;
        nvars++;

        #ifdef TRACE_INTERPRETADOR
            //SERIALX.print( " elem " );
            //SERIALX.println( elem );
        #endif

        return &var[elem];
    }

    void eval( char *lnh )
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "eval( " );
            SERIALX.print( lnh );
            SERIALX.println( " )" );
        #endif

        enum Erros rc = SUCESSO;
        Fixo res;
        Variavel resultado( VAR_FIXO, "ans", (void*) &res );
        eco = true;

        linha = lnh;
        getToken();

        // [ LValue ] [ = ] [ Expressao ]

        if( tipoToken == NOME )
        {
            // tenta primeiros comandos hard coded
            rc = evalHardCoded( &resultado );

            if( rc )
            {
                rc = SUCESSO;

                char bkpToken[TAM_TOKEN];
                strncpy( bkpToken, token, TAM_TOKEN );

                Variavel *v = buscaVar( token );

                getToken();

                if( v ) // LValue
                {
                    if( tipoToken == NULO )
                    {
                        // imprime valor da variavel
                        resultado = *v;
                    }
                    else if( token[0] == '=' )  // atribuicao
                    {
                        getToken();
                        rc = evalExpressao( &resultado );

                        if( ! rc )
                            rc = v->converteAtribui( resultado );

                        resultado = *v;
                    }
                    else
                    {
                        devolve();

                        strncpy( token, bkpToken, TAM_TOKEN );
                        tipoToken = NOME;

                        rc = evalExpressao( &resultado );
                    }
                }
                else
                    rc = ERRO_VAR_INVALIDA;
            }
        }
        else
            rc = evalExpressao( &resultado );

        if( eco )
        {
            SERIALX.print( resultado.nome );
            SERIALX.print( " = " );
            resultado.print();
            SERIALX.println();
        }

        if( rc )
        {
            printErro( rc );
            SERIALX.println();
        }
    }

    Erros evalHardCoded( Variavel* resultado )
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalHardCoded( " );
            SERIALX.print( token );
            SERIALX.println( " )" );
        #endif

        enum Erros rc = SUCESSO;

        char dest[TAM_TOKEN];
        strcpy( dest, token );

        eco=false;

        if( 0 == strncmp( token, CMD_GRAVA, TAM_TOKEN )  )	        // salva EEPROM
            eeprom.save();
        else if( 0 == strncmp( token, CMD_CARREGA, TAM_TOKEN ) )    // descarta mudancas e recarrega da EEPROM
            eeprom.load();
        else if( 0 == strncmp( token, CMD_DEFAULT, TAM_TOKEN ) )    // hard-coded
            eeprom.defaults();
        else if( 0 == strncmp( token, CMD_CAL, TAM_TOKEN ) )        // entra no modo autocalibracao de joystick
            gamepad.calibrar();
        else if( 0 == strncmp( token, CMD_CENTRO, TAM_TOKEN ) )     // sai do modo de autocalibracao e centra joystick
            gamepad.centrar();
        else if( 0 == strncmp( token, CMD_STATUS, TAM_TOKEN ) )
            enviaStatus();
        else if( 0 == strncmp( token, CMD_UNAME, TAM_TOKEN ) )
            uname();
        else if( 0 == strncmp( token, CMD_LF_CAL, TAM_TOKEN ) )     // calibra sensores do line follower
            lineFollower.calibrar();
        else if( 0 == strncmp( token, CMD_LF, TAM_TOKEN ) )
            lineFollower.iniciarCorrida();
        else if( 0 == strncmp( token, NOME_AS, TAM_TOKEN ) )
            enviaSensores(true);
        else if( 0 == strncmp( token, CMD_MV_PARAR, TAM_TOKEN ) )
        {
            eeprom.dados.programa = DFT_PROGRAMA;
            resetPrg = true;

            drive.parar();
            #ifdef RODAS_PWM_x4
                drive2.parar();
            #endif
        }
        else if( 0 == strncmp( token, NOME_RODA_ESQ, TAM_TOKEN ) )
        {
            getToken();

            if( token[0] == '=' )
            {
                getToken();

                if( SUCESSO == ( rc = evalExpressao( resultado ) ) )
                    drive.motorEsq.move( resultado->getInt() );
            }
            else
            {
                SERIALX.print( dest );          // ecoa nome da variavel
                SERIALX.print(" = ");
                SERIALX.println(drive.motorEsq.getAtual());
            }
        }
        else if( 0 == strncmp( token, NOME_RODA_DIR, TAM_TOKEN ) )
        {
            getToken();

            if( token[0] == '=' )
            {
                getToken();

                if( SUCESSO == ( rc = evalExpressao( resultado ) ) )
                    drive.motorDir.move( resultado->getInt() );
            }
            else
            {
                SERIALX.print( dest );          // ecoa nome da variavel
                SERIALX.print(" = ");
                SERIALX.println(drive.motorDir.getAtual());
            }
        }
        else if( 0 == strncmp( token, CMD_MV_RODAS, TAM_TOKEN ) )
        {
            getToken();

            if( SUCESSO == ( rc = evalExpressao( resultado ) ) )
            {
                int lw = resultado->getInt();

                if( SUCESSO == ( rc = evalExpressao( resultado ) ) )
                {
                    drive.motorEsq.move(lw);
                    drive.motorDir.move(resultado->getInt());
                }
            }
        }
        else if( 0 == strncmp( token, CMD_MV_VET, TAM_TOKEN ) )
        {
            eeprom.dados.programa = PRG_RC_SERIAL;
            Vetor2i direcao;
/*
            if ((tok = STRTOK(NULL, " ")))			// segundo token eh o percentual de potencia p/ eixo X
            {
                direcao.x = atoi(tok);
                if ((tok = STRTOK(NULL, " ")))		// terceiro token eh o percentual de potencia p/ eixo Y
                {
                    direcao.y = atoi(tok);
                    drive.vetorial( direcao );
                    #ifdef RODAS_PWM_x4
                        if ((tok = STRTOK(NULL, " ")))  // quarto token eh o percentual de potencia p/ eixo X
                        {
                            direcao.x = atoi(tok);
                            if ((tok = STRTOK(NULL, " ")))// quinto token eh o percentual de potencia p/ eixo Y
                            {
                                direcao.y = atoi(tok);
                                drive2.vetorial( direcao );
                            }
                        }
                    #endif
                }
            }
*/
        }
        else if( 0 == strncmp( token, CMD_BIP, TAM_TOKEN ) )
        {
            #ifdef PINO_BIP
            /*
                tok = STRTOK(NULL, " ");
                if (tok)			        // frequencia
                {
                    int hz = atoi(tok);
                    tok = STRTOK(NULL, " ");
                    if (tok)                // duracao
                        BIP(hz, atoi(tok));
                    else
                        BIP(hz, 200);
                }
            */
            #endif
        }
        else if( 0 == strncmp( token, CMD_JOYPAD, TAM_TOKEN ) )
        {
            int temp;

            // segundo token eh o status dos botoes
            getToken();
            if( SUCESSO == getInt( &temp ) )
            {
                // gambi
                if( gamepad.tipo != ConfigGamepad::TIPO_PC  )
                {
                    gamepad.setConfig( &eeprom.dados.joyPC );
                }

                gamepad.refreshBotoes( temp );

                // terceiro token eh o eixo X
                if( SUCESSO == getInt( &temp ) )
                {
                    gamepad.x.setValor( temp );

                    // quarto token eh o eixo Y
                    if( SUCESSO == getInt( &temp ) )
                    {
                        gamepad.y.setValor( temp );

                        // quinto token eh o eixo Z
                        if( SUCESSO == getInt( &temp ) )
                        {
                            gamepad.z.setValor( temp );

                            // sexto token eh o eixo Rudder
                            if( SUCESSO == getInt( &temp ) )
                                gamepad.r.setValor( temp );
                        }
                    }
                }
            }
            else
                enviaJoystick();
        }
        else
        {
            eco = true;
            rc = SKIP;
        }

        #ifdef TRACE_INTERPRETADOR
        if( SUCESSO == rc )
            SERIALX.println( " exec ok" );
        else
            SERIALX.println( " nope" );
        #endif

        return rc;
    }
private:

    char *linha;
    char token[TAM_TOKEN];

    enum TipoToken
    {
        NULO=0,
        NUMERO,
        NOME,
        DELIMIT,
        STRING,
        CHAVE,
        BLOCO
    } tipoToken;

    bool eco;               // imprime resultado da expressao

    // Expressao -> Termo [ + Termo ] [ - Termo ]

    Erros evalExpressao( Variavel* resultado )
    {
        enum Erros rc = SUCESSO;

        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalExpressao: " );
            SERIALX.print( token );
            SERIALX.print( " _ " );
            SERIALX.println( linha );
        #endif

        rc = evalTermo( resultado );

        char op;
        while( rc == SUCESSO && ( ( op = token[0] ) == '+' || op == '-' ) )
        {
            Fixo res;
            Variavel temp( VAR_FIXO, "tmp", &res );

            getToken();

            rc = evalTermo( &temp );

            switch( op )
            {
            case '-':
                *resultado -= temp;
                break;
            case '+':
                *resultado += temp;
                break;
            }
        }
        return rc;
    }

    // Termo -> Fator [ * Fator ] [ / Fator ]

    Erros evalTermo( Variavel* resultado )
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalTermo: " );
            SERIALX.print( token );
            SERIALX.print( " _ " );
            SERIALX.println( linha );
        #endif

        enum Erros rc = SUCESSO;

        rc = evalFator( resultado );

        char op;
        while( rc == SUCESSO && ( ( op = token[0] ) == '*' || op == '/' ) )
        {
            Fixo res;
            Variavel temp( VAR_FIXO, "tmp", &res );

            getToken();

            rc = evalFator( &temp );

            switch( op )
            {
            case '*':
                *resultado *= temp;
                break;
            case '/':
                *resultado /= temp;
                break;
            }
        }
        return rc;
    }

    // Fator -> [ - ][ + ] Atomo

    Erros evalFator( Variavel* resultado )
    {

        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalFator: " );
            SERIALX.print( token );
            SERIALX.print( " _ " );
            SERIALX.println( linha );
        #endif

        enum Erros rc = SUCESSO;

        // +/- unario
        char op = 0;

        if( tipoToken == DELIMIT && token[0] == '+' || token[0] == '-' )
        {
            op = token[0];
            getToken();
        }

        // parenteses
        if( token[0] == '(' )
        {
            getToken();
            rc = evalExpressao( resultado );
            if( token[0] != ')' )
                return ERRO_INTERPRETADOR;
            getToken();
        }
        else
            rc = evalAtomo( resultado );

        if( op == '-' )
            resultado->inv();

        return rc;
    }

    // NOME ou NUMERO
    Erros evalAtomo( Variavel* resultado )
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalAtomo token: " );
            SERIALX.print( token );
            SERIALX.print( " _ " );
            SERIALX.print( linha );
        #endif

        Erros rc = SUCESSO;

        if( tipoToken == NUMERO )
        {
            rc = resultado->parse( token );
        }
        else if( tipoToken == NOME )
        {
            Variavel *v = buscaVar( token );

            if( v )
            {
                resultado->converteAtribui( *v );
            }
            else
                rc = ERRO_VAR_INVALIDA;
        }

        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( " = " );
            resultado->print();
            SERIALX.println();
        #endif

        getToken();

        if( tipoToken == DELIMIT && token[0] == ';')
        {
            eco = false;

            #ifdef TRACE_INTERPRETADOR
                SERIALX.println("eco off");
            #endif

            getToken();
        }

        return rc;
    }

    Erros getInt( int* resultado )
    {
        Erros rc = SKIP;

        if( tipoToken == NUMERO )
        {
            Variavel temp( VAR_INT, "tmp", resultado );

            rc = evalExpressao( &temp );
        }

        return rc;
    }

    TipoToken getToken()
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "getToken " );
        #endif

        tipoToken = NULO;
        char *tok = token;

        while( ' ' == *linha ) linha++;

        if( *linha )
        {
            if( strchr("{}", *linha ) )
            {
                tipoToken = BLOCO;
                *tok++ = *linha++;
            }
            else if( isdelim( *linha ) )
            {
                tipoToken = DELIMIT;
                *tok++ = *linha++;
            }
            else if( isalpha( *linha ))
            {
                tipoToken = NOME;
                while( !isdelim( *linha ) ) *tok++ = *linha++;
            }
            else if( isdigit( *linha ))
            {
                tipoToken = NUMERO;
                while( !isdelim( *linha ) ) *tok++ = *linha++;
            }
        }

        *tok = 0;

        #ifdef TRACE_INTERPRETADOR
            switch( tipoToken )
            {
            case NULO:
                SERIALX.print("NULO ");
                break;
            case NUMERO:
                SERIALX.print("NUMERO ");
                break;
            case NOME:
                SERIALX.print("NOME ");
                break;
            case DELIMIT:
                SERIALX.print("DELIMIT ");
                break;
            case STRING:
                SERIALX.print("STRING ");
                break;
            case CHAVE:
                SERIALX.print("CHAVE ");
                break;
            case BLOCO:
                SERIALX.print("BLOCO ");
                break;
            }
            SERIALX.println( token );
        #endif

        return tipoToken;
    }

    void devolve()
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "devolve: " );
        #endif

        for( char *t = token ; *t ; t++ )
        {
            #ifdef TRACE_INTERPRETADOR
                SERIALX.print( *t );
            #endif
            linha--;
        }

        #ifdef TRACE_INTERPRETADOR
            SERIALX.println( "" );
        #endif
    }

    bool isdelim( char c )
    {
        return ( strchr("+-*/=();", c ) || c == 0 || c == ' ' );
    }
}
interpretador;

class Telnet
{
    char command[MAX_CMD];
    unsigned char pos;

public:
    Telnet() : pos(0)
    {}

    bool recebe()
    {
        while( SERIALX.available() > 0 )
        {
            char c = SERIALX.read();

            if ( pos == MAX_CMD )
            {
                pos = 0;
                printErro( ERRO_TAM_MAX_CMD );
            }
            else if( c == CMD_EOL )
            {
                command[ pos ] = 0;
                pos = 0;
                return true;
            }
            else
                command[ pos++ ] = c;
        }
        return false;
    }

    void loop()
    {
        while( recebe() )
            interpretador.eval( command );
    }
}
telnet;

void trataJoystick()
{
    if(gamepad.botoesEdgeR & BT_L3)
    {
        gamepad.calibrar();
        eeprom.dados.handBrake = 1;
        eeprom.dados.programa = PRG_RC_SERIAL;
    }

    if( (gamepad.botoesEdgeF & BT_A) || (gamepad.botoesEdgeF & BT_B) )
    {
        drive.parar();
		#ifdef RODAS_PWM_x4
			drive2.parar();
		#endif

        if( eeprom.dados.programa == PRG_LINE_FOLLOW )
        {
            lineFollower.finalizarCorrida();
        }

        // auto centra joystick
        gamepad.centrar();

        eeprom.dados.handBrake = (gamepad.botoesEdgeF & BT_B);

        // poe no modo RC
        eeprom.dados.programa = PRG_RC_SERIAL;
        //eeprom.dados.velEscala = 60;

        //desliga trace
        trc = false;
        delaySensores = -1;
        delayStatus = -1;
    }

    if(gamepad.botoesEdgeF & BT_X)
    {
        eeprom.dados.handBrake = 0;
        trc = true;
        lineFollower.calibrar();
    }

    if(gamepad.botoesEdgeF & BT_Y)
    {
        eeprom.dados.handBrake = 0;
        trc = true;
        lineFollower.iniciarCorrida();
    }
/*
    if(gamepad.botoesAgora & BT_LT)
    {
        // 4 rodas: controla servos com eixos Z e R
    }

    if(gamepad.botoesAgora & BT_RT)
    {
        eeprom.dados.handBrake = 0;
#ifdef PINO_ARMA
        digitalWrite(PINO_ARMA, HIGH);
    }
    else
    {
        digitalWrite(PINO_ARMA, LOW);
#endif
    }
*/
    if(gamepad.botoesEdgeF & BT_LB)
    {
        if( eeprom.dados.velEscala > 30 )
            eeprom.dados.velEscala -= 10;

        if( eeprom.dados.velEscala < 30 )
            eeprom.dados.velEscala = 30;
    }

    if(gamepad.botoesEdgeF & BT_RB)
    {
        if( eeprom.dados.velEscala < 100 )
            eeprom.dados.velEscala += 10;

        if( eeprom.dados.velEscala > 100 )
            eeprom.dados.velEscala = 100;
    }

    gamepad.botoesEdgeR = 0;
    gamepad.botoesEdgeF = 0;
}

// ******************************************************************************
//		SETUP
// ******************************************************************************

void setup()
{
    SERIALX.begin(115200);

    uname();

    eeprom.load();

    drive.motorEsq.init( &eeprom.dados.motorEsq );
    drive.motorDir.init( &eeprom.dados.motorDir );

#ifdef ENCODER_RODAS
    pinMode( 10, INPUT );
    enableInterruptFast( 10, CHANGE );
    pinMode( 11, INPUT );
    enableInterruptFast( 11, CHANGE );
    pinMode( 14, INPUT );
    enableInterruptFast( 14, CHANGE );
    pinMode( 15, INPUT );
    enableInterruptFast( 15, CHANGE );

    drive.motorEsq.setEncoder( &encoderEsq1, &encoderEsq2 );
    drive.motorDir.setEncoder( &encoderDir1, &encoderDir2 );
#endif

#ifdef RODAS_PWM_x4
    drive2.motorEsq.init( &eeprom.dados.motorEsqT );
    drive2.motorDir.init( &eeprom.dados.motorDirT );
#endif

    pinMode(PINO_IR_EN, OUTPUT);
    digitalWrite(PINO_IR_EN, LOW);

    pinMode(PINO_LED, OUTPUT);
    digitalWrite(PINO_LED, LOW);

#ifdef PINO_ARMA
    pinMode(PINO_ARMA, OUTPUT);
    digitalWrite(PINO_ARMA, LOW);
#endif

    for(int s=0; s<NUM_SENSORES; s++)
        sensores[s].setConfig(&eeprom.dados.sensores[s]);

    drive.sensorEsq = &sensores[1];
    drive.sensorDir = &sensores[2];
    drive.sensorFre = &sensores[3];

    gamepad.setConfig( &eeprom.dados.joyPC );

    // liga pull-up de pinos livres pra economizar energia
    unsigned char unused[] = PINO_UNUSED_ARRAY;
    for(unsigned char p=0; p < PINO_UNUSED_CNT; p++)
    {
        pinMode(unused[p], INPUT);
        digitalWrite(unused[p], HIGH);
    }

/*    // plaquinha com 8 leds conectada no canto do mega
    for( int i = 39; i <= 53; i += 2 )
        pinMode(i, OUTPUT);

    for( int x = 0; x < LF_NUM_SENSORES; x++ )
        digitalWrite( (53 - (2 * x)) , sensoresBool[x] );
*/


/*  Variavel* declaraVar( TipoVariavel tipo, char *nome, void *dados, char tam=1 )

	VAR_NULO = 0,
    VAR_CHAR,
    VAR_INT,
    VAR_LONG,  -> nao permite temporario !!!
    VAR_FIXO,
    VAR_BOOL,
    VAR_STRING
*/

    interpretador.declaraVar( VAR_CHAR, NOME_PROGRAMA,   &eeprom.dados.programa );
    interpretador.declaraVar( VAR_CHAR, NOME_BALANCO,    &eeprom.dados.balanco );
    interpretador.declaraVar( VAR_CHAR, NOME_VEL_MAX,    &eeprom.dados.velMax );
    interpretador.declaraVar( VAR_CHAR, NOME_VEL_ESCALA, &eeprom.dados.velEscala );
    interpretador.declaraVar( VAR_CHAR, NOME_FREIO,      &eeprom.dados.handBrake );
    interpretador.declaraVar( VAR_BOOL, NOME_TRACE,      &trc );
    interpretador.declaraVar( VAR_LONG, NOME_TIMESTAMP,  &agora );
    interpretador.declaraVar( VAR_INT,  NOME_ERRNO,      &erro );
    interpretador.declaraVar( VAR_INT,  NOME_T_SE,       &delaySensores );
    interpretador.declaraVar( VAR_INT,  NOME_T_ST,       &delayStatus );
    interpretador.declaraVar( VAR_INT,  NOME_T_RF,       &eeprom.dados.delays.ES );
    interpretador.declaraVar( VAR_INT,  NOME_T_DEB,      &eeprom.dados.delays.debounce );
    interpretador.declaraVar( VAR_INT,  NOME_ZERO_ESQ,   &eeprom.dados.motorEsq.deadband );
    interpretador.declaraVar( VAR_INT,  NOME_ACEL_ESQ,   &eeprom.dados.motorEsq.aceleracao );
    interpretador.declaraVar( VAR_INT,  NOME_ZERO_DIR,   &eeprom.dados.motorDir.deadband );
    interpretador.declaraVar( VAR_INT,  NOME_ACEL_DIR,   &eeprom.dados.motorDir.aceleracao );

    interpretador.declaraVar( VAR_FIXO, NOME_PID_CAL_KP,    &eeprom.dados.pid[ PID_CALIBRA ].Kp );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_CAL_KI,    &eeprom.dados.pid[ PID_CALIBRA ].Ki );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_CAL_KD,    &eeprom.dados.pid[ PID_CALIBRA ].Kd );
    interpretador.declaraVar( VAR_INT,  NOME_PID_CAL_MVX,   &eeprom.dados.pid[ PID_CALIBRA ].maxMV );
    interpretador.declaraVar( VAR_INT,  NOME_PID_CAL_MVN,   &eeprom.dados.pid[ PID_CALIBRA ].minMV );
    interpretador.declaraVar( VAR_INT,  NOME_PID_CAL_ZAC,   &eeprom.dados.pid[ PID_CALIBRA ].zeraAcc );

    interpretador.declaraVar( VAR_FIXO, NOME_PID_KP,        &eeprom.dados.pid[ PID_CORRIDA ].Kp );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_KI,        &eeprom.dados.pid[ PID_CORRIDA ].Ki );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_KD,        &eeprom.dados.pid[ PID_CORRIDA ].Kd );
    interpretador.declaraVar( VAR_INT,  NOME_PID_MVX,       &eeprom.dados.pid[ PID_CORRIDA ].maxMV );
    interpretador.declaraVar( VAR_INT,  NOME_PID_MVN,       &eeprom.dados.pid[ PID_CORRIDA ].minMV );
    interpretador.declaraVar( VAR_INT,  NOME_PID_ZAC,       &eeprom.dados.pid[ PID_CORRIDA ].zeraAcc );

    interpretador.declaraVar( VAR_FIXO, NOME_PID_RETA_KP,   &eeprom.dados.pid[ PID_RETA ].Kp );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_RETA_KI,   &eeprom.dados.pid[ PID_RETA ].Ki );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_RETA_KD,   &eeprom.dados.pid[ PID_RETA ].Kd );
    interpretador.declaraVar( VAR_INT,  NOME_PID_RETA_MVX,  &eeprom.dados.pid[ PID_RETA ].maxMV );
    interpretador.declaraVar( VAR_INT,  NOME_PID_RETA_MVN,  &eeprom.dados.pid[ PID_RETA ].minMV );
    interpretador.declaraVar( VAR_INT,  NOME_PID_RETA_ZAC,  &eeprom.dados.pid[ PID_RETA ].zeraAcc );

    interpretador.declaraVar( VAR_FIXO, NOME_PID_ME_KP,     &eeprom.dados.motorEsq.pid.Kp );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_ME_KI,     &eeprom.dados.motorEsq.pid.Ki );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_ME_KD,     &eeprom.dados.motorEsq.pid.Kd );
    interpretador.declaraVar( VAR_INT,  NOME_PID_ME_ZAC,    &eeprom.dados.motorEsq.pid.zeraAcc );

    interpretador.declaraVar( VAR_FIXO, NOME_PID_MD_KP,     &eeprom.dados.motorDir.pid.Kp );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_MD_KI,     &eeprom.dados.motorDir.pid.Ki );
    interpretador.declaraVar( VAR_FIXO, NOME_PID_MD_KD,     &eeprom.dados.motorDir.pid.Kd );
    interpretador.declaraVar( VAR_INT,  NOME_PID_MD_ZAC,    &eeprom.dados.motorDir.pid.zeraAcc );

}

// ******************************************************************************
//		for(;;) loop();
// ******************************************************************************

void loop()
{
    agora = millis();

    telnet.loop();

    trataJoystick();

    #ifdef PINO_BOTAO_CAL
    if( botaoCal.refresh().trocouEstado() )
    {
        // soltar o botao => false
        if( false == botaoCal.getEstado() )
        {
            lineFollower.calibrar();
        }
    }
    #endif

    #ifdef PINO_BOTAO_PRG
    if( botaoPrg.refresh().trocouEstado() )
    {
        if( false == botaoPrg.getEstado() )
        {
            if( eeprom.dados.programa == PRG_LINE_FOLLOW )
            {
                drive.parar();
                eeprom.dados.programa = DFT_PROGRAMA;
            }
            else
                lineFollower.iniciarCorrida();
        }
        else
        {
            digitalWrite( PINO_LED, HIGH );
        }
    }
    #endif

    if( delaySemBlock(&ultimoStatus, delayStatus) )
    {
//        digitalWrite( PINO_IR_EN, HIGH );
//        enviaSensores();
        enviaStatus();
        //lineFollower.pid.print();
    }

    if( delaySemBlock(&ultimoSensores, delaySensores) )
    {
        //enviaJoystick();
        digitalWrite( PINO_IR_EN, HIGH );
        //lineFollower.refresh();
        lineFollower.print();
    }

    //#define TESTE_PERFORMANCE

    #ifdef TESTE_PERFORMANCE
    static unsigned long passagensLoop = 0;
    static unsigned long ultimoLoop = 0;
    static unsigned long passagensIdle = 0;

    if( delaySemBlock(&ultimoLoop, 10000) )
    {
        if( trc )
        {
            SERIALX.print( passagensLoop / 10 );
            SERIALX.print(" fps ");
            SERIALX.print( passagensIdle / 10 );
            SERIALX.println(" idle");
        }
        passagensLoop = 0;
        passagensIdle = 0;
    }
    #endif

    static unsigned short msExec = 10;
    static unsigned long ultimaExec = 0;
    if( delaySemBlock(&ultimaExec, msExec) )
    {
        #ifdef TESTE_PERFORMANCE
            passagensLoop++;
        #endif

        switch(eeprom.dados.programa)
        {
        case PRG_RC_SERIAL:
        {
            #ifdef RODAS_PWM_x4
                drive.vetorial(gamepad.x.getPorcentoCentro() + gamepad.z.getPorcentoCentro(), -gamepad.y.getPorcentoCentro());
                drive2.vetorial(-gamepad.x.getPorcentoCentro() + gamepad.z.getPorcentoCentro(), -gamepad.y.getPorcentoCentro());
            #else
                Vetor2i direcao( gamepad.x.getPorcentoCentro(), -gamepad.y.getPorcentoCentro() );
//                if( gamepad.botoesAgora & BT_RT ) // arma ligada desabilita sensores
                    drive.vetorial( direcao );
//                else
//                    drive.vetorialSensor( direcao );
            #endif

            // fall through
        }
        case PRG_IDLE:
        {
            static unsigned long piscaLed = 0;
            static int intervalo = 100;

            if( delaySemBlock( &piscaLed, intervalo ) )
            {
                if( intervalo == 100 )
                {
                    digitalWrite( PINO_LED, false );
                    intervalo = 2000;
                }
                else
                {
                    digitalWrite( PINO_LED, true );
                    intervalo = 100;
                }
            }
        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_FOTOVORO:
        {
            const int threshold = 25;

            int ldrEsq = analogRead(0);
            int ldrDir = analogRead(1);

            // numeros menores significam mais luz

            if ( (ldrDir - ldrEsq) > threshold )		// vira pra esq
                drive.giraEsq();
            else if ( (ldrEsq - ldrDir) > threshold )	// vira pra direita
                drive.giraDir();
            else
                drive.praFrente();
        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_LINE_FOLLOW:
            lineFollower.loop();
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_LF_CAL:
            SERIALX.println(encoderDir1);
            SERIALX.println(encoderDir2);
            SERIALX.println(encoderEsq1);
            SERIALX.println(encoderEsq2);
        msExec = 1000;
        break;

        case PRG_COLISAO:
        {
            // ignora objetos muito longe
            if( drive.sensorFre->refresh().getValor() > 200 )
            {
                // calc velocidade em unidades sensor / segundo
                int v = ( (long) drive.sensorFre->delta() * 1000 ) / eeprom.dados.delays.ES;

                // s = s0 + v * t;
                int timeToCollision = ( 700 - drive.sensorFre->getValor() ) / v;

                if(timeToCollision < 3 )
                    eeprom.dados.programa = PRG_ALARME;
            }

        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_SENTINELA:
        {
            drive.sensorFre->refresh();
            if( abs(drive.sensorFre->delta() ) > 30 )
                eeprom.dados.programa = PRG_ALARME;
        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_SCOPE:
        {
            // pra conseguir performance melhor descarta os 2 bits menos significativos
            // e envia somente um byte
            SERIALX.write((analogRead(0) >> 2) & 0xFF);
        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_KNOB:
            #ifdef PINO_SERVO_PAN
            pan.write( map( analogRead(0), 0, 1023, 5, 174 ));
            #endif
        msExec = 50;
        break;

        case PRG_NAV_3S:
        {
            drive.sensorFre->refresh();
            drive.sensorEsq->refresh();
            drive.sensorDir->refresh();

            static char palpite = 0; // pra seguir girando pra um lado ateh encontrar um caminho livre

            #define MARGEM_SHARP 200
            #define MARGEM_PING 400

            if( drive.sensorFre->ehMinimo(MARGEM_PING) )
            {
//                int getReta()
//                    { return ( cfg->a * valor + cfg->b ); }
//                if( ! palpite )
//                    palpite = constrain( ( drive.sensorDir->getReta() - drive.sensorEsq->getReta() ), -1, 1 );

                if(palpite < 0)
                    drive.giraEsq( 50 );
                else
                    drive.giraDir( 50 );
            }
            else if( drive.sensorEsq->ehMinimo(MARGEM_SHARP) )
                drive.giraEsq( 50 );
            else if( drive.sensorDir->ehMinimo(MARGEM_SHARP) )
                drive.giraEsq( 50 );
            else
            {
                // ceu de brigadeiro
                drive.praFrente( 100 );
                palpite = 0;
            }

        }
        msExec = eeprom.dados.delays.ES;
        break;

        case PRG_ALARME:
        {
            digitalWrite(PINO_LED, HIGH);
            SERIALX.println("ALARME");

            #define SIRENE_TOM_MIN  1000
            #define SIRENE_TOM_MAX  3000
            #define SIRENE_PASSO    2
            #define SIRENE_COMPASSO 150

            for(int x=0; x<2; x++)
            {
                for(int tom=SIRENE_TOM_MIN;tom<SIRENE_TOM_MAX;tom+=SIRENE_PASSO)
                {
                    BIP(tom,SIRENE_COMPASSO);
                }

                for(int tom=SIRENE_TOM_MAX;tom>SIRENE_TOM_MIN;tom-=SIRENE_PASSO)
                {
                    BIP(tom,SIRENE_COMPASSO/2);
                }
            }
            delay(1000);
            #ifdef PINO_BIP
                // TODO (mbs#1#): noTone() naum funfa na v2
                noTone(PINO_BIP);
                digitalWrite(PINO_BIP, LOW);
            #endif
            digitalWrite(PINO_LED, LOW);
            //delay(1000);
        }
        eeprom.dados.programa = DFT_PROGRAMA;
        break;

        default:
            printErro( ERRO_PRG_INVALIDO );
            SERIALX.println(eeprom.dados.programa);
            eeprom.dados.programa = DFT_PROGRAMA;
        break;
        }
    }
    #ifdef TESTE_PERFORMANCE
    else
        passagensIdle++;
    #endif

    drive.refresh();
    #ifdef RODAS_PWM_x4
        drive2.refresh();
    #endif
}
