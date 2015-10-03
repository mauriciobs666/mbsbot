function [ m ] = May2015()
    s = [ 'Mon'; 'Tue'; 'Wed'; 'Thu'; 'Fri'; 'Sat'; 'Sun' ];
    for d = 1:31
        m(d) = struct( 'month', 'May', 'date', d, 'day', s(( rem(d,7)+1 ),:));
    end;