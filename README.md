# Alee Forth

**Still very much in development! Not suitable for real applications yet.**

Alee is a portable and concise Forth implementation in modern C++. Its primary aims are for reduced program size and execution efficiency. Portability includes bare-metal platforms, with intentions to support microcontrollers with kilobytes of memory.

## Cross-platform compatibility

Alee relies on the C++17 standard. Alee *does not* rely on operating-system-specific functions, making portability easy. See the `msp430` target for an example of a port.

System-specific functionality such as text output is contained to a `sys` word. This word calls a user-supplied `user_sys` C++ function that should implement the necessary (or any additional) system-specific functionality.

# Forth compatibility

A base dictionary is being built by working through the "core" and "core extension" [glossaries](https://forth-standard.org/standard/core). These glossaries are listed in `compat.txt`, with "yes" indicating that the word is implemented either in `core.fth` or within Alee itself. `core.fth` may be compiled into a binary for loading on targets without filesystems.

Alee Forth aims for compliance with common Forth standards like Forth 2012 and ANS Forth. Compliance is tested using a [Forth 2012 test suite](https://github.com/gerryjackson/forth2012-test-suite). Supported test files are in the `test` directory, with non-passing or unimplemented tests commented out.

## Building

Alee requires `make` and a C++17-compatible compiler.

To compile, simply run the `make` command. This will produce a library, `libalee.a`, as well as a REPL binary named `alee`.  
A `small` target exists that optimizes the build for size.  
A `fast` target exists that optimizes for maximum performance on the host (not target) system.

