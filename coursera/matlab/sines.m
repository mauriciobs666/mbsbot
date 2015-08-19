
function [s1, s2, sums] = sines(pts,amp,n1,n2)

    for i = 1:pts
        s1(1,i) = sin(2*pi*i*n1/pts);
        s2(1,i) = sin(2*pi*i*n2/pts);
        sums(1,i) = s1(1,i) + s2(1,i);
    endfor;
    
   % plot(s1);
   % hold on;
   % plot(s2);
   plot(sums);