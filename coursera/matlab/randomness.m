
function m = randomness( limit, n, m )

    m = floor( rand( n, m ) * limit + 1 );