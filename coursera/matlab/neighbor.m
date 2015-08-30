function rv = neighbor( v )
    
    rv = [];
    
    if isvector(v)
 #       rv = abs( v - v(1) );
        for i = 1:length(v)-1
            rv(i) = abs(v(i) - v(i+1));
        endfor;
    endif;