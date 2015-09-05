function p = myprime( n )
    p = ( n == 2 ) || ( rem(n,2) != 0 );
    for t = 3:2:n-1
        if rem(n,t) == 0
            p = false;
            return;
        endif;
    endfor;
    