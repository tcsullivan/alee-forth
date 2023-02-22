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
: rot      >r swap r> swap ;
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
: _latest  1 cells ;
: imm      _latest @ dup @ 1 5 << | swap ! ;
: state    2 cells ;
: postpone 1 3 cells ! ; imm
: _input   4 cells ;

: decimal  1 1+ base ! 1010 base ! ;

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
: unloop   postpone 2r> ['] 2drop , ; imm
: +loop    postpone 2r> ['] rot , ['] + , ['] 2dup ,
           postpone 2>r ['] - , ['] 0= , ['] _jmp0 , ,
           postpone unloop ; imm
: loop     1 postpone literal postpone +loop ; imm
: i        postpone r@ ; imm 

: align    here 1 & if 1 allot then ;
: aligned  dup 1 & if 1+ then ;

: and      & ;
: or       | ;
: xor      ^ ;
: lshift   << ;
: rshift   >> ;
: invert   -1 ^ ;
: mod      % ;
: 2*       2 * ;
: 2/       2 / ;

: cr       9 emit ;
: bl       32 ;
: space    bl emit ;
: spaces   begin dup 0 > while space 1- repeat drop ;

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

: (        begin [char] ) key <> while repeat ; imm

: type     begin dup 0 > while swap dup c@ emit char+ swap 1- repeat 2drop ;
: s"       state @ if ['] _jmp , here 0 , then
           [char] " word count
           state @ 0= if exit then
           dup cell+ allot
           rot here swap !
           swap postpone literal postpone literal ; imm
: ."       postpone s" state @ if ['] type , else type then ; imm

: create   align here bl word count nip cell+ allot align
           ['] _lit , here 3 cells + , ['] exit , 0 ,
           dup @ 31 & over _latest @ - 6 << or over ! _latest ! ;
: _latword _latest @
           dup @ 31 & + cell+ aligned ;
: does>    _latword 2 cells +
           ['] _jmp over ! cell+
           here swap ! ] ;
: >body    cell+ @ ;

: variable create 1 cells allot ;
: constant create , does> ['] @ , postpone ; ;
( TODO fix compile-time does>... above should simply be "does> @ ;" )

-1 constant true
0  constant false

: >in      _input 80 chars + cell+ _input @ - 4 chars - ;
: source   _input @ 6 chars + >in 3 chars - swap ;

: quit     begin _rdepth 1 > while r> drop repeat postpone [ ;
: abort    begin depth 0 > while drop repeat quit ;

: recurse  _latword , ; imm

: move     begin dup 0 > while
           rot dup @ >r cell+
           rot r> over ! cell+
           rot 1- repeat drop 2drop ;
: fill     ( c-addr u char -- )
           -rot begin dup 0 > while
           >r 2dup c! char+ r> 1- repeat
           2drop drop ;

: environment? 2drop false ;

: accept   over >r begin dup 0 > while
           key dup 32 < if
           2drop r> - 1 chars / 0 else
           dup emit rot 2dup c! char+ nip swap 1- then
           repeat drop ;
