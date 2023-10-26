: .s       depth dup 0 ?do dup i - pick . loop drop ;
: ?        @ . ;
: dump     hex 0 do i cells over + @ s>d <# # # # # bl hold #> type loop
           drop decimal ;

7 cells 80 chars + constant _begin
: words    _latest @ begin
           dup @ dup 31 &
           2 pick cell+ \ lt l len ws
           2 pick 6 >> 1023 < if \ lt l len ws
           rot 6 >> else \ lt len ws adv
           >r cell+ rot drop r> @ then
           -rot swap type space \ lt adv
           over _begin <> while - repeat 2drop ;

( xt -- caddr u )
: _nameof  _latest @ begin \ xt lt
           2dup < while \ xt lt
           dup @ \ xt lt l
           dup 6 >> 1023 < if \ xt lt l
           6 >> - else
           drop dup cell+ @ - then \ xt lt
           repeat nip \ lt
           dup @ swap cell+ over 31 & \ l lt len
           rot 6 >> 1023 = if swap cell+ swap then ; \ lt len
