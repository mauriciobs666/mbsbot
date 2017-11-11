
// (c) 2017 MBS - Mauricio Bieze Stefani

#ifndef INTERPRETADOR_HPP_INCLUDED
#define INTERPRETADOR_HPP_INCLUDED

#include "Arduino.h"

void printErro( int err )
{
    SERIALX.print( "Erro ");
    SERIALX.println( err );
}

typedef enum
{
    VAR_NULO = 0,
    VAR_CHAR,   // signed  8
    VAR_INT,    // signed 16
    VAR_LONG,   // signed 32
    VAR_BOOL,
    VAR_DOUBLE
}
TipoVariavel;

class Variavel
{
public:
    char nome[TAM_NOME];
    char tipo;
    int linhas;
    int colunas;
    void* dados;

    Variavel( TipoVariavel tipo_=VAR_NULO, const char* nome_=NULL, void* dados_=NULL, int linhas_=1, int colunas_=1 )
    {
        declara( tipo_, nome_, dados_, linhas_, colunas_ );
    }

    void declara( TipoVariavel tipo_, const char* nome_, void* dados_, int linhas_=1, int colunas_=1 )
    {
        tipo = tipo_;
        strncpy( nome, nome_, TAM_NOME );
        dados = dados_;
        linhas = linhas_;
        colunas = colunas_;
    }

    Variavel operator[]( size_t i )
    {
        void* pos = NULL;
        size_t siz;

        if( i >= linhas )
            printErro( ERRO_VAR_ARRAY );
        else
        {
                /*
            switch( tipo )
            {
            case VAR_CHAR:
                pos = (void*)(((char*)dados) + i);
                break;
            case VAR_INT:
                pos = (void*)(((int*)dados) + i);
                siz = sizeof( int );
                break;            case VAR_LONG:
                siz = sizeof( long );
                break;            case VAR_BOOL:
                siz = sizeof( bool );
                break;
            case VAR_PID:
                siz = sizeof( PID );
                break;
            case VAR_SENSOR:
                siz = sizeof( Sensor );
                break;
            case VAR_MOTOR:
                siz = sizeof( Motor );
                break;
            case VAR_GAMEPAD:
                siz = sizeof( MbsGamePad );
                break;
            }
                */
        }
        return Variavel( (TipoVariavel)tipo, nome, pos );;
    }

    int getInt() const
    {
        switch( tipo )
        {
        case VAR_CHAR:
            return *((char*)dados);
        case VAR_INT:
            return *((int*)dados);
        case VAR_LONG:
            return *((long*)dados);
        case VAR_BOOL:
            return *((bool*)dados) ? 1 : 0;
        case VAR_DOUBLE:
            return *((double*)dados);
        }
        return 0;
    }

    long getLong() const
    {
        switch( tipo )
        {
        case VAR_CHAR:
            return *((char*)dados);
        case VAR_INT:
            return *((int*)dados);
        case VAR_LONG:
            return *((long*)dados);
        case VAR_BOOL:
            return *((bool*)dados) ? 1 : 0;
        case VAR_DOUBLE:
            return *((double*)dados);
        }
        return 0;
    }

    double getDouble() const
    {
        switch( tipo )
        {
        case VAR_CHAR:
            return *((char*)dados);
        case VAR_INT:
            return *((int*)dados);
        case VAR_LONG:
            return *((long*)dados);
        case VAR_BOOL:
            return *((bool*)dados) ? 1 : 0;
        case VAR_DOUBLE:
            return *((double*)dados);
        }
        return 0;
    }

