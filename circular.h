
// (c) 2017 MBS - Mauricio Bieze Stefani

#ifndef CIRCULAR_H_INCLUDED
#define CIRCULAR_H_INCLUDED

template <class tipo, int tamanho> class Circular
{
    tipo circular[tamanho];
    int leitura;
    int escrita;

    int proximo( int ponteiro )
    {
        ponteiro++;

        if( ponteiro == tamanho )
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
        leitura = escrita = 0;
    }

    bool vazio()
    {
        return ( escrita == leitura );
    }

    void insere( const tipo & elemento )
    {
        circular[escrita] = elemento;

        incrementa(escrita);

        if( escrita == leitura )
            incrementa( leitura );
    }

    void retira()
    {
        if( escrita != leitura )
            incrementa( leitura );
    }

    tipo * topo()
    {
        return vazio() ? NULL : &circular[leitura];
    }
};

#endif // CIRCULAR_H_INCLUDED
