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

#ifndef SENSOR_H_INCLUDED
#define SENSOR_H_INCLUDED

#include "dados.h"

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
    unsigned short centrar()
    {
        refresh();
        if( cfg )
        {
            cfg->centro = valor;
            cfg->autoMinMax = false;
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
};

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

#endif // SENSOR_H_INCLUDED
