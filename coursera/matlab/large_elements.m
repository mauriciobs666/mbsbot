function rm = large_elements( X )
    rm = [];
    for r = 1:rows(X)
        for c = 1:columns(X)
            if( X(r,c) > r+c )
                rm = [ rm ; [ r c ] ];
            endif;
        endfor;
    endfor;
    