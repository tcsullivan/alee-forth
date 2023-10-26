# Alee Forth

Alee Forth is a concise Forth implementation written in modern C++ that aims for portability, minimal memory footprint, and execution efficiency.

## Cross-platform compatibility

Alee Forth relies on the C++20 standard. It *does not* rely on any operating system. As a result, portability extends down to microcontroller targets with < 16kB flash and < 1 kB of RAM. See the `msp430` folder for an example of such a port.

System-specific functionality is achieved through a `sys` Forth word. This word calls a user-supplied C++ function that implements whatever functionality is needed.

## Forth compatibility

Alee Forth uses the [Forth 2012 test suite](https://github.com/gerryjackson/forth2012-test-suite) to ensure standards compliance. The entire "core" [word-set](https://forth-standard.org/standard/core) is implemented as well as most of the "core extension" word-set. The compiled program contains a minimal set of fundamental words with libraries in the `forth` directory supplying these larger word-sets. The "core" word-set can be compiled into the program by building the `standalone` target.

**Missing** core extension words:

```
PARSE PARSE-NAME REFILL RESTORE-INPUT S\" SAVE-INPUT SOURCE-ID [COMPILE]
```

## Building

Alee requires `make` and a compiler that supports C++20. Simply running `make` will produce the `libalee.a` library and a REPL binary named `alee`. The core word-sets can be passed into `alee` via the command line: `./alee forth/core.fth forth/core-ext.fth`.

Other available build targets:

* `small`: Optimize for minimal binary size.
* `fast`: Optimize for maximum performance on the host system.
* `standalone`: Builds the core dictionary (`core.fth`) into the binary.
* `msp430-prep` and `msp430`: Builds a binary for the [MSP430G2553](https://www.ti.com/product/MSP430G2553) microcontroller. See the `msp430` folder for more information.

If building for a new platform, review these files: `Makefile`, `libalee/types.hpp`, and `libalee/state.hpp`. It is possible to modify the implementation to use 32-bit words, but this will require re-writing the core word-sets.

