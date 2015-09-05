function [ pai, k ] = approximate_pi( delta )
    k = 0;
    parcial = 0;
    do
        parcial = parcial + ( ( (-3)^(-k) ) / ( 2*k + 1) );
        pai = sqrt(12) * parcial;
        k++;
    until abs(pai - pi) < delta;
