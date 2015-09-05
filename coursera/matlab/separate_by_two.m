function [ par, impar ] = separate_by_two( A )
    par = A(rem(A(:)',2) == 0);
    impar = A(rem(A(:)',2) == 1);