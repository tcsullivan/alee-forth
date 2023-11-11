: vector!  10 sys ;
: byte!    11 sys ;
: byte@    12 sys ;
: reg!     13 sys ;
: reg@     14 sys ;
: sr+      15 sys ;
: sr-      16 sys ;
: lpm-exit 17 sys ;

: reg [ ' reg@ ' reg! ] literal literal ;
: byte [ ' byte@ ' byte! ] literal literal ;

: set ( b r reg/byte -- )
  >r over r> execute >r rot r> | -rot execute ;
: clear ( b r reg/byte -- )
  >r over r> execute >r rot invert r> & -rot execute ;
: toggle ( b r reg/byte -- )
  >r over r> execute >r rot r> ^ -rot execute ;

create _outs p1out , p2out , p3out , p4out , p5out , p6out ,
create _ins  p1in  , p2in  , p3in  , p4in  , p5in  , p6in  ,
create _dirs p1dir , p2dir , p3dir , p4dir , p5dir , p6dir ,
create _sel0 p1sel0 , p2sel0 , p3sel0 , p4sel0 , p5sel0 , p6sel0 ,
create _sel1 p1sel1 , p2sel1 , p3sel1 , p4sel1 , p5sel1 , p6sel1 ,

1 constant output
0 constant input

: pin-mode ( output? pin port -- )
  rot >r cells _dirs + @ byte r> if set else clear then ;

: pin-set ( high? pin port -- )
  rot >r cells _outs + @ byte r> if set else clear then ;

: pin-get ( pin port -- high? )
  cells _ins + @ byte@ swap and 0 > ;

: analog-init
  adcon adcsht_2 or adcctl0 reg set
  adcshp adcctl1 reg set
  adcres adcctl2 reg clear
  adcres_2 adcctl2 reg set
  adcie0 adcie reg set ;

: D0  bit5 1 ;
: D1  bit6 1 ;
: D2  bit1 2 ;
: D3  bit4 1 ;
: D4  bit7 2 ;
: D5  bit0 3 ;
: D6  bit1 3 ;
: D7  bit7 3 ;
: D8  bit6 3 ;
: D9  bit5 3 ;
: D10 bit4 4 ;
: D11 bit2 2 ;
: D12 bit6 2 ;
: D13 bit5 2 ;

: A0   bit0 0 ;
: A1   bit1 0 ;
: A2   bit5 0 ;
: A3   bit6 0 ;
: A4   bit2 0 ;
: A5   bit3 0 ;
: AREF bit4 0 ;

: LED1R bit1 5 ;
: LED1G bit0 5 ;
: LED1B bit2 5 ;

: LED2R bit6 4 ;
: LED2G bit5 4 ;
: LED2B bit7 4 ;
