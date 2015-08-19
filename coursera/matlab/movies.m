
function simul = movies( h1, m1, durmin1, h2, m2, durmin2 )

    fim1 = h1 * 60 + m1 + durmin1;
    ini2 = h2 * 60 + m2;

    if ( fim1 + 30 ) > ini2 && ( ini2 > fim1 )
        simul = 1;
    else
        simul = 0;
    endif;
