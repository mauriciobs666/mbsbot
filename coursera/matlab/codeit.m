function coded = codeit( txt )
    for i = 1:length(txt)
        c = txt(i);
        if( c >= 'A' && c <= 'Z' )
            coded(i) = char( 'Z' - ( c - 'A' ) );
        elseif( c >= 'a' && c <= 'z' )
            coded(i) = char( 'z' - ( c - 'a' ) );
        else
            coded(i) = txt(i);
        end;
    end;