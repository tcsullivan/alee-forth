: *        m* drop ;
: s>d      1 m* ;
: /        >r s>d r> _/ ;
: %        >r s>d r> _% ;

: cell+    2 + ;
: cells    2 * ;

: .        0 sys ;
: u.       1 sys ;
: emit     2 sys ;

: 1+       1 + ;
: 1-       1 - ;

: '        _' drop ;
: !        1 _! ;
: @        1 _@ ;
: +!       dup >r swap r> @ + swap ! ;

: base     0 ;
: here     1 cells @ ;
: allot    1 cells +! ;
: _latest  2 cells ;
: imm      _latest @ dup @ 1 5 << | swap ! ;
: immediate imm ;
: state    3 cells ;
: _source  4 cells ;
: _sourceu 5 cells ;
: >in      6 cells ;

: ,        here ! 1 cells allot ;

: [        0 state ! ; imm
: ]        1 state ! ;

: literal  [ ' _lit dup , , ] , , ; imm
: [']      ' [ ' literal , ] ; imm

: if       ['] _jmp0 , here 0 , ; imm
: then     here swap ! ; imm
: else     ['] _jmp , here 0 , swap here swap ! ; imm

: postpone _' dup 0 = if exit then
           1 = swap ['] _lit , , if ['] execute ,
           else ['] , , then ; imm

: over     1 pick ;
: rot      >r swap r> swap ;
: -rot     rot rot ;

: 2drop    drop drop ;
: 2dup     over over ;
: 2over    3 pick 3 pick ;
: 2swap    rot >r rot r> ;

: c!       0 _! ;
: c@       0 _@ ;
: c,       here c! 1 allot ;
: char+    1+ ;
: chars    ;

: decimal  10 base ! ;

: 2r>      ['] r> , ['] r> , ['] swap , ; imm
: 2>r      ['] swap , ['] >r , ['] >r , ; imm
: r@       ['] r> , ['] dup , ['] >r , ; imm

: 2!       swap over ! cell+ ! ;
: 2@       dup cell+ @ swap @ ;

: 0=       0 = ;
: 0<       0 < ;
: <=       2dup < >r = r> | ;
: >        swap < ;
: <>       = 0= ;

: begin    0 here ; imm
: while    swap 1+ swap postpone if -rot ; imm
: repeat   ['] _jmp , , if postpone then then ; imm
: until    ['] _jmp0 , , drop ; imm

: do       ['] _lit , here 0 , ['] >r , postpone 2>r here ; imm
: unloop   postpone 2r> ['] 2drop , ['] r> , ['] drop , ; imm
: leave    postpone 2r> ['] 2drop , postpone 2r>
           ['] drop , ['] >r , ['] exit , ; imm
: +loop    postpone 2r> ['] 2dup , ['] swap , ['] < , ['] >r ,
           ['] rot , ['] + , ['] 2dup , ['] swap , ['] < ,
           ['] r> , ['] ^ , ['] -rot ,
           postpone 2>r ['] _jmp0 , ,
           postpone unloop here swap ! ; imm
: loop     postpone 2r> ['] 1+ , ['] 2dup ,
           postpone 2>r ['] = , ['] _jmp0 , ,
           postpone unloop here swap ! ; imm
: i        postpone r@ ; imm 
: j        postpone 2r> ['] r> , postpone r@ ['] swap ,
           ['] >r , ['] -rot , postpone 2>r ; imm

: align    here 1 cells 1- swap over & if 1 cells swap - allot else drop then ;
: aligned  dup 1 cells 1- swap over & if 1 cells swap - + else drop then ;

: and      & ;
: or       | ;
: xor      ^ ;
: lshift   << ;
: rshift   >> ;
: invert   -1 ^ ;
: mod      % ;
: 2*       2 * ;
: _msb     1 1 cells 8 * 1- << ;
: 2/       dup 1 >> swap 0< if _msb or then ;

: /mod     2dup % -rot / ;
: */       >r m* r> _/ ;
: sm/rem   >r 2dup r@ _% -rot r> _/ ;
: */mod    >r m* r> sm/rem ;
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
: min      2dup <= if drop else swap drop then ;
: max      2dup <= if swap drop else drop then ;

: source   _source @ 0 begin 2dup + c@ while char+ repeat ;
: key      _source @ >in @ +
           begin dup c@ 0 = while _in repeat
           c@ 1 >in +! ;
: key?     _source @ >in @ + c@ 0 <> ;
: word     here dup >r char+ >r
           begin key? if key 2dup <> else 0 0 then while
           r> swap over c! char+ >r repeat
           2drop r> r> swap over - 1- over c! ;
: count    dup char+ swap c@ ;
: char     bl word char+ c@ ;
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

: create   align here
           1 cells 1 chars - allot
           bl word count swap drop
           1 chars allot
           swap over over ! swap allot align
           ['] _lit , here 3 cells + , ['] exit dup , ,
           dup @ 31 & over _latest @ - 6 << or over ! _latest ! ;
: _does>   _latest @ dup @ 31 & + cell+ aligned 2 cells +
           ['] _jmp over ! cell+
           r@ 1 cells - @ swap ! ;
: does>    ['] _jmp , here 2 cells + dup , 2 cells + ,
           ['] _does> , ['] exit , ; imm
: >body    cell+ @ ;

: variable create 1 cells allot ;
: constant create , does> @ ;

: quit     begin _rdepth 1 > while r> drop repeat postpone [ ;
: abort    begin depth 0 > while drop repeat quit ;
: abort"   postpone s" ['] rot ,
           postpone if ['] type , ['] abort ,
           postpone else ['] 2drop , postpone then ; imm

: recurse  depth 1- pick dup @ 31 & + cell+ aligned , ; imm

: move     dup 0 <= if drop 2drop exit then
           >r 2dup < r> swap if
           1- 0 swap do over i + c@ over i + c! -1 +loop
           else
           0 do over i + c@ over i + c! loop
           then 2drop ;
: fill     -rot begin dup 0 > while
           >r 2dup c! char+ r> 1- repeat
           2drop drop ;

: environment? 2drop 1 0= ;

: accept   over >r begin dup 0 > while
           key dup 32 < if 2drop 0
           else dup emit rot 2dup c! char+ swap drop swap 1- then
           repeat drop r> - 1 chars / ;

: :noname  0 , here ] ;

: evaluate _source @ >r _sourceu @ >r >in @ >r
           0 >in ! _sourceu ! _source ! _ev
           r> >in ! r> _sourceu ! r> _source ! ;
