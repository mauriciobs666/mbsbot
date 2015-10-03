function digits = dial( str )
    digits = str;
   
    ap = (digits >= 'A') & (digits <= 'P');
    digits( ap ) = char( (fix( (digits( ap ) - 'A') / 3) ) + '2' );

    ry = (digits >= 'R') & (digits <= 'Y');
    digits( ry ) = char( (fix( (digits( ry ) - 'Q') / 3) ) + '7' );

    par = (digits == '(') | (digits == ')') | (digits == '-');
    digits( par ) = ' ';
