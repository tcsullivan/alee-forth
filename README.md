# Alee Forth

Alee Forth is a concise Forth implementation written in modern C++ that aims for portability, minimal program size, and execution efficiency.

## Cross-platform compatibility

Alee Forth relies on the C++20 standard. It *does not* rely on any operating system. As a result, portability extends down to microcontroller targets with < 1 kB of memory. See the `msp430` folder for an example of such a port.

System-specific functionality is obtained through a `sys` Forth word. This word calls a user-supplied C++ function that implements whatever functionality is needed.

## Forth compatibility

Alee implements the entire "core" and majority of the "core extension" [word-sets](https://forth-standard.org/standard/core). Implementation is tracked in `compat.txt` with missing words listed below. Fundamental words are hard-coded into Alee while the rest of the implementation is found in `forth/core.fth` and `forth/core-ext.fth`. Running Alee without these implementation files will leave you with a very minimal word-set. These files may be compiled into the Alee binary by building the `standalone` target.

**Missing** core extensions:  
```
.R HOLDS PAD PARSE PARSE-NAME REFILL RESTORE-INPUT S\" SAVE-INPUT SOURCE-ID U.R U> UNUSED WITHIN [COMPILE]
```

Alee aims for compliance with common Forth standards like Forth 2012 and ANS Forth. Compliance is tested using a [Forth 2012 test suite](https://github.com/gerryjackson/forth2012-test-suite). Supported test files are in the `test` directory with tests for unimplemented words commented out.

## Building

Alee requires `make` and a compiler that supports C++20. Simply running `make` will produce the `libalee.a` library and a REPL binary named `alee`. You will likely want to pass in the core implementation files by calling `./alee forth/core.fth forth/core-ext.fth`.

Other available build targets:

* `small`: Optimize for minimal binary size.
* `fast`: Optimize for maximum performance on the host system.
* `standalone`: Builds the core dictionary (`core.fth`) into the binary.
* `msp430-prep` and `msp430`: Builds a binary for the [MSP430G2553](https://www.ti.com/product/MSP430G2553) microcontroller. See the `msp430` folder for more information.

If building for a new platform, review these files: `Makefile`, `libalee/types.hpp`, and `libalee/state.hpp`.

