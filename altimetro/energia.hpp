#ifndef ENERGIA_HPP_INCLUDED
#define ENERGIA_HPP_INCLUDED

#include "Arduino.h"

class Voltimetro
{
public:
    int valorMedido;

    Voltimetro( int pino = -1,
                double A = 1,
                double B = 0 )
                :   pinoSensor( pino ),
                    valorMedido(0)
    {
    }
    void refresh()
    {
        valorMedido = ( pinoSensor > 0) ? analogRead( pinoSensor ) : 0;
    }
    void setPino( int pino )
    {
        pinoSensor = pino;
    }
    // y = a*x + b
    void setFuncao( double A, double B )
    {
        a = A;
        b = B;
    }
    double getVolts()
    {
        return a * valorMedido + b;
    }
private:
    int pinoSensor;
    double a;
    double b;
};

#endif // ENERGIA_HPP_INCLUDED
