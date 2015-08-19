
function avg = moving_average( x )

    persistent buf = zeros(25);
    persistent i = 1;
    
    if i < 25
        i++;
    else 
        i=1;
    endif;
    
    buf(1,i) = x;
    
    avg = mean(buf);
    