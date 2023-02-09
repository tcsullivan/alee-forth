/**
 * Alee Forth: A portable and concise Forth implementation in modern C++.
 * Copyright (C) 2023  Clyne Sullivan <clyne@bitgloo.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ALEEFORTH_COREWORDS_HPP
#define ALEEFORTH_COREWORDS_HPP

#include "types.hpp"
#include "state.hpp"

int user_sys(State&);

class CoreWords
{
public:
    constexpr static std::size_t VisibleWordCount = 30;  // size
    constexpr static std::size_t HiddenWordLiteral = 30; // index
    constexpr static std::size_t HiddenWordJump = 31;    // index
    constexpr static std::size_t WordCount = 32;         // size

    constexpr static Cell Immediate = (1 << 5);

    static int findi(std::string_view);
    static Func find(std::string_view);
    static void run(int, State&);

private:
    constexpr static char wordsarr[] =
        "drop\0dup\0swap\0pick\0sys\0"
        "+\0-\0*\0/\0%\0"
        "@\0!\0rot\0>r\0r>\0"
        "=\0<\0allot\0&\0|\0"
        "^\0<<\0>>\0(\0:\0"
        ";\0here\0exit\0imm\0const\0";
    // lit, jmp, jmp0, ', lits

    static Func get(int);

    static int op_drop(State&);
    static int op_dup(State&);
    static int op_swap(State&);
    static int op_pick(State&);
    static int op_sys(State&);
    static int op_add(State&);
    static int op_sub(State&);
    static int op_mul(State&);
    static int op_div(State&);
    static int op_mod(State&);
    static int op_peek(State&);
    static int op_poke(State&);
    static int op_rot(State&);
    static int op_pushr(State&);
    static int op_popr(State&);
    static int op_eq(State&);
    static int op_lt(State&);
    static int op_allot(State&);
    static int op_and(State&);
    static int op_or(State&);
    static int op_xor(State&);
    static int op_shl(State&);
    static int op_shr(State&);
    static int op_comment(State&);
    static int op_colon(State&);
    static int op_semic(State&);
    static int op_here(State&);
    static int op_exit(State&);
    static int op_imm(State&);
    static int op_const(State&);
    static int op_literal(State&);
    static int op_jump(State&);
};

#endif // ALEEFORTH_COREWORDS_HPP

