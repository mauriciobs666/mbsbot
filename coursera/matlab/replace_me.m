
function w = replace_me(v,a,b,c)

j = 1;
for i = 1:length(v)
    if v(i) == a
        if exist('b')
            w(j) = b;
            j=j+1;

            if exist('c');
                w(j) = c;
            else
                w(j) = b;
            endif;
        else
            w(j) = 0;
            j=j+1;
            w(j) = 0;
        endif;
    else
        w(j) = v(i);
    endif;
    j = j+1;
endfor;
