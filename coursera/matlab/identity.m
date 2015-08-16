
function id = identity( n )
    
    id =  zeros(n);
        
    for ij = 1:n
        id(ij,ij) = 1;
    endfor;