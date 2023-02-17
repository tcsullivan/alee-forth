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
    switch (index) {
    case 0: return op_drop;
    case 1: return op_dup;
    case 2: return op_swap;
    case 3: return op_pick;
    case 4: return op_sys;
    case 5: return op_add;
    case 6: return op_sub;
    case 7: return op_mul;
    case 8: return op_div;
    case 9: return op_mod;
    case 10: return op_peek;
    case 11: return op_poke;
    case 12: return op_rot;
    case 13: return op_pushr;
    case 14: return op_popr;
    case 15: return op_eq;
    case 16: return op_lt;
    case 17: return op_allot;
    case 18: return op_and;
    case 19: return op_or;
    case 20: return op_xor;
    case 21: return op_shl;
    case 22: return op_shr;
    case 23: return op_comment;
    case 24: return op_colon;
    case 25: return op_semic; // :267
    case 26: return op_here;
    case 27: return op_imm;
    case 28: return op_const;
    case 29: return op_if;
    case 30: return op_then;
    case 31: return op_else;
    case 32: return op_depth;
    case 33: return op_literal;
    case 34: return op_jump;
    default: return nullptr;
    }
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

void CoreWords::op_rot(State& state) {
    std::swap(state.pick(2), state.pick(1));
    std::swap(state.pick(1), state.pick(0));
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

void CoreWords::op_comment(State& state) {
    do {
        op_key(state);
    } while (state.pop() != ')');
}

void CoreWords::op_colon(State& state) {
    if (state.compiling()) {
        Word word = state.dict.input();
        while (word.size() == 0) {
            state.input(state);
            word = state.dict.input();
        }

        state.pushr(state.dict.alignhere());
        state.dict.addDefinition(word);
    }

}

void CoreWords::op_semic(State& state) {
    if (!state.compiling()) {
        state.ip = state.popr();
    } else {
        auto begin = state.popr();

        state.dict.write(begin,
            (state.dict.read(begin) & 0x1F) |
            ((begin - state.dict.latest) << 6));

        state.dict.latest = begin;
        state.compiling(false);
    }

}

void CoreWords::op_here(State& state) {
    state.push(state.dict.here);
}

void CoreWords::op_imm(State& state)
{
    state.dict.write(state.dict.latest,
        state.dict.read(state.dict.latest) | Immediate);
}

void CoreWords::op_const(State& state)
{
    if (state.compiling()) {
        Word word = state.dict.input();
        while (word.size() == 0) {
            state.input(state);
            word = state.dict.input();
        }

        state.pushr(state.dict.alignhere());
        state.dict.addDefinition(word);
        state.dict.add(CoreWords::HiddenWordLiteral);
        state.dict.add(state.pop());
        state.dict.add(25 | CoreImmediate);
        op_semic(state);
        state.compiling(false);
    }

}

void CoreWords::op_literal(State& state)
{
    state.push(state.beyondip());
    state.ip += sizeof(Cell);
}

void CoreWords::op_jump(State& state)
{
    state.pushr(state.ip + sizeof(Cell));
    state.ip = state.beyondip() - sizeof(Cell);
}

void CoreWords::op_if(State& state)
{
    if (state.compiling()) {
        state.push(state.dict.here);
        state.dict.add(0);
    } else {
        if (state.pop())
            state.ip += sizeof(Cell);
        else
            state.ip = state.beyondip() - sizeof(Cell);
    }

}

void CoreWords::op_then(State& state)
{
    if (state.compiling()) {
        const auto ifaddr = state.pop();
        if (state.dict.read(ifaddr) == 0)
            state.dict.write(ifaddr, state.dict.here);
    }

}

void CoreWords::op_else(State& state)
{
    if (state.compiling()) {
        const auto ifaddr = state.pop();
        state.push(state.dict.here);
        state.dict.add(0);
        state.dict.write(ifaddr, state.dict.here);
    } else {
        state.ip = state.beyondip() - sizeof(Cell);
    }

}

void CoreWords::op_depth(State& state)
{
    state.push(state.size());
}

void CoreWords::op_key(State& state)
{
    auto len = state.dict.read(Dictionary::Input);
    while (len <= 0)
        state.input(state);

    state.dict.write(Dictionary::Input, len - 1);
    Addr addr = Dictionary::Input + sizeof(Cell) +
                Dictionary::InputCells - len;
    Cell val = state.dict.readbyte(addr);

    state.push(val);
}

int CoreWords::findi(State& state, Word word)
{
    std::size_t i;
    int wordsi = 0;

    std::string_view words (wordsarr, sizeof(wordsarr));

    for (i = 0; i < words.size();) {
        const auto end = words.find_first_of({"\0\1", 2}, i);

        if (state.dict.equal(word, words.substr(i, end - i)))
            return words[end] == '\0' ? wordsi : (wordsi | CoreImmediate);

        ++wordsi;
        i = end + 1;
    }

    return -1;
}

Func CoreWords::find(State& state, Word word)
{
    const auto i = findi(state, word);
    return i >= 0 ? get(i & ~CoreWords::CoreImmediate) : nullptr;
}

void CoreWords::run(int i, State& state)
{
    i &= ~CoreWords::CoreImmediate;

    if (i >= 0 && i < WordCount)
        get(i)(state);
}

