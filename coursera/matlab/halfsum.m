function hs = halfsum( A )
    hs = 0;
    for i = 1:size(A,1)
        hs = hs + sum( A(i, i:end) ); 
    endfor;