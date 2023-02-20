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

: base     0 ;
: state    2 ;
: decimal  1 1+ base ! 1010 base ! ;

: postpone 1 4 ! ; imm
: [']      ' postpone literal ; imm
: [        0 state ! ; imm
: ]        1 state ! ;

: 2r>      ['] r> , ['] r> , ['] swap , ; imm
: 2>r      ['] swap , ['] >r , ['] >r , ; imm
: r@       ['] r> , ['] dup , ['] >r , ; imm

: 2!       swap over ! cell+ ! ;
: 2@       dup cell+ @ swap @ ;
: +!       swap over @ + swap ! ;

: 0=       0 = ;
: 0<       0 < ;
: <=       - 1- 0< ;
: >        <= 0= ;
: <>       = 0= ;

: if       ['] _jmp0 , here 0 , ; imm
: then     here swap ! ; imm
: else     ['] _jmp , here 0 , here rot ! ; imm

: begin    0 here ; imm
: while    swap 1+ swap postpone if -rot ; imm
: repeat   ['] _jmp , , if postpone then then ; imm
: until    ['] _jmp0 , , drop ; imm

: do       postpone 2>r here ; imm
: +loop    postpone 2r> ['] rot , ['] + , ['] 2dup ,
           postpone 2>r ['] - , ['] 0= , ['] _jmp0 , ,
           postpone 2r> ['] 2drop , ; imm
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
: spaces   begin dup 0 > while space 1- repeat ;

: ?dup     dup if dup then ;

: negate   -1 * ;
: abs      dup 0< if negate then ;
: min      2dup <= if drop else nip then ;
: max      2dup <= if nip else drop then ;

: word     here -1 cells over ! dup cell+ rot begin key 2dup <> while
           2 pick c! swap char+ swap repeat
           2drop over - over +!  ;
: count    dup cell+ swap @ ;
: char     bl word cell+ c@ ;
: [char]   char postpone literal ; imm

: type     begin dup 0 > while swap dup c@ emit char+ swap 1- repeat ;
: ."       [char] " word count type ;
