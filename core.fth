( : variable create 0 , ; )
( : create here const ; )

: . 0 sys ;

: over   1 pick ;
: -rot   rot rot ;
: nip    swap drop ;
: tuck   swap over ;

: 1+ 1 + ;
: 1- 1 - ;

: 0= 0 = ;
: >= < 0= ;
