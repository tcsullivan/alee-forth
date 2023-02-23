# Alee Forth

**Still very much in development! Not suitable for real applications yet.**

Alee is a portable and concise Forth implementation in modern C++. Its primary aims are for reduced program size and execution efficiency. Portability includes bare-metal platforms, with intentions to support microcontrollers with kilobytes of memory.

## Compatibility

A base dictionary is being built by working through the "core" and "core extension" [glossaries](https://forth-standard.org/standard/core). These word lists are included in `compat.txt`, with "yes" indicating that the word is implemented either in `core.fth` or within Alee itself.

A `sys` word is available to provide access to user-defined C++ functions.

## Building

Alee requires `make` and a C++17-compatible compiler.

To compile, simply run the `make` command. This will produce a library, `libalee.a`, as well as a REPL binary named `alee`.  
A `small` target exists that optimizes the build for size.

