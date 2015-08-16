
function [ mt, s ] = mtable( n, m )
    
    mt = zeros( n, m );
    
    for i = 1:n
        for j = 1:m
            mt( i, j ) = i * j;
        endfor;
    endfor;
    
    s = sum( mt(:) );
    