    Variavel& operator+=( const Variavel& var )
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) += var.getInt();
            break;
        case VAR_LONG:
            *((long*)dados) += var.getLong();
            break;
        case VAR_DOUBLE:
            *((double*)dados) += var.getDouble();
            break;
        default:
            break;
        }
        return *this;
    }

    Variavel& operator-=( const Variavel& var )
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) -= var.getInt();
            break;
        case VAR_LONG:
            *((long*)dados) -= var.getLong();
            break;
        case VAR_DOUBLE:
            *((double*)dados) -= var.getDouble();
            break;
        default:
            break;
        }
        return *this;
    }

    Variavel& operator*=( const Variavel& var )
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) *= var.getInt();
            break;
        case VAR_LONG:
            *((long*)dados) *= var.getLong();
            break;
        case VAR_DOUBLE:
            *((double*)dados) *= var.getDouble();
            break;
        default:
            break;
        }
        return *this;
    }

    Variavel& operator/=( const Variavel& var )
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) /= var.getInt();
            break;
        case VAR_LONG:
            *((long*)dados) /= var.getLong();
            break;
        case VAR_DOUBLE:
            *((double*)dados) /= var.getDouble();
            break;
        default:
            break;
        }
        return *this;
    }

    Variavel& inv()
    {
        switch( tipo )
        {
        case VAR_INT:
            *((int*)dados) = - (*((int*)dados));
            break;
        case VAR_LONG:
            *((long*)dados) = - (*((long*)dados));
            break;
        case VAR_BOOL:
            *((bool*)dados) = ! (*((bool*)dados));
            break;
        case VAR_DOUBLE:
            *((double*)dados) = - (*((double*)dados));
            break;
        default:
            break;
        }
        return *this;
    }

    Erros converteAtribui( const Variavel& v )
    {
        switch( tipo )
        {
        case VAR_CHAR:
            *( (char*) dados ) = v.getInt();
            return SUCESSO;
        case VAR_INT:
            *( (int* ) dados ) = v.getInt();
            return SUCESSO;
        case VAR_LONG:
            *( (long*) dados ) = v.getLong();
            return SUCESSO;
        case VAR_BOOL:
            *( (bool*) dados ) = v.getInt();
            return SUCESSO;
        case VAR_DOUBLE:
            *( (double*) dados ) = v.getDouble();
            return SUCESSO;
        default:
            break;
        }
        return ERRO_INTERPRETADOR;
    }

    Erros parse( const char *str )
    {
        switch( tipo )
        {
        case VAR_CHAR:
            *( (char*) dados ) = atoi( str );
            break;
        case VAR_INT:
            *( (int* ) dados ) = atoi( str );
            break;
        case VAR_LONG:
            *( (long*) dados ) = atol( str );
            break;
        case VAR_BOOL:
            *( (bool*) dados ) = atoi( str );
            break;
        case VAR_DOUBLE:
            *( (double*) dados ) = atof( str );
            break;
        }
        return SUCESSO;
    }

    void print()
    {
        switch( tipo )
        {
        case VAR_CHAR:
            SERIALX.print( (int) *( (char*) dados ) );
            break;
        case VAR_INT:
            SERIALX.print( (int) *( (int* ) dados ) );
            break;
        case VAR_LONG:
            SERIALX.print( (long) *( (long*) dados ) );
            break;
        case VAR_BOOL:
            SERIALX.print( (bool) *( (bool*) dados ) );
            break;
        case VAR_DOUBLE:
            SERIALX.print( (double) *( (double*) dados ), 5 );
            break;
        }
    }
};

int compVar( const void *a, const void *b )
{
    return strncmp( ((Variavel*)a)->nome, ((Variavel*)b)->nome, TAM_NOME );
}

//#define TRACE_INTERPRETADOR

class Interpretador
{
public:
    Variavel var[NUM_VARS];
    char nvars;

    Interpretador() : nvars(0), linha(NULL) {}

    Variavel* buscaVar( const char *nome )
    {
        Variavel v( VAR_NULO, nome );
        return buscaVar( &v );
    }

    Variavel* buscaVar( const Variavel* chave )
    {
        return nvars
                ? ( Variavel* ) bsearch( chave, var, nvars, sizeof( Variavel ), compVar )
                : NULL ;
    }

