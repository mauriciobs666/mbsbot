function [ t, km ] = light_time( miles )

    km = miles * 1.609;
    t = km / ( 300000 * 60 );
    