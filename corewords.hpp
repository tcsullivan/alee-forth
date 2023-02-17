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

void user_sys(State&);

class CoreWords
{
public:
    constexpr static std::size_t VisibleWordCount = 33;             // size
    constexpr static auto HiddenWordLiteral = VisibleWordCount;     // index
    constexpr static auto HiddenWordJump    = VisibleWordCount + 1; // index
    constexpr static auto WordCount         = HiddenWordJump + 1;   // size

    constexpr static Cell Immediate     = (1 << 5);
    constexpr static Cell CoreImmediate = (1 << 6);

    static int findi(State&, Word);
    static Func find(State&, Word);
    static void run(int, State&);

private:
    constexpr static char wordsarr[] =
        "drop\0dup\0swap\0pick\0sys\0"
        "+\0-\0*\0/\0%\0"
        "_@\0_!\0rot\0>r\0r>\0"
        "=\0<\0allot\0&\0|\0"
        "^\0<<\0>>\0(\0:\1"
        ";\1here\0imm\0const\0"
        "if\1then\1else\1depth\0";

    static Func get(int);

    static void op_drop(State&);
    static void op_dup(State&);
    static void op_swap(State&);
    static void op_pick(State&);
    static void op_sys(State&);
    static void op_add(State&);
    static void op_sub(State&);
    static void op_mul(State&);
    static void op_div(State&);
    static void op_mod(State&);
    static void op_peek(State&);
    static void op_poke(State&);
    static void op_rot(State&);
    static void op_pushr(State&);
    static void op_popr(State&);
    static void op_eq(State&);
    static void op_lt(State&);
    static void op_allot(State&);
    static void op_and(State&);
    static void op_or(State&);
    static void op_xor(State&);
    static void op_shl(State&);
    static void op_shr(State&);
    static void op_comment(State&);
    static void op_colon(State&);
    static void op_semic(State&);
    static void op_here(State&);
    static void op_imm(State&);
    static void op_const(State&);
    static void op_literal(State&);
    static void op_jump(State&);
    static void op_if(State&);
    static void op_then(State&);
    static void op_else(State&);
    static void op_depth(State&);
    static void op_key(State&);
    static void op_word(State&);
};

#endif // ALEEFORTH_COREWORDS_HPP

