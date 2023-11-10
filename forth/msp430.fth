: vector!  10 sys ;
: byte!    11 sys ;
: byte@    12 sys ;
: reg!     13 sys ;
: reg@     14 sys ;
: sr+      15 sys ;
: sr-      16 sys ;
: lpm-exit 17 sys ;

: reg [ ' reg@ ' reg! ] literal literal ;
: byte [ ' byte@ ' byte! ] literal literal ;

: set ( b r reg/byte -- )
  >r over r> execute >r rot r> | -rot execute ;
: clear ( b r reg/byte -- )
  >r over r> execute >r rot invert r> & -rot execute ;
: toggle ( b r reg/byte -- )
  >r over r> execute >r rot r> ^ -rot execute ;

