( : variable create 0 , ; )
( : create here const ; )

: !       2 _! ;
: @       2 _@ ;
: ,       here ! 2 allot ;
: cell+   2 + ;
: cells   2 * ;

: c!      1 _! ;
: c@      1 _@ ;
: c,      here c! 1 allot ;
: char+   1+ ;
: chars   ;

: align   here 1 & if 1 allot then ;
: aligned dup 1 & if 1 + then ;

( set decimal numbers )
10 0 !

: .       0 sys ;
: emit    1 sys ;

: over    1 pick ;
: -rot    rot rot ;
: nip     swap drop ;
: tuck    swap over ;

: +!      swap over @ + swap ! ;

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

