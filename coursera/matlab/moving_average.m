
function avg = moving_average( x )

    persistent buf;
    persistent i;
    
    if isempty(i)
        i = 0;
    endif;
    
    if i < 25
        i++;
    else 
        i=1;
    endif;
    
    buf(1,i) = x;
    
    avg = mean(buf);
    