    Variavel* declaraVar( TipoVariavel tipo, char *nome, void *dados, int linhas=1, int colunas=1 )
    {
        Variavel nova( tipo, nome, dados, linhas, colunas );

        #ifdef TRACE_INTERPRETADOR
            //SERIALX.print( "declara " );
            //SERIALX.println( nome );
        #endif

        if( buscaVar( nome ) )
        {
            SERIALX.print( nome );
            printErro( ERRO_VAR_EXISTE );
            return NULL;
        }

        if( nvars >= NUM_VARS )
        {
            printErro( ERRO_VAR_ARRAY );
            return NULL;
        }

        int elem = 0;
        if( nvars )
        {
            // insere ordenado no array
            for( elem = nvars; elem > 0 ; elem-- )
                if( compVar( &nova, &var[elem-1] ) < 0 )
                    var[elem] = var[elem-1];
                else
                    break;
        }
        var[elem] = nova;
        nvars++;

        #ifdef TRACE_INTERPRETADOR
            //SERIALX.print( " elem " );
            //SERIALX.println( elem );
        #endif

        return &var[elem];
    }

    void eval( char *lnh )
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "eval( " );
            SERIALX.print( lnh );
            SERIALX.println( " )" );
        #endif

        Erros rc = SUCESSO;
        double res;
        Variavel resultado( VAR_DOUBLE, "ans", (void*) &res );
        eco = true;

        linha = lnh;
        getToken();

        // [ LValue ] [ = ] [ Expressao ]

        if( tipoToken == NOME )
        {
            // tenta primeiros comandos hard coded
            rc = evalHardCoded( &resultado );

            if( rc == SKIP )
            {
                rc = SUCESSO;

                char bkpToken[TAM_TOKEN];
                strncpy( bkpToken, token, TAM_TOKEN );

                Variavel *v = buscaVar( token );

                getToken();

                if( v ) // LValue
                {
                    if( tipoToken == NULO )
                    {
                        // imprime valor da variavel
                        resultado = *v;
                    }
                    else if( token[0] == '=' )  // atribuicao
                    {
                        getToken();
                        rc = evalExpressao( &resultado );

                        if( ! rc )
                            rc = v->converteAtribui( resultado );

                        resultado = *v;
                    }
                    else
                    {
                        devolve();

                        strncpy( token, bkpToken, TAM_TOKEN );
                        tipoToken = NOME;

                        rc = evalExpressao( &resultado );
                    }
                }
                else
                    rc = ERRO_VAR_INVALIDA;
            }
        }
        else
            rc = evalExpressao( &resultado );

        if( eco )
        {
            SERIALX.print( resultado.nome );
            SERIALX.print( " = " );
            resultado.print();
            SERIALX.println();
        }

        if( rc )
        {
            printErro( rc );
            SERIALX.println();
        }
    }

    Erros evalHardCoded( Variavel* resultado );

