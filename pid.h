/**	Copyright (C) 2010-2016 - Mauricio Bieze Stefani
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

#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED

#include "dados.h"

extern unsigned long agora; // millis()

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

    PID()
    {
        cfg = NULL;
        zera();
    }

    int executaSample( int entrada, bool reinicia = false )
    {
        if( ( ( agora - tUltimoLoop ) > cfg->sampleTime )
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
        zera(); // ou reinicia?
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
        Fixo tmp = cfg->Kp * erro;
        proporcional = tmp.getInt();

        // I
        if( reinicia )
        {
            // apenas mantem curso, ignora acumulador anterior
            acumulador = MV;
        }
        else if( erro > cfg->zeraAcc || erro < -cfg->zeraAcc )
        {
            acumulador += cfg->Ki * erro;
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
            tmp = cfg->Kd * ( entrada - entAnterior );
            derivada = tmp.getInt();
        }
        else
        {
            // deriva erro ( setPoint - entrada )
            tmp = cfg->Kd * ( erro - erroAnterior );
            derivada = tmp.getInt();
        }

        entAnterior = entrada;
        erroAnterior = erro;

        MV = constrain( proporcional + integral + derivada,
                        cfg->minMV,
                        cfg->maxMV );

        return MV;
    }

protected:
    unsigned long tUltimoLoop;  // timestamp da iteracao anterior de executa()
};

#endif // PID_H_INCLUDED
