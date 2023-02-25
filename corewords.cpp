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

void CoreWords::run(unsigned int index, State& state)
{
    auto getword = [&state] {
        auto word = state.dict.input();
        while (word.size() == 0) {
            state.input(state);
            word = state.dict.input();
        }
        return word;
    };
    auto newdef = [](Dictionary& dict, Word word) {
        auto addr = dict.alignhere();
        dict.addDefinition(word);
        dict.write(addr,
            (dict.read(addr) & 0x1F) |
            ((addr - dict.latest()) << 6));
        dict.latest(addr);
    };
    auto tick = [&state](Word word) {
        if (auto j = state.dict.find(word); j > 0)
            state.push(state.dict.getexec(j));
        else if (auto i = CoreWords::findi(state, word); i >= 0)
            state.push(i & ~CoreWords::Compiletime);
        else
            state.push(0);
    };

    Cell cell;
    DoubleCell dcell;

    switch (index) {
    default:
        // must be calling a defined subroutine
        state.pushr(state.ip);
        state.ip = index - sizeof(Cell);
        break;
    case 0: // drop
        state.pop();
        break;
    case 1: // dup
        state.push(state.top());
        break;
    case 2: // swap
        std::swap(state.top(), state.pick(1));
        break;
    case 3: // pick
        state.push(state.pick(state.pop()));
        break;
    case 4: // sys
        user_sys(state);
        break;
    case 5: // add
        cell = state.pop();
        state.top() += cell;
        break;
    case 6: // sub
        cell = state.pop();
        state.top() -= cell;
        break;
    case 7: // mul ( n n -- d )
        cell = state.pop();
        dcell = state.pop() * cell;
        state.push(dcell);
        state.push(dcell >> (sizeof(Cell) * 8));
        break;
    case 8: // div ( d n -- n )
        cell = state.pop();
        dcell = state.pop() << (sizeof(Cell) * 8);
        dcell |= state.pop();
        state.push(dcell / cell);
        break;
    case 9: // mod ( d n -- n )
        cell = state.pop();
        dcell = state.pop() << (sizeof(Cell) * 8);
        dcell |= state.pop();
        state.push(dcell % cell);
        break;
    case 10: // peek
        if (state.pop())
            state.push(state.dict.read(state.pop()));
        else
            state.push(state.dict.readbyte(state.pop()));
        break;
    case 11: // poke
        cell = state.pop();
        if (auto addr = state.pop(); cell)
            state.dict.write(addr, state.pop());
        else
            state.dict.writebyte(addr, state.pop());
        break;
    case 12: // pushr
        state.pushr(state.pop());
        break;
    case 13: // popr
        state.push(state.popr());
        break;
    case 14: // equal
        cell = state.pop();
        state.top() = state.top() == cell;
        break;
    case 15: // lt
        cell = state.pop();
        state.top() = state.top() < cell;
        break;
    case 16: // allot
        state.dict.allot(state.pop());
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
        state.top() <<= cell;
        break;
    case 21: // shr
        cell = state.pop();
        state.top() >>= cell;
        break;
    case 22: // colon
        newdef(state.dict, getword());
        state.compiling(true);
        break;
    case 23: // tick
        tick(getword());
        break;
    case 24: // execute
        run(state.pop(), state);
        break;
    case 25: // exit
        state.ip = state.popr();
        break;
    case 26: // semic
        state.dict.add(findi("exit"));
        state.compiling(false);
        break;
    case 27: // here
        state.push(state.dict.here);
        break;
    case 28: // _lit
        state.push(state.beyondip());
        break;
    case 29: // literal
        state.dict.add(findi("_lit"));
        state.dict.add(state.pop());
        break;
    case 30: // _jmp
        state.ip = state.beyondip() - sizeof(Cell);
        break;
    case 31: // _jmp0
        if (state.pop())
            state.beyondip();
        else
            state.ip = state.beyondip() - sizeof(Cell);
        break;
    case 32: // depth
        state.push(state.size());
        break;
    case 33: // _rdepth
        state.push(state.rsize());
        break;
    case 34: // key
        cell = state.dict.read(Dictionary::Input);
        while (cell <= 0) {
            state.input(state);
            cell = state.dict.read(Dictionary::Input);
        }

        state.dict.write(Dictionary::Input, cell - 1);

        state.push(
            state.dict.readbyte(
                Dictionary::Input + sizeof(Cell) +
                Dictionary::InputCells - cell));
        break;
    }
}

int CoreWords::findi(const char *word)
{
    const auto size = std::strlen(word);
    std::size_t i;
    int wordsi = 0;

    for (i = 0; i < sizeof(wordsarr);) {
        auto end = i;
        while (wordsarr[end] > '\1')
            ++end;

        if (size == end - i && !std::strncmp(word, wordsarr + i, size))
            return wordsarr[end] == '\0' ? wordsi : (wordsi | Compiletime);

        ++wordsi;
        i = end + 1;
    }

    return -1;
}

int CoreWords::findi(State& state, Word word)
{
    std::size_t i;
    int wordsi = 0;

    for (i = 0; i < sizeof(wordsarr);) {
        auto end = i;
        while (wordsarr[end] > '\1')
            ++end;

        if (state.dict.equal(word, wordsarr + i, end - i))
            return wordsarr[end] == '\0' ? wordsi : (wordsi | Compiletime);

        ++wordsi;
        i = end + 1;
    }

    return -1;
}

