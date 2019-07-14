
// (c) 2017 MBS - Mauricio Bieze Stefani

#ifndef TOCADOR_HPP_INCLUDED
#define TOCADOR_HPP_INCLUDED

#ifndef ARDUINO_ARCH_ESP32
#include <toneAC.h>
#endif

class TocadorToneAC
{
public:
    void limpa()
    {
        leitura = escrita = 0;
        tocando = 0;
    }

    TocadorToneAC()
    {
        limpa();
    }

    void loop( unsigned long agora )
    {
        static bool mudoQdoTerminar = false;

        if( agora > tocando )
        {
            if( leitura != escrita )
            {
                #ifndef ARDUINO_ARCH_ESP32
                toneAC( notas[leitura].frequencia,
                        notas[leitura].volume,
                        notas[leitura].duracao,
                        true );
                #endif

                tocando = agora + notas[leitura].duracao;

                incrementa( leitura );

                //mudoQdoTerminar = true;
            }
            else
            {
                if( mudoQdoTerminar )
                {
                    #ifndef ARDUINO_ARCH_ESP32
                    noToneAC();
                    #endif
                    mudoQdoTerminar = false;
                }

                tocando = agora;
            }
        }
    }

    void insere( int duracao, int frequencia = 0, int volume = 0 )
    {
        notas[escrita].duracao = duracao;
        notas[escrita].frequencia = frequencia;
        notas[escrita].volume = volume;

        incrementa(escrita);

        if( escrita == leitura )
            incrementa( leitura );
    }

    void bipe( int nbips = 1, int vol = 10, int freq = 2700, int dur = 250, int inter = 250 )
    {
//            SERIALX.print("bipe(");
//            SERIALX.print( nbips );
//            SERIALX.print( ", " );
//            SERIALX.print( vol );
//            SERIALX.print( ", " );
//            SERIALX.print( freq );
//            SERIALX.println( ")" );

        for( int n = 0; n < nbips; n++ )
        {
            insere( dur, freq, vol );
            insere( inter );
        }
    }

    void bipeBlk( int freq, int vol, int dur )
    {
        #ifndef ARDUINO_ARCH_ESP32
        toneAC( freq, vol, dur );
        #endif
    }

private:
    int leitura;
    int escrita;
    unsigned long tocando;

    #define SZ_NOTAS 20
    struct Nota
    {
        int frequencia;
        int duracao;
        char volume;
    } notas[SZ_NOTAS];

    int proximo( int ponteiro )
    {
        ponteiro++;

        if( ponteiro == SZ_NOTAS )
            ponteiro = 0;

        return ponteiro;
    }

    int incrementa( int & ponteiro )
    {
        return ( ponteiro = proximo( ponteiro ) );
    }
};

#endif // TOCADOR_HPP_INCLUDED
