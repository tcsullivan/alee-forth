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

: compile,  postpone literal postpone execute ;
: [compile] bl word find -1 = if , else compile, then ; imm
: \         _source @ >in @ +
            begin dup c@ while 0 over c! char+ repeat drop ; imm
: again     postpone repeat ; imm

: ?do       ['] _lit , here 0 , ['] >r , ['] 2dup , postpone 2>r
            ['] = , postpone if postpone leave postpone then
            here ; imm

: .(        [char] ) word count type ; imm
: c"        state @ if ['] _jmp , here 0 , then
            [char] " here char+ begin
            key dup 3 pick <> while
            over c! char+ repeat drop
            swap drop here - here c! here
            state @ 0= if exit then
            dup count nip 1+ allot
            here rot !
            postpone literal ; imm

: buffer:   create allot ;
: value     constant ;
: to        ' 4 cells + state @ if postpone literal ['] ! , else ! then ; imm

: defer     create ['] exit , does> @ execute ;
: defer@    >body @ ;
: defer!    >body ! ;
: is        state @ if postpone ['] postpone defer! else ' defer! then ; imm
: action-of state @ if postpone ['] postpone defer@ else ' defer@ then ; imm

: erase    0 fill ;
: roll     dup if swap >r 1- recurse r> swap exit then drop ;

: marker   here _latest @ create , , does>
           dup @ _latest ! cell+ @ here - allot ;

: case      0 ; imm
: of        ['] over , ['] = , postpone if ['] drop , ; imm
: endof     postpone else ; imm
: endcase   ['] drop , begin ?dup while postpone then repeat ; imm

: holds    begin dup while 1- 2dup + c@ hold repeat 2drop ;

: .r       over dup 0< if 1 else 0 then
           begin 1+ swap base @ / dup 0<> while swap repeat
           drop - spaces . ;
: u.r      over 0 begin 1+ swap 0 base @ _/ dup 0<> while swap repeat
           drop - spaces u. ;

( WORD uses HERE and must be at least 33 characters. )
: pad      here 50 chars + align ;

: parse     here dup >r swap begin
            key? if key else dup then 2dup <> while
            rot dup >r c! r> char+ swap repeat
            2drop r> tuck - ;

