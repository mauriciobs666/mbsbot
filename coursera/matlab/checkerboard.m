
function board = checkerboard(n,m)
    
    board = zeros(n,m);
    
    for i = 1:n
        for j = 1+rem(i,2):2:m
            board(i,j) = 1;
        endfor;
    endfor;
