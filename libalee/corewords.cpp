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
#include "parser.hpp"

#include <utility>

void find(State& state, Word word)
{
    Cell tok = 0;
    Cell imm = 0;

    if (auto j = state.dict.find(word); j > 0) {
        tok = state.dict.getexec(j);
        imm = (state.dict.read(j) & Dictionary::Immediate) ? 1 : -1;
    } else if (tok = CoreWords::findi(state, word); tok >= 0) {
        imm = (tok == CoreWords::Semicolon) ? 1 : -1;
    }

    state.push(tok);
    state.push(imm);
}

void CoreWords::run(Cell ins, State& state)
{
    Cell cell;
    DoubleCell dcell;

    Addr index = ins;

    auto& ip = state.ip();

    auto popd = [](State& s) {
        DoubleCell dcell = s.pop();
        dcell <<= sizeof(Cell) * 8;
        dcell |= static_cast<Addr>(s.pop());
        return dcell;
    };

    auto pushd = [](State& s, DoubleCell d) {
        s.push(static_cast<Cell>(d));
        s.push(static_cast<Cell>(d >> (sizeof(Cell) * 8)));
    };

execute:
    if (index >= Dictionary::Begin) {
        // must be calling a defined subroutine
        state.pushr(ip);
        ip = index;
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
        pushd(state, dcell);
        break;
    case 9: // div ( d n -- n )
        cell = state.pop();
        dcell = popd(state);
        state.push(static_cast<Cell>(dcell / cell));
        break;
    case 10: // mod ( d n -- n )
        cell = state.pop();
        dcell = popd(state);
        state.push(static_cast<Cell>(dcell % cell));
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
            state.dict.writebyte(addr, state.pop() & 0xFFu);
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
        state.push(state.dict.alignhere());
        state.dict.write(Dictionary::CompToken, state.top());
        while (!state.dict.hasInput())
            state.input();
        state.dict.addDefinition(state.dict.input());
        state.compiling(true);
        break;
    case 23: // tick
        while (!state.dict.hasInput())
            state.input();
        find(state, state.dict.input());
        break;
    case 24: // execute
        index = state.pop();
        goto execute;
    case 25: // exit
        ip = state.popr();
        state.verify(ip != 0, Error::exit);
        break;
    case 26: // semic
        state.dict.add(findi("exit"));
        state.compiling(false);

        cell = state.pop();
        dcell = cell - state.dict.latest();
        if (dcell > (1 << (sizeof(Cell) * 8 - 6)) - 1) {
            state.dict.write(cell,
                (state.dict.read(cell) & 0x1F) | static_cast<Cell>(((1 << (sizeof(Cell) * 8 - 6)) - 1) << 6));
            state.dict.write(static_cast<Addr>(cell) + sizeof(Cell), static_cast<Cell>(dcell));
        } else {
            state.dict.write(cell,
                (state.dict.read(cell) & 0x1F) | static_cast<Cell>(dcell << 6));
        }
        state.dict.latest(cell);
        break;
    case 27: // _jmp0
        if (state.pop()) {
            state.beyondip();
            break;
        }
        [[fallthrough]];
    case 28: // _jmp
        ip = state.beyondip();
        return;
    case 29: // depth
        state.push(static_cast<Cell>(state.size()));
        break;
    case 30: // _rdepth
        state.push(static_cast<Cell>(state.rsize()));
        break;
    case 31: // _in
        state.input();
        break;
    case 32: // _ex
        {
        const auto st = state.save();
        ip = 0;
        Parser::parseSource(state);
        state.load(st);
        }
        break;
    case 33: // find
        cell = state.pop();
        find(state,
             Word::fromLength(static_cast<Addr>(cell + 1),
                              state.dict.readbyte(cell)));
        break;
    case 34: // _uma
        {
        const auto plus = state.pop();
        cell = state.pop();
        dcell = popd(state);
        dcell *= static_cast<Addr>(cell);
        dcell += static_cast<Addr>(plus);
        pushd(state, dcell);
        }
        break;
    case 35: // u<
        cell = state.pop();
        state.top() = static_cast<Addr>(state.top()) <
                      static_cast<Addr>(cell) ? -1 : 0;
        break;
    case 36: // um/mod
        cell = state.pop();
        dcell = popd(state);

        state.push(static_cast<Cell>(
            static_cast<DoubleAddr>(dcell) %
            static_cast<Addr>(cell)));
        state.push(static_cast<Cell>(
            static_cast<DoubleAddr>(dcell) /
            static_cast<Addr>(cell)));
        break;
    default:
        state.push(ins - WordCount);
        break;
    }

    ip += sizeof(Cell);
}

Cell CoreWords::findi(State& state, Word word)
{
    return findi(word.begin(&state.dict), word.size());
}

