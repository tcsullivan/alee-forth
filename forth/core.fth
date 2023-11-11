: *        m* drop ;
: s>d      1 m* ;
: /        >r s>d r> _/ ;
: %        >r s>d r> _% ;
: um*      0 swap 0 _uma ;

: cell+    2 + ;
: cells    2 * ;
: char+    1 + ;
: chars    ;

: .        0 sys ;
: emit     2 sys ;

: 1+       1 + ;
: 1-       1 - ;

: over     1 pick ;
: rot      >r swap r> swap ;
: -rot     rot rot ;

: '        _' drop ;
: !        1 _! ;
: @        1 _@ ;
: +!       dup >r swap r> @ + swap ! ;

: _latest  2 cells ;
: imm      _latest @ dup @ 1 5 << | swap ! ;
: immediate imm ;

: [        0 3 cells ! ; imm
: ]        1 3 cells ! ;

: ,        1 cells dup >r @ ! r> dup +! ;

: literal  [ ' _lit dup , , ] , , ; imm
: [']      ' [ ' literal , ] ; imm

: base     0 ;
: here     [ 1 cells ] literal @ ;
: allot    [ 1 cells ] literal +! ;
: state    [ 3 cells ] literal ;
: _compxt  [ 4 cells ] literal ;
: _source  [ 5 cells ] literal ;
: _sourceu [ 6 cells ] literal ;
: >in      [ 7 cells ] literal ;
: _begin   [ 8 cells 80 chars + ] literal ;

: c!       0 _! ;
: c@       0 _@ ;
: c,       here c! 1 allot ;

: if       ['] _jmp0 , here 0 , ; imm
: then     here swap ! ; imm
: else     ['] _jmp , here 0 , swap here swap ! ; imm

: postpone _' dup 0 = if exit then
           1 = swap ['] _lit , , if ['] execute ,
           else ['] , , then ; imm

: 2drop    drop drop ;
: 2dup     over over ;
: 2over    3 pick 3 pick ;
: 2swap    rot >r rot r> ;

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
: leave    postpone 2r> ['] 2drop , ['] exit , ; imm
: +loop    ['] r> , ['] 2dup , ['] + ,
           postpone r@ ['] swap , ['] >r ,
           ['] - , ['] 2dup , ['] + , ['] over , ['] ^ ,
           ['] rot , ['] rot , ['] ^ , ['] & , ['] _lit , 0 ,
           ['] < , ['] _jmp0 , ,
           postpone unloop here 1 cells - swap ! ; imm
: loop     postpone 2r> ['] 1+ , ['] 2dup ,
           postpone 2>r ['] = , ['] _jmp0 , ,
           postpone unloop here 1 cells - swap ! ; imm
: i        postpone r@ ; imm 
: j        postpone 2r> ['] r> , postpone r@ ['] swap ,
           ['] >r , ['] -rot , postpone 2>r ; imm

: aligned  dup [ 1 cells 1- ] literal swap over & if [ 1 cells ] literal
           swap - + else drop then ;
: align    here dup aligned swap - allot ;

: and      & ;
: or       | ;
: xor      ^ ;
: lshift   << ;
: rshift   >> ;
: invert   -1 ^ ;
: mod      % ;
: 2*       2 * ;
: _msb     [ 1 1 cells 8 * 1- << ] literal ;
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

: source   _source @ _sourceu @ ;
: key      _source @ >in @ +
           begin dup c@ 0 = while _in repeat
           c@ 1 >in +! ;
: key?     _source @ >in @ + c@ 0 <> ;
: word     begin key? if key else -1 then 2dup <> until
           key? 0= if 2drop 0 here c! here exit then
           here begin char+ swap over c! swap
           key? if key else dup then
           2dup <> while rot repeat
           2drop here - here c! here ;
: count    dup char+ swap c@ ;
: char     bl word char+ c@ ;
: [char]   char postpone literal ; imm

: (        begin [char] ) key <> while repeat ; imm

: _type    >r begin dup 0 > while
           swap dup c@ r@ execute char+ swap 1- repeat 2drop r> drop ;
: type     [ ' emit ] literal _type ;
: s"       state @ if ['] _jmp , here 0 , then
           [char] " word count
           state @ 0= if exit then
           dup cell+ allot
           rot here swap !
           swap postpone literal postpone literal ; imm
: ."       postpone s" state @ if ['] type , else type then ; imm

: :noname  here dup _compxt ! 0 , here swap ] ;

: create   : here [ 4 cells ] literal + postpone literal postpone ; 0 , ;
: >body    cell+ @ ;

: _does>   >r _latest @ dup @ 31 & + cell+ aligned [ 2 cells ] literal +
           ['] _jmp over ! cell+ r> cell+ swap ! ;

: does>    state @ if
           ['] _lit , here 2 cells + , ['] _does> , ['] exit , else
           here dup _does> dup _compxt ! 0 , ] then ; imm

: variable create [ 1 cells ] literal allot ;
: constant create , does> @ ;

: quit     begin _rdepth 1 > while r> drop repeat postpone [ ;
: abort    begin depth 0 > while drop repeat quit ;
: abort"   postpone s" ['] rot ,
           postpone if ['] type , ['] abort ,
           postpone else ['] 2drop , postpone then ; imm

: recurse  _compxt @ dup @ 31 & + cell+ aligned , ; imm

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
           repeat drop r> - [ 1 chars ] literal / ;

: evaluate _source @ >r _sourceu @ >r >in @ >r
           0 >in ! _sourceu ! _source ! _ev
           r> >in ! r> _sourceu ! r> _source ! ;

: _isdigit ( ch -- bch )
  dup [char] 0 over <= swap [char] 0 base @ 10 min 1- + <= and
  if drop [char] 0 exit then
  base @ 11 < if drop 0 exit then
  base @ 36 min 10 - >r
  dup [char] a over <= swap [char] a r@ + < and
  if r> 2drop [char] a 10 - exit then
  [char] A over <= swap [char] A r> + < and
  if [char] A 10 - else 0 then ;
: >number  begin dup 0 >
           dup if drop over c@ _isdigit then while
           >r dup c@ swap >r base @ swap
           dup _isdigit - _uma
           r> char+ r> 1- repeat ;

: <#       40 here c! ;
: #>       2drop here dup c@ + 40 here c@ - ;
: hold     -1 here +! here dup c@ + c! ;
: #        base @
           >r 0 i um/mod r> swap >r um/mod r>
           rot 9 over <
           if 7 + then 48 + hold ;
: #s       begin # 2dup or 0= until ;
: sign     0< if [char] - hold then ;

: u.       0 <# bl hold #s #> type ;
