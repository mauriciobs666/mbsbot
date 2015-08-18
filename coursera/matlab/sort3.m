function v = sort3( a, b, c )
    if a <= b 
        if b <= c
            v = [ a b c ];
        else
            if a <= c
                v = [ a c b ];
            else 
                v = [ c a b ];
            endif;
        endif;
    else
        if a <= c
            v = [ b a c ];
        else
            if b <= c
                v = [ b c a ];
            else 
                v = [ c b a ];
            endif;
        endif;
    endif;
