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

#ifndef MATEMATICA_HPP_INCLUDED
#define MATEMATICA_HPP_INCLUDED

#include <math.h>

// ******************************************************************************
//		MATEMATICA DE PONTO FIXO
// ******************************************************************************

// 16.16
class Fixo
{
    long raw;

    Fixo( long raww )
    {
        raw = raww;
    }
public:
    Fixo() : raw(0) {}
    /*
    Fixo( int inteiro = 0, unsigned int fracao = 0 )
    {
        setInt( inteiro, fracao );
    }
    */

    int getInt()
    {
        return (int)( raw >> 16 );
    }
    unsigned int getFrac()
    {
        return ( raw & 0xFFFF );
    }
    Fixo& operator=( int inteiro )
    {
        setInt( inteiro );
        return *this;
    }
    void setInt( int inteiro = 0, unsigned int fracao = 0 )
    {
        raw = ( (long)inteiro << 16 ) + fracao;
    }
    long getLong()
    {
        return raw;
    }
    void setLong( long raww )
    {
        raw = raww;
    }
    float getFloat()
    {
        return raw / 65536.0;
    }
    void setFloat( float f )
    {
        raw = f * 65536;
    }
    int Constrain( int minimo, int maximo )
    {
        int i = getInt();
        if( i <= minimo )
        {
            setInt( i = minimo );
        }
        else if( i >= maximo )
        {
            setInt( i = maximo );
        }
        return i;
    }

    Fixo operator*( int valor )
    {
        return ( raw * valor );
    }
    Fixo operator*( Fixo& valor )
    {
        return ( ( raw >> 8 ) * ( valor.raw >> 8 ) );
    }
    Fixo operator+( Fixo& valor )
    {
        return raw + valor.raw;
    }
    Fixo operator-( Fixo& valor )
    {
        return raw - valor.raw;
    }
    Fixo& operator+=( const Fixo& valor )
    {
        raw += valor.raw;
        return *this;
    }
    Fixo& operator-=( const Fixo& valor )
    {
        raw -= valor.raw;
        return *this;
    }
    Fixo& operator*=( const Fixo& valor )
    {
        raw >>= 8;
        raw *= ( valor.raw >> 8 );
        return *this;
    }
    Fixo& operator/=( const Fixo& valor )
    {
        raw <<= 8;
        raw /= ( valor.raw >> 8 );
        return *this;
    }
    operator bool()
    {
        return raw;
    }
};

// ******************************************************************************
//      Vetor 2D int
// ******************************************************************************
// TODO (Mauricio#1#): template

class Vetor2i
{
public:
	int x, y;
	Vetor2i( int xx=0, int yy=0 ) : x(xx), y(yy)
        {}

	bool operator==(const Vetor2i& v) const
        { return ( ( x == v.x ) && ( y == v.y ) ); }
	bool operator!=(const Vetor2i& v) const
        { return ( ( x != v.x ) || ( y != v.y ) ); }

	void Constrain(int min_xy = -100, int max_xy = 100)
	{
	    if( x < min_xy )
            x = min_xy;
        else if( x > max_xy )
            x = max_xy;
	    if( y < min_xy )
            y = min_xy;
        else if( y > max_xy )
            y = max_xy;
	}
	int norma()
    {
        return (int) sqrt( x*x + y*y );
    }
    void normalizar()
    {
        int n = norma();
        *this *= 100;
        *this /= n;
    }

    Vetor2i operator+(const Vetor2i& v) const
    {
        Vetor2i n=*this;
        n.x+=v.x;
        n.y+=v.y;
        return n;
    }
    Vetor2i operator-(const Vetor2i& v) const
    {
        Vetor2i n=*this;
        n.x-=v.x;
        n.y-=v.y;
        return n;
    }
    Vetor2i operator*(int i) const
    {
        Vetor2i n=*this;
        n.x*=i;
        n.y*=i;
        return n;
    }
    Vetor2i operator/(int i) const
    {
        Vetor2i n=*this;
        n.x/=i;
        n.y/=i;
        return n;
    }

    Vetor2i& operator+=(const Vetor2i& v)
    {
        x+=v.x;
        y+=v.y;
        return *this;
    }
    Vetor2i& operator-=(const Vetor2i& v)
    {
        x-=v.x;
        y-=v.y;
        return *this;
    }
    Vetor2i& operator*=(int i)
    {
        x*=i;
        y*=i;
        return *this;
    }
    Vetor2i& operator/=(int i)
    {
        x/=i;
        y/=i;
        return *this;
    }
};

#endif // MATEMATICA_HPP_INCLUDED
