
function ret = classify( x )

    sr = size(x,1);
    sc = size(x,2);
    
    if  sr == 0 && sc == 0
        ret = -1;
    elseif sr == 1 && sc == 1
        ret = 0;
    elseif sr == 1 || sc == 1
        ret = 1;
    else 
        ret = 2;
    endif;
    