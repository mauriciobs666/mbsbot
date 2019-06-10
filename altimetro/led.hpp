#ifndef LED_HPP_INCLUDED
#define LED_HPP_INCLUDED

#include "Arduino.h"

class Led
{
public:
    Led()
    {
        t1 = t2 = 0;
        estado = false;
        tProx = 0;
    }
    void setup( int pinoled )
    {
        pino = pinoled;
        pinMode( pino, OUTPUT );
    }
    void acende()
    {
        estado = true;
        digitalWrite( pino, estado );
    }
    void apaga()
    {
        estado = false;
        digitalWrite( pino, estado );
    }
    void loop( unsigned long agora )
    {
        if( tProx && tProx < agora )
        {
            if( estado )
                tProx += t2;
            else
                tProx += t1;

            estado = !estado;
            digitalWrite( pino, estado );
        }
    }
    void start( unsigned long agora, int lt1, int lt2 )
    {
        t1 = lt1;
        t2 = lt2;
        tProx = agora + t1;
        acende();
    }
    void stop()
    {
        tProx = 0;
        apaga();
    }
private:
    int t1;
    int t2;
    bool estado;
    unsigned long tProx;
    int pino;
};

#endif // LED_HPP_INCLUDED
