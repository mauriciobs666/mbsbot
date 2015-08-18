function [a,s] = meuRand(baixo,alto)
a = baixo + rand(3,4)* (alto-baixo)
v = a(:)
s = sum(v)
end