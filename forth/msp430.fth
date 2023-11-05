: vector! 10 sys ;
: reg!    11 sys ;
: reg@    12 sys ;
: 2reg!   13 sys ;
: 2reg@   14 sys ;
: sr+     15 sys ;
: sr-     16 sys ;

: reg [ ' reg@ ' reg! ] literal literal ;
: 2reg [ ' 2reg@ ' 2reg! ] literal literal ;

: set ( b r reg/wreg -- )
  >r over r> execute >r rot r> | -rot execute ;
: clear ( b r reg/wreg -- )
  >r over r> execute >r rot invert r> & -rot execute ;
: toggle ( b r reg/wreg -- )
  >r over r> execute >r rot r> ^ -rot execute ;

