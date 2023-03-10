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

#include <cstring>
#include <utility>

Word getword(State& state)
{
    auto word = state.dict.input();
    while (word.size() == 0) {
        state.input(state);
        word = state.dict.input();
    }
    return word;
}
void newdef(State& state, Word word)
{
    auto& dict = state.dict;

    auto addr = dict.alignhere();
    dict.addDefinition(word);
    state.push(addr);
};
void tick(State& state)
{
    auto word = getword(state);
    if (auto j = state.dict.find(word); j > 0) {
        state.push(state.dict.getexec(j));
        auto imm = state.dict.read(j) & CoreWords::Immediate;
        state.push(imm ? 1 : -1);
    } else if (auto i = CoreWords::findi(state, word); i >= 0) {
        state.push(i);
        state.push(i == CoreWords::Semicolon ? 1 : -1);
    } else {
        state.push(0);
        state.push(0);
    }
}

void CoreWords::run(unsigned int index, State& state)
{
    Cell cell;
    DoubleCell dcell;

execute:
    if (index >= Dictionary::Begin) {
        // must be calling a defined subroutine
        state.pushr(state.ip);
        state.ip = index;
        return;
    } else switch (index) {
    case 0: // _lit
        state.push(state.beyondip());
        break;
    case 1: // drop
        state.pop();
        break;
    case 2: // dup
        state.push(state.top());
        break;
    case 3: // swap
        std::swap(state.top(), state.pick(1));
        break;
    case 4: // pick
        state.push(state.pick(state.pop()));
        break;
    case 5: // sys
        user_sys(state);
        break;
    case 6: // add
        cell = state.pop();
        state.top() += cell;
        break;
    case 7: // sub
        cell = state.pop();
        state.top() -= cell;
        break;
    case 8: // mul ( n n -- d )
        cell = state.pop();
        dcell = state.pop() * cell;
        state.push(dcell);
        state.push(dcell >> (sizeof(Cell) * 8));
        break;
    case 9: // div ( d n -- n )
        cell = state.pop();
        dcell = state.pop();
        dcell <<= sizeof(Cell) * 8;
        dcell |= static_cast<Addr>(state.pop());
        state.push(dcell / cell);
        break;
    case 10: // mod ( d n -- n )
        cell = state.pop();
        dcell = state.pop();
        dcell <<= sizeof(Cell) * 8;
        dcell |= static_cast<Addr>(state.pop());
        state.push(dcell % cell);
        break;
    case 11: // peek
        if (state.pop())
            state.push(state.dict.read(state.pop()));
        else
            state.push(state.dict.readbyte(state.pop()));
        break;
    case 12: // poke
        cell = state.pop();
        if (auto addr = state.pop(); cell)
            state.dict.write(addr, state.pop());
        else
            state.dict.writebyte(addr, state.pop());
        break;
    case 13: // pushr
        state.pushr(state.pop());
        break;
    case 14: // popr
        state.push(state.popr());
        break;
    case 15: // equal
        cell = state.pop();
        state.top() = state.top() == cell ? -1 : 0;
        break;
    case 16: // lt
        cell = state.pop();
        state.top() = state.top() < cell ? -1 : 0;
        break;
    case 17: // and
        cell = state.pop();
        state.top() &= cell;
        break;
    case 18: // or
        cell = state.pop();
        state.top() |= cell;
        break;
    case 19: // xor
        cell = state.pop();
        state.top() ^= cell;
        break;
    case 20: // shl
        cell = state.pop();
        reinterpret_cast<Addr&>(state.top()) <<= static_cast<Addr>(cell);
        break;
    case 21: // shr
        cell = state.pop();
        reinterpret_cast<Addr&>(state.top()) >>= static_cast<Addr>(cell);
        break;
    case 22: // colon
        newdef(state, getword(state));
        state.compiling(true);
        break;
    case 23: // tick
        tick(state);
        break;
    case 24: // execute
        index = state.pop();
        goto execute;
    case 25: // exit
        state.ip = state.popr();
        if (state.ip == 0) {
            std::longjmp(state.jmpbuf,
                static_cast<int>(State::Error::exit));
        }
        break;
    case 26: // semic
        {
        state.dict.add(findi("exit"));
        state.compiling(false);

        auto addr = state.pop();
        state.dict.write(addr,
            (state.dict.read(addr) & 0x1F) |
            ((addr - state.dict.latest()) << 6));
        state.dict.latest(addr);
        }
        break;
    case 27: // _jmp0
        if (state.pop()) {
            state.beyondip();
            break;
        }
        [[fallthrough]];
    case 28: // _jmp
        state.ip = state.beyondip();
        return;
    case 29: // depth
        state.push(state.size());
        break;
    case 30: // _rdepth
        state.push(state.rsize());
        break;
    case 31: // _in
        state.input(state);
        break;
    default:
        state.push(index - WordCount);
        break;
    }

    state.ip += sizeof(Cell);
}

template<class Comp>
int findi(Comp comp)
{
    std::size_t i = 0;
    int wordsi = 0;

    while (i < sizeof(CoreWords::wordsarr)) {
        auto end = i;
        while (CoreWords::wordsarr[end])
            ++end;

        if (comp(CoreWords::wordsarr + i, end - i))
            return wordsi;

        ++wordsi;
        i = end + 1;
    }

    return -1;
}

int CoreWords::findi(const char *word)
{
    return ::findi([word](auto b, auto e) {
        return !std::strncmp(word, b, e); });
}

int CoreWords::findi(State& state, Word word)
{
    return ::findi([state, word](auto b, auto e) {
        return state.dict.equal(word, b, e); });
}

