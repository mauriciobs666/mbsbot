function g = letter_grade( score )
    if score < 61
        g = 'F';
    elseif score < 71
        g = 'D';
    elseif score < 81
        g = 'C';
    elseif score < 91
        g = 'B';
    else
        g = 'A';
    endif;
