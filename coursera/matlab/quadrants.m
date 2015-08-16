function Q = quadrants( n )
    q1 = ones( n );
    q2 = ones( n ) * 2;
    q3 = ones( n ) * 3;
    q4 = ones( n ) * 4;
    Q = [ q1, q2; q3, q4 ];