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

#include "protocolo.h"
#include "placa.h"

// ******************************************************************************
//		Scanner IR 1D
// ******************************************************************************

class Scanner
{
    #define SCANNER_STEPS 30
    #define SCANNER_ANG ( 180 / SCANNER_STEPS )
public:
    Scanner() : stepAtual(0), stepDir(1)
        {}
    bool stepUp()
    {
        if(upperBound())
            stepAtual = (SCANNER_STEPS-1);
        else
            stepAtual++;
        return upperBound();
    }
    bool stepDown()
    {
        if(lowerBound())
            stepAtual = 0;
        else
            stepAtual--;
        return lowerBound();
    }
    void refreshServo()
        {
            #ifdef PINO_SERVO_PAN
            pan.write( (SCANNER_ANG/2) + stepAtual * SCANNER_ANG );
            #endif
        }
    void chase()
    {
        if( s->refresh() > 300 ) // threshold
        {
            if(stepDir < 0)
                stepDown();
            else
                stepUp();
        }
        else // nada a vista, apenas continua na mesma direcao
            step();

        if(upperBound() || lowerBound())
            reverseDir();

        refreshServo();
    }
    void fillArray()
    {
        dataArray[stepAtual] = s->refresh(); // le sensor

        if( step() ) // calcula proxima posicao, retorna true se encheu array
        {
            reverseDir();

//            SERIALX.print("RFA ");
//            for(int x = 0; x < SCANNER_STEPS; x++)
//            {
//                SERIALX.print(dataArray[x]);
//                SERIALX.print(" ");
//            }
//            SERIALX.println("");

        }
        refreshServo();
    }
    bool step()
        { return (stepDir < 0) ? stepDown() : stepUp(); }
    void reverseDir()
        { stepDir = -stepDir; }
    bool lowerBound()
        { return stepAtual <= 0; }
    bool upperBound()
        { return stepAtual >= (SCANNER_STEPS-1); }
    void setSensor(Sensor *ss)
        { s = ss; }
private:
    Sensor *s;
    short stepAtual;
    char stepDir;
    unsigned short dataArray[SCANNER_STEPS];
};
*/
