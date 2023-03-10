# Alee Forth

Alee is a concise Forth implementation written in modern C++ that aims for portability, minimal program size, and execution efficiency.

## Cross-platform compatibility

Alee relies on the C++17 standard. Alee *does not* rely on operating-system-specific functions, making portability easy.

The goal of portability extends down to microcontroller targets with kilobytes of memory. See the `msp430` target for an example of a port.

System-specific functionality is obtained through a `sys` Forth word. This word calls a user-supplied C++ function that implements the necessary (or any additional) functionality.

## Forth compatibility

Alee implements a large majority of the "core" and "core extension" [word sets](https://forth-standard.org/standard/core). Implementation is tracked in `compat.txt`, with missing words listed below. Fundamental words are built into Alee (written in C++); the rest of the implementation is in `core.fth` and `core-ext.fth`.

Running Alee without `core.fth` or `core-ext.fth` passed as arguments will leave you with a minimal word set. The `standalone` target will package the `core.fth` dictionary into the program.

**Missing** core features:  
* Pictured numeric output conversion (e.g. `<# #>`)
* Some words for unsigned integers: `U. U< UM/MOD`
* `>NUMBER`

**Missing** core extensions:  
```
.R HOLDS PAD PARSE PARSE-NAME REFILL RESTORE-INPUT S\" SAVE-INPUT SOURCE-ID U.R U> UNUSED WITHIN [COMPILE]
```

Alee aims for compliance with common Forth standards like Forth 2012 and ANS Forth. Compliance is tested using a [Forth 2012 test suite](https://github.com/gerryjackson/forth2012-test-suite). Supported test files are in the `test` directory, with tests for unimplemented words commented out.

## Building

Alee requires `make` and a C++17-compatible compiler. Simply running `make` will produce the `libalee.a` library and a REPL binary named `alee`. Note that this binary has no built-in libraries; these can be passed in by calling `./alee core.fth core-ext.fth`.

There are other build targets:

* `small`: Optimize for minimal binary size.
* `fast`: Optimize for maximum performance on the host system.
* `standalone`: Builds the core dictionary (`core.fth`) into the binary.
* `msp430`: Builds a binary for the [MSP430G2553](https://www.ti.com/product/MSP430G2553) microcontroller. The `standalone` target must be built first for the core dictionary.

If building for a new platform, see `Makefile`, `types.hpp`, and `state.hpp` for available configuration options.

