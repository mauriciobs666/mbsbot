
function ret = older( y1, m1, d1, y2, m2, d2 )
    
    ret = y1 - y2;
    
    if ret == 0 
        
        ret = m1 - m2;
        
        if ret == 0
        
            ret = d1 - d2;
        
        endif;
    endif;

    ret=sign(ret);