private:

    char *linha;
    char token[TAM_TOKEN];

    enum TipoToken
    {
        NULO=0,
        NUMERO,
        NOME,
        DELIMIT,
        STRING,
        CHAVE,
        BLOCO
    } tipoToken;

    bool eco;               // imprime resultado da expressao

    // Expressao -> Termo [ + Termo ] [ - Termo ]

    Erros evalExpressao( Variavel* resultado )
    {
        Erros rc = SUCESSO;

        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalExpressao: " );
            SERIALX.print( token );
            SERIALX.print( " _ " );
            SERIALX.println( linha );
        #endif

        rc = evalTermo( resultado );

        char op;
        while( rc == SUCESSO && ( ( op = token[0] ) == '+' || op == '-' ) )
        {
            double res;
            Variavel temp( VAR_DOUBLE, "tmp", &res );

            getToken();

            rc = evalTermo( &temp );

            switch( op )
            {
            case '-':
                *resultado -= temp;
                break;
            case '+':
                *resultado += temp;
                break;
            }
        }
        return rc;
    }

    // Termo -> Fator [ * Fator ] [ / Fator ]

    Erros evalTermo( Variavel* resultado )
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalTermo: " );
            SERIALX.print( token );
            SERIALX.print( " _ " );
            SERIALX.println( linha );
        #endif

        Erros rc = SUCESSO;

        rc = evalFator( resultado );

        char op;
        while( rc == SUCESSO && ( ( op = token[0] ) == '*' || op == '/' ) )
        {
            double res;
            Variavel temp( VAR_DOUBLE, "tmp", &res );

            getToken();

            rc = evalFator( &temp );

            switch( op )
            {
            case '*':
                *resultado *= temp;
                break;
            case '/':
                *resultado /= temp;
                break;
            }
        }
        return rc;
    }

    // Fator -> [ - ][ + ] Atomo

    Erros evalFator( Variavel* resultado )
    {

        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalFator: " );
            SERIALX.print( token );
            SERIALX.print( " _ " );
            SERIALX.println( linha );
        #endif

        Erros rc = SUCESSO;

        // +/- unario
        char op = 0;

        if( tipoToken == DELIMIT && token[0] == '+' || token[0] == '-' )
        {
            op = token[0];
            getToken();
        }

        // parenteses
        if( token[0] == '(' )
        {
            getToken();
            rc = evalExpressao( resultado );
            if( token[0] != ')' )
                return ERRO_INTERPRETADOR;
            getToken();
        }
        else
            rc = evalAtomo( resultado );

        if( op == '-' )
            resultado->inv();

        return rc;
    }

    // NOME ou NUMERO
    Erros evalAtomo( Variavel* resultado )
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "evalAtomo token: " );
            SERIALX.print( token );
            SERIALX.print( " _ " );
            SERIALX.print( linha );
        #endif

        Erros rc = SUCESSO;

        if( tipoToken == NUMERO )
        {
            rc = resultado->parse( token );
        }
        else if( tipoToken == NOME )
        {
            Variavel *v = buscaVar( token );

            if( v )
            {
                resultado->converteAtribui( *v );
            }
            else
                rc = ERRO_VAR_INVALIDA;
        }

        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( " = " );
            resultado->print();
            SERIALX.println();
        #endif

        getToken();

        if( tipoToken == DELIMIT && token[0] == ';')
        {
            eco = false;

            #ifdef TRACE_INTERPRETADOR
                SERIALX.println("eco off");
            #endif

            getToken();
        }

        return rc;
    }

    Erros getInt( int* resultado )
    {
        Erros rc = SKIP;

        if( tipoToken == NUMERO )
        {
            Variavel temp( VAR_INT, "tmp", resultado );

            rc = evalExpressao( &temp );
        }

        return rc;
    }

    TipoToken getToken()
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "getToken " );
        #endif

        tipoToken = NULO;
        char *tok = token;

        while( ' ' == *linha ) linha++;

        if( *linha )
        {
            if( strchr("{}", *linha ) )
            {
                tipoToken = BLOCO;
                *tok++ = *linha++;
            }
            else if( isdelim( *linha ) )
            {
                tipoToken = DELIMIT;
                *tok++ = *linha++;
            }
            else if( isalpha( *linha ))
            {
                tipoToken = NOME;
                while( !isdelim( *linha ) ) *tok++ = *linha++;
            }
            else if( isdigit( *linha ))
            {
                tipoToken = NUMERO;
                while( !isdelim( *linha ) ) *tok++ = *linha++;
            }
        }

        *tok = 0;

        #ifdef TRACE_INTERPRETADOR
            switch( tipoToken )
            {
            case NULO:
                SERIALX.print("NULO ");
                break;
            case NUMERO:
                SERIALX.print("NUMERO ");
                break;
            case NOME:
                SERIALX.print("NOME ");
                break;
            case DELIMIT:
                SERIALX.print("DELIMIT ");
                break;
            case STRING:
                SERIALX.print("STRING ");
                break;
            case CHAVE:
                SERIALX.print("CHAVE ");
                break;
            case BLOCO:
                SERIALX.print("BLOCO ");
                break;
            }
            SERIALX.println( token );
        #endif

        return tipoToken;
    }

    void devolve()
    {
        #ifdef TRACE_INTERPRETADOR
            SERIALX.print( "devolve: " );
        #endif

        for( char *t = token ; *t ; t++ )
        {
            #ifdef TRACE_INTERPRETADOR
                SERIALX.print( *t );
            #endif
            linha--;
        }

        #ifdef TRACE_INTERPRETADOR
            SERIALX.println( "" );
        #endif
    }

    bool isdelim( char c )
    {
        return ( strchr("+-*/=();", c ) || c == 0 || c == ' ' );
    }
};

#endif // INTERPRETADOR_HPP_INCLUDED
