# msp430 implementation

This is the MSP430 port of Alee Forth. It produces a binary that enters a REPL made available on the UART peripheral at 115200 baud. The specific target is MSP430G2553.

## Building

1. `make clean` (just in case)
2. `make msp430-prep`: Builds `alee` for the host computer and uses it to create an `alee.dat` blob containing bytecode for `forth/core.fth` and `forth/msp430.fth`.
3. `make msp430`: Produces `alee-msp430`, a standalone binary for the MSP430 with built-in core and msp430 word-sets.

The final binary is < 11 kB and provides 150 bytes for user dictionary in RAM (assuming 512 bytes of total RAM).

## msp430.fth

The msp430 word-set makes programming for the MSP430 easier:

* All register names are defined (P1OUT, ADC10MEM, etc.).
* `r!` and `r@` to write and read device memory (i.e. registers).
* `rset`, `rclr`, `rtgl` work like `r!` but set, clear, or toggle the given value/mask instead.

