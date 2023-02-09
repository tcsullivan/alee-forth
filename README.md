# Alee Forth

**Still very much in development! Not suitable for real applications yet.**

Alee is a portable and concise Forth implementation in modern C++. Its primary aims are for reduced program size and execution efficiency. Portability includes bare-metal platforms, with intentions to support microcontrollers with kilobytes of memory.

## Built-in words

```
drop dup swap pick rot >r r>
here allot const imm @ ! : ; (
+ - * / %
= <
& | ^ << >>
sys
```

The `sys` word links to a C++ function for user-defined functionality.

## Building

Alee requires `make` and a C++17-compatible compiler.

To compile, simply run the `make` command. The resulting binary, `alee`, runs a read-eval-print loop.

