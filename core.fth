( : variable create 0 , ; )
( : create here const ; )

: .        0 sys ;
: emit     1 sys ;

: 1+       1 + ;
: 1-       1 - ;

: !        2 _! ;
: @        2 _@ ;
: ,        here ! 2 allot ;
: cell+    2 + ;
: cells    2 * ;

: over     1 pick ;
: -rot     rot rot ;
: nip      swap drop ;
: tuck     swap over ;

: 2drop    drop drop ;
: 2dup     over over ;
: 2over    3 pick 3 pick ;
: 2swap    rot >r rot r> ;

: c!       1 _! ;
: c@       1 _@ ;
: c,       here c! 1 allot ;
: char+    1+ ;
: chars    ;

: 2r>      r> r> swap ;
: 2>r      swap >r >r ;

: 2!       swap over ! cell+ ! ;
: 2@       dup cell+ @ swap @ ;
: +!       swap over @ + swap ! ;

: 0=       0 = ;
: 0<       0 < ;
: <=       - 1- 0< ;
: >        <= 0= ;

: base     0 ;
: state    2 ;
: decimal  1 1+ base ! 1010 base ! ;

: postpone 1 4 ! ; imm
: [']      ' postpone literal ; imm

: r@       ['] r> , ['] dup , ['] >r , ; imm

: if       ['] _jmp0 , here 0 , ; imm
: then     here swap ! ; imm
: else     ['] _jmp , here 0 , here rot ! ; imm

: begin    here 0 ; imm
: while    1+ postpone if swap ; imm
: repeat   ['] _jmp , if swap , postpone then else , then ; imm
: until    drop ['] _jmp0 , , ; imm

: do       ['] swap , ['] >r , ['] >r , here ; imm
: +loop    ['] r> , ['] r> , ['] swap , ['] rot , ['] + , ['] 2dup ,
           ['] swap , ['] >r , ['] >r , ['] - , ['] 0= ,
           ['] _jmp0 , , ['] r> , ['] r> , ['] swap , ['] 2drop , ; imm
: loop     1 postpone literal postpone +loop ; imm
: i        postpone r@ ; imm 

: align    here 1 & if 1 allot then ;
: aligned  dup 1 & if 1+ then ;

: and      & ;
: or       | ;
: xor      ^ ;
: lshift   << ;
: rshift   >> ;
: mod      % ;
: 2*       2 * ;
: 2/       2 / ;

: cr       9 emit ;
: bl       32 ;
: space    bl emit ;

: ?dup     dup if dup then ;

: negate   -1 * ;
: abs      dup 0< if negate then ;
: min      2dup <= if drop else nip then ;
: max      2dup <= if nip else drop then ;
