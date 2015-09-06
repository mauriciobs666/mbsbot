function v = square_wave( n )
    
    v = zeros(1,1001);
    
    fator = ( 4 * pi ) / 1001;
    
    for t = 1:1001
        for k = 1:n
            v(1,t) = v(1,t) + ( sin( (2*k-1) * t ) / ( 2*k -1 ) );
        endfor;
    endfor;
    
    plot(v);