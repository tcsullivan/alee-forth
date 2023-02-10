( : variable create 0 , ; )
( : create here const ; )

( set decimal numbers )
10 0 !

: .       0 sys ;
: emit    1 sys ;

: over    1 pick ;
: -rot    rot rot ;
: nip     swap drop ;
: tuck    swap over ;

: and     & ;
: or      | ;
: xor     ^ ;
: lshift  << ;
: rshift  >> ;
: mod     % ;
: 2*      2 * ;
: 2/      2 / ;

: 0=      0 = ;
: 0<      0 < ;
: <=      2dup < -rot = and ;
: >       <= 0= ;

: 1+      1 + ;
: 1-      1 - ;

: 2drop   drop drop ;
: 2dup    over over ;
: 2over   3 pick 3 pick ;
: 2swap   rot >r rot r> ;

: r@      r> dup >r ;
: ,       here ! 1 allot ;
: +!      swap over @ + swap ! ;
: c!      ! ;
: c,      , ;
: c@      @ ;
: cell+   1+ ;
: cells   ;
: char+   1+ ;
: chars   ;
: 2!      swap over ! cell+ ! ;
: 2@      dup cell+ @ swap @ ;

: cr      9 emit ;
: bl      32 ;
: space   bl emit ;

: state   1 ;
: base    0 ;
: decimal 1 2* base ! 1010 base ! ;

: ?dup    dup if dup then ;

: negate  -1 * ;
: abs     dup 0< if negate then ;
: min     2dup <= if drop else nip then ;
: max     2dup <= if nip else drop then ;

: align ;
: aligned ;

