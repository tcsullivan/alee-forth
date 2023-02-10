( : variable create 0 , ; )
( : create here const ; )

( set decimal numbers )
10 0 !

: .      0 sys ;
: emit   1 sys ;

: over   1 pick ;
: -rot   rot rot ;
: nip    swap drop ;
: tuck   swap over ;

: +!     swap over @ + swap ! ;

: 1+ 1 + ;
: 1- 1 - ;

: 0= 0 = ;
: >= < 0= ;

: 2drop  drop drop ;
: 2dup   over over ;
: 2over  3 pick 3 pick ;
: 2swap  rot >r rot r> ;

: and    & ;
: or     | ;
: xor    ^ ;
: lshift << ;
: rshift >> ;
: 2*     2 * ;
: 2/     2 / ;

: ,      here ! 1 allot ;
: c!     ! ;
: c,     , ;
: c@     @ ;
: cell+  1+ ;
: cells  ;
: char+  1+ ;
: chars  ;

: cr 9 emit ;
: bl 32 ;

: base    0 ;
: decimal 1 2* base ! 1010 base ! ;

