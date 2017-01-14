
// (c) 2017 MBS - Mauricio Bieze Stefani

#ifndef CIRCULAR_H_INCLUDED
#define CIRCULAR_H_INCLUDED

template <class tipo, int tamanho> class CircularStats
{
    const static int alocados = tamanho + 1;

    tipo circular[ alocados ];
    tipo soma;
    int leitura;
    int escrita;

    int proximo( int ponteiro )
    {
        ponteiro++;

        if( ponteiro == alocados )
            ponteiro = 0;

        return ponteiro;
    }

    int incrementa( int & ponteiro )
    {
        return ( ponteiro = proximo( ponteiro ) );
    }

public:
    Circular()
    {
        limpa();
    }

    void limpa()
    {
        soma = 0;
        leitura = escrita = 0;
    }

    bool vazio()
    {
        return ( escrita == leitura );
    }

    int disponiveis()
    {
        int dif = escrita - leitura;
        return ( dif < 0 ) ? ( alocados + dif ) : dif;
    }

    void insere( const tipo & elemento )
    {
        circular[escrita] = elemento;

        soma += elemento;

        incrementa(escrita);

        if( escrita == leitura )
        {
            soma -= circular[ leitura ];
            incrementa( leitura );
        }
    }

    void retira()
    {
        if( escrita != leitura )
        {
            soma -= circular[ leitura ];
            incrementa( leitura );
        }
    }

    tipo * topo()
    {
        return vazio() ? NULL : &circular[leitura];
    }

    tipo media()
    {
        int sz = disponiveis();
        return ( ! sz ) ? 0 : ( soma / sz );
    }

    tipo variancia()
    {
        tipo med = media();
        tipo somaQ = 0;

        for( int l = leitura; l != escrita; incrementa( l ) )
        {
            somaQ += pow( circular[l] - med, 2 );
        }

        return ( somaQ / (tipo) disponiveis() );
    }
};

#endif // CIRCULAR_H_INCLUDED
