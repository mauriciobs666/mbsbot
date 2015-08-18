function g = generationXYZ( y )
    if y < 1966
        g = 'O';
    elseif y < 1981
        g = 'X';
    elseif y < 2000
        g = 'Y';
    elseif y < 2013
        g = 'Z';
    else
        g = 'K';
    endif;
