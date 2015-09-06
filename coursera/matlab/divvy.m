function B = divvy( A, k  )
    B = A
    B(rem(A,k)!=0) = B(rem(A,k)!=0)*k;