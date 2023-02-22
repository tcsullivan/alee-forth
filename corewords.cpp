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

#include "corewords.hpp"

Func CoreWords::get(int index)
{
    static const Func ops[WordCount] = {
        op_drop,  op_dup,   op_swap,  op_pick,    op_sys,
        op_add,   op_sub,   op_mul,   op_div,     op_mod,
 /*10*/ op_peek,  op_poke,  op_pushr, op_popr,    op_eq,
        op_lt,    op_allot, op_and,   op_or,      op_xor,
 /*20*/ op_shl,   op_shr,   op_colon, op_semic,   op_here,
        op_depth, op_key,   op_exit,  op_tick,    op_execute,
 /*30*/ op_jmp,   op_jmp0,  op_lit,   op_literal, op_rdepth
    };

    return index >= 0 && index < WordCount ? ops[index] : nullptr;
}

void CoreWords::op_drop(State& state)
{
    state.pop();
}

void CoreWords::op_dup(State& state)
{
    state.push(state.top());
}

void CoreWords::op_swap(State& state)
{
    std::swap(state.top(), state.pick(1));
}

void CoreWords::op_pick(State& state)
{
    state.push(state.pick(state.pop()));
}

void CoreWords::op_sys(State& state)
{
    return user_sys(state);
}

void CoreWords::op_add(State& state)
{
    const auto a = state.pop();
    state.top() += a;
}

void CoreWords::op_sub(State& state)
{
    const auto a = state.pop();
    state.top() -= a;
}

void CoreWords::op_mul(State& state) {
    const auto a = state.pop();
    state.top() *= a;
}

void CoreWords::op_div(State& state) {
    const auto a = state.pop();
    state.top() /= a;
}

void CoreWords::op_mod(State& state) {
    const auto a = state.pop();
    state.top() %= a;
}

void CoreWords::op_peek(State& state) {
    if (auto w = state.pop(); w == 1)
        state.push(state.dict.readbyte(state.pop()));
    else
        state.push(state.dict.read(state.pop()));
}

void CoreWords::op_poke(State& state) {
    const auto w = state.pop();
    const auto addr = state.pop();
    if (w == 1)
        state.dict.writebyte(addr, state.pop());
    else
        state.dict.write(addr, state.pop());
}

void CoreWords::op_pushr(State& state) {
    state.pushr(state.pop());
}

void CoreWords::op_popr(State& state) {
    state.push(state.popr());
}

void CoreWords::op_eq(State& state) {
    const auto a = state.pop();
    state.top() = state.top() == a;
}

void CoreWords::op_lt(State& state) {
    const auto a = state.pop();
    state.top() = state.top() < a;
}

void CoreWords::op_allot(State& state) {
    state.dict.allot(state.pop());
}

void CoreWords::op_and(State& state) {
    const auto a = state.pop();
    state.top() &= a;
}

void CoreWords::op_or(State& state) {
    const auto a = state.pop();
    state.top() |= a;
}

void CoreWords::op_xor(State& state) {
    const auto a = state.pop();
    state.top() ^= a;
}

void CoreWords::op_shl(State& state) {
    const auto a = state.pop();
    state.top() <<= a;
}

void CoreWords::op_shr(State& state) {
    const auto a = state.pop();
    state.top() >>= a;
}

void CoreWords::op_colon(State& state) {
    Word word = state.dict.input();
    while (word.size() == 0) {
        state.input(state);
        word = state.dict.input();
    }

    const auto start = state.dict.alignhere();
    state.dict.addDefinition(word);
    state.dict.write(start,
        (state.dict.read(start) & 0x1F) |
        ((start - state.dict.latest()) << 6));
    state.dict.latest(start);
    state.compiling(true);
}

void CoreWords::op_tick(State& state) {
    Word word = state.dict.input();
    while (word.size() == 0) {
        state.input(state);
        word = state.dict.input();
    }

    Cell xt = 0;
    if (auto i = CoreWords::findi(state, word); i >= 0) {
        xt = i & ~CoreWords::Compiletime;
    } else if (auto j = state.dict.find(word); j > 0) {
        xt = state.dict.getexec(j);
    }

    state.push(xt);
}

void CoreWords::op_execute(State& state) {
    state.execute(state.pop());
}

void CoreWords::op_exit(State& state) {
    state.ip = state.popr();
}

void CoreWords::op_semic(State& state) {
    if (state.compiling()) {
        state.dict.add(findi("exit"));
        state.compiling(false);
    }
}

void CoreWords::op_here(State& state) {
    state.push(state.dict.here);
}

void CoreWords::op_lit(State& state)
{
    state.push(state.beyondip());
    state.ip += sizeof(Cell);
}

void CoreWords::op_literal(State& state)
{
    if (state.compiling()) {
        state.dict.add(findi("_lit"));
        state.dict.add(state.pop());
    }
}

void CoreWords::op_jmp(State& state)
{
    state.ip = state.beyondip() - sizeof(Cell);
}

void CoreWords::op_jmp0(State& state)
{
    if (state.pop())
        state.ip += sizeof(Cell);
    else
        op_jmp(state);
}

void CoreWords::op_depth(State& state)
{
    state.push(state.size());
}

void CoreWords::op_rdepth(State& state)
{
    state.push(state.rsize());
}

void CoreWords::op_key(State& state)
{
    auto len = state.dict.read(Dictionary::Input);
    while (len <= 0) {
        state.input(state);
        len = state.dict.read(Dictionary::Input);
    }

    state.dict.write(Dictionary::Input, len - 1);
    Addr addr = Dictionary::Input + sizeof(Cell) +
                Dictionary::InputCells - len;
    Cell val = state.dict.readbyte(addr);

    state.push(val);
}

int CoreWords::findi(std::string_view word)
{
    std::size_t i;
    int wordsi = 0;

    std::string_view words (wordsarr, sizeof(wordsarr));

    for (i = 0; i < words.size();) {
        const auto end = words.find_first_of({"\0\1", 2}, i);

        if (word == words.substr(i, end - i))
            return words[end] == '\0' ? wordsi : (wordsi | Compiletime);

        ++wordsi;
        i = end + 1;
    }

    return -1;
}

int CoreWords::findi(State& state, Word word)
{
    std::size_t i;
    int wordsi = 0;

    std::string_view words (wordsarr, sizeof(wordsarr));

    for (i = 0; i < words.size();) {
        const auto end = words.find_first_of({"\0\1", 2}, i);

        if (state.dict.equal(word, words.substr(i, end - i)))
            return words[end] == '\0' ? wordsi : (wordsi | Compiletime);

        ++wordsi;
        i = end + 1;
    }

    return -1;
}

Func CoreWords::find(State& state, Word word)
{
    const auto i = findi(state, word);
    return i >= 0 ? get(i & ~Compiletime) : nullptr;
}

void CoreWords::run(int i, State& state)
{
    if (i >= 0 && i < WordCount)
        get(i)(state);
}

