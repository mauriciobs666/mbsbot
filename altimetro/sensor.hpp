#ifndef SENSOR_HPP_INCLUDED
#define SENSOR_HPP_INCLUDED

class Botao
{
public:
    Botao() : pino(-1), estado(false), trocaEstado(false), debounce(0)
    {
    }
    void setup( int pinob )
    {
        pino = pinob;
        pinMode( pino, INPUT_PULLUP );
        estado = ! digitalRead( pino );
    }
    bool getEstado()
    {
        return estado;
    }
    bool isTrocaEstado()
    {
        bool retorno = trocaEstado;
        trocaEstado = false;
        return retorno;
    }
    void refresh( unsigned long t )
    {
        bool leitura = ! digitalRead( pino );

        if( leitura != estado )
        {
            if( debounce == 0 )
            {
                debounce = t + 100;
            }
            else if( debounce < t)
            {
                estado = leitura;
                trocaEstado = true;
                debounce = 0;
            }
        }
        else
            debounce = 0;
    }
private:
    int pino;
    bool estado;
    bool trocaEstado;
    unsigned long debounce;
};

#endif // SENSOR_HPP_INCLUDED
