: *        m* drop ;
: /        0 swap _/ ;
: %        0 swap _% ;

: cell+    2 + ;
: cells    2 * ;

: .        0 sys ;
: emit     1 sys ;

: 1+       1 + ;
: 1-       1 - ;

: !        1 _! ;
: @        1 _@ ;
: ,        here ! 1 cells allot ;

: over     1 pick ;
: rot      >r swap r> swap ;
: -rot     rot rot ;
: nip      swap drop ;
: tuck     swap over ;

: 2drop    drop drop ;
: 2dup     over over ;
: 2over    3 pick 3 pick ;
: 2swap    rot >r rot r> ;

: c!       0 _! ;
: c@       0 _@ ;
: c,       here c! 1 allot ;
: char+    1+ ;
: chars    ;

: base     0 ;
: _latest  1 cells ;
: imm      _latest @ dup @ 1 5 << | swap ! ;
: state    2 cells ;
: postpone 1 3 cells ! ; imm
: _input   4 cells ;

: decimal  10 base ! ;
: hex      16 base ! ;

: [']      ' postpone literal ; imm
: [        0 state ! ; imm
: ]        1 state ! ;

: 2r>      ['] r> , ['] r> , ['] swap , ; imm
: 2>r      ['] swap , ['] >r , ['] >r , ; imm
: 2r@      ['] r> , ['] r> , ['] 2dup , ['] >r , ['] >r , ['] swap , ; imm
: r@       ['] r> , ['] dup , ['] >r , ; imm

: 2!       swap over ! cell+ ! ;
: 2@       dup cell+ @ swap @ ;
: +!       swap over @ + swap ! ;

: 0=       0 = ;
: 0<       0 < ;
: <=       - 1- 0< ;
: >        <= 0= ;
: <>       = 0= ;
: 0<>      0= 0= ;
: 0>       0 > ;

: if       ['] _jmp0 , here 0 , ; imm
: then     here swap ! ; imm
: else     ['] _jmp , here 0 , here rot ! ; imm

: begin    0 here ; imm
: while    swap 1+ swap postpone if -rot ; imm
: repeat   ['] _jmp , , if postpone then then ; imm
: until    ['] _jmp0 , , drop ; imm
: again    postpone repeat ; imm

: do       postpone 2>r here ; imm
: unloop   postpone 2r> ['] 2drop , ; imm
: +loop    postpone 2r> ['] rot , ['] + , ['] 2dup ,
           postpone 2>r ['] - , ['] 0= , ['] _jmp0 , ,
           postpone unloop ; imm
: loop     1 postpone literal postpone +loop ; imm
: i        postpone r@ ; imm 
: j        postpone 2r> postpone r@ ['] -rot , postpone 2>r ; imm

: align    here 1 cells 1- tuck & if 1 cells swap - allot else drop then ;
: aligned  dup 1 cells 1- tuck & if 1 cells swap - allot else drop then ;

: and      & ;
: or       | ;
: xor      ^ ;
: lshift   << ;
: rshift   >> ;
: invert   -1 ^ ;
: mod      % ;
: 2*       2 * ;
: 2/       2 / ;

: /mod     2dup % -rot / ;
: s>d      1 m* ;
: */       >r m* r> _/ ;
: */mod    >r m* 2dup r@ _% r> _/ ;
: sm/rem   >r 2dup r@ _% -rot r> _/ ;
: fm/mod   2dup dup >r ^ >r sm/rem swap dup
           if r> 0< if r> + swap 1- else swap r> drop then
           else swap 2r> 2drop then ;

: cr       10 emit ;
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
: \        _input @ begin dup 0 > while key drop 1- repeat drop ; imm

: type     begin dup 0 > while swap dup c@ emit char+ swap 1- repeat 2drop ;
: s"       state @ if ['] _jmp , here 0 , then
           [char] " word count
           state @ 0= if exit then
           dup cell+ allot
           rot here swap !
           swap postpone literal postpone literal ; imm
: c"       state @ if ['] _jmp , here 0 , then
           [char] " word
           state @ 0= if exit then
           dup count nip allot
           here rot !
           postpone literal ; imm
: ."       postpone s" state @ if ['] type , else type then ; imm
: .(       [char] ) word count type ; imm

: create   align here bl word count nip cell+ allot align
           ['] _lit , here 3 cells + , ['] exit dup , ,
           dup @ 31 & over _latest @ - 6 << or over ! _latest ! ;
: _latword _latest @
           dup @ 31 & + cell+ aligned ;
: _does>   _latword 2 cells +
           ['] _jmp over ! cell+
           r@ 1 cells - @ swap ! ;
: does>    ['] _jmp , here 2 cells + dup , 2 cells + ,
           ['] _does> , ['] exit , ; imm
: >body    cell+ @ ;
: compile, postpone literal postpone execute ;

: buffer:  create allot ;
: variable create 1 cells allot ;
: constant create , does> @ ;
: value    constant ;
: to       ' 4 cells + state @ if postpone literal ['] ! , else ! then ; imm
: defer    create does> @ execute ;
: defer@   >body @ ;
: defer!   >body ! ;
: is       state @ if postpone ['] postpone defer! else ' defer! then ; imm
: action-of state @ if postpone ['] postpone defer@ else ' defer@ then ; imm

-1 constant true
0 constant false

: >in      _input 80 chars + cell+ _input @ - 4 chars - ;
: source   _input @ 6 chars + >in 3 chars - swap ;

: quit     begin _rdepth 1 > while r> drop repeat postpone [ ;
: abort    begin depth 0 > while drop repeat quit ;

: recurse  _latword , ; imm

: move     begin dup 0 > while
           rot dup @ >r 1+
           rot r> over ! 1+
           rot 1- repeat drop 2drop ;
: fill     -rot begin dup 0 > while
           >r 2dup c! char+ r> 1- repeat
           2drop drop ;
: erase    begin dup 0 > while swap 0 over ! 1+ swap 1- repeat ;
: roll     dup if swap >r 1- recurse r> swap exit then drop ;

: environment? 2drop false ;

: accept   over >r begin dup 0 > while
           key dup 32 < if 2drop 0
           else dup emit rot 2dup c! char+ nip swap 1- then
           repeat drop r> - 1 chars / ;

: marker   create _latest @ , here , does>
           dup @ _latest ! cell+ @ here swap - allot ;
