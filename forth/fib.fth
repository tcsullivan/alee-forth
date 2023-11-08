: fib ( n -- d )
  >r 0 dup 1 0 r> 0 do
  2dup 2>r 2swap 6 sys 2r> 2swap loop 2drop ;

: fibtest ( n -- )
  0 do i fib <# #s #> type space loop ;

: fibbench ( n -- )
  5 sys fib 5 sys >r 2drop r> ;

variable avg 0 avg !
100 constant iters

: bench ( -- )
  iters 0 do 100 fibbench avg +! loop
  avg @ iters / avg ! ;

bench ." avg time: " avg @ . ." us" cr
bye
