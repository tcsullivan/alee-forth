-1 constant true
0 constant false

: unused   1 sys ;

: hex      16 base ! ;

: nip      swap drop ;
: tuck     swap over ;

: 0>       0 > ;
: 0<>      0= 0= ;
: u>       swap u< ;

: within   over - >r - r> swap u> ;

: 2r@      ['] r> , ['] r> , ['] 2dup , ['] >r , ['] >r , ['] swap , ; imm

: compile, postpone literal postpone execute ;
: \        _source @ >in @ +
           begin dup c@ while 0 over c! char+ repeat drop ; imm
: again    postpone repeat ; imm
: ?do      ['] 2dup , ['] _lit , here 0 , ['] >r , ['] = , postpone if
           ['] 2drop , postpone 2r> ['] drop , ['] >r , ['] leave ,
           postpone then postpone 2>r here ; imm

: .(       [char] ) word count type ; imm
: c"       state @ if ['] _jmp , here 0 , then
           [char] " word
           state @ 0= if exit then
           dup count nip allot
           here rot !
           postpone literal ; imm

: buffer:  create allot ;
: value    constant ;
: to       ' 4 cells + state @ if postpone literal ['] ! , else ! then ; imm
: defer    create does> @ execute ;
: defer@   >body @ ;
: defer!   >body ! ;
: is       state @ if postpone ['] postpone defer! else ' defer! then ; imm
: action-of state @ if postpone ['] postpone defer@ else ' defer@ then ; imm

: erase    0 fill ;
: roll     dup if swap >r 1- recurse r> swap exit then drop ;

: marker   create _latest @ , here , does>
           dup @ _latest ! cell+ @ here swap - allot ;

: case     ['] _lit , 1 here 0 , ['] drop , ; imm
: of       ['] over , ['] = , postpone if ; imm
: endof    ['] _jmp , here >r 0 , postpone then
           swap 1+ swap r> tuck ! ; imm
: endcase  swap 0 do dup @ swap here swap ! loop drop ['] drop , ; imm

: holds    begin dup while 1- 2dup + c@ hold repeat 2drop ;

: .r       over dup 0< if 1 else 0 then
           begin 1+ swap base @ / dup 0<> while swap repeat
           drop - spaces . ;
: u.r      over 0 begin 1+ swap 0 base @ _/ dup 0<> while swap repeat
           drop - spaces u. ;

( WORD uses HERE and must be at least 33 characters. )
: pad      here 50 chars + align ;

