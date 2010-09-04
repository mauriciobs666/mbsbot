/**	Copyright (C) 2010 - Mauricio Bieze Stefani
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

#ifndef AVERAGE_HPP_INCLUDED
#define AVERAGE_HPP_INCLUDED

#ifndef NUM_READINGS
#define NUM_READINGS 10
#endif

class Average
{
public:
    Average()
    {
        for(int i=0; i < NUM_READINGS; i++)
            readings[i] = 0;
        total = 0;
        index = 0;
    }
    long read(long value)
    {
        total -= readings[(int)index];       // subtract the last reading:
        readings[(int)index] = value;
        total += readings[(int)index];

        if (index < NUM_READINGS)
            index++;
        else
            index = 0;

        return getAverage();
    }
    long getAverage()
    {
        return total / NUM_READINGS;
    }
private:
    long readings[NUM_READINGS]; // the readings from the analog input
    char index;                  // the index of the current reading
    long total;                  // the running total
};

#endif // AVERAGE_HPP_INCLUDED
