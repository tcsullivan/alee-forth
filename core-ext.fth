-1 constant true
0 constant false

: hex      16 base ! ;

: nip      swap drop ;
: tuck     swap over ;

: 0>       0 > ;
: 0<>      0= 0= ;

: 2r@      ['] r> , ['] r> , ['] 2dup , ['] >r , ['] >r , ['] swap , ; imm

: compile, postpone literal postpone execute ;
: \        _source @ >in @ +
           begin dup c@ while 0 over c! char+ repeat drop ; imm
: again    postpone repeat ; imm
: ?do      ['] 2dup , ['] _lit , here 0 , ['] >r , ['] = , postpone if
           ['] 2drop , postpone 2r> ['] drop , ['] >r , ['] exit ,
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

: erase    begin dup 0 > while swap 0 over ! 1+ swap 1- repeat ;
: roll     dup if swap >r 1- recurse r> swap exit then drop ;

: marker   create _latest @ , here , does>
           dup @ _latest ! cell+ @ here swap - allot ;

: case     ['] _lit , 1 here 0 , ['] drop , ; imm
: of       ['] over , ['] = , postpone if ; imm
: endof    ['] _jmp , here >r 0 , postpone then
           swap 1+ swap r> tuck ! ; imm
: endcase  swap 0 do dup @ swap here swap ! loop drop ['] drop , ; imm
