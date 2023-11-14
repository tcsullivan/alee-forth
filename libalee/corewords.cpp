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

#include "alee.hpp"

#include <utility>

static void find(State&, Word);
static DoubleCell popd(State&);
static void pushd(State&, DoubleCell);

LIBALEE_SECTION
void CoreWords::run(Cell ins, State& state)
{
    Cell cell;
    DoubleCell dcell;

    Addr index = ins;
    auto& ip = state.ip();

execute:
    if (index >= Dictionary::Begin) {
        // must be calling a defined subroutine
        state.pushr(ip);
        ip = index;
        return;
    } else switch (index) {
    case token("_lit"): // Execution semantics of `literal`.
        state.push(state.beyondip());
        break;
    case token("drop"):
        state.pop();
        break;
    case token("dup"):
        state.push(state.top());
        break;
    case token("swap"):
        std::swap(state.top(), state.pick(1));
        break;
    case token("pick"):
        state.push(state.pick(state.pop()));
        break;
    case token("sys"): // Calls user-defined "system" handler.
        user_sys(state);
        break;
    case token("+"):
        cell = state.pop();
        state.top() += cell;
        break;
    case token("-"):
        cell = state.pop();
        state.top() -= cell;
        break;
    case token("m*"): // ( n n -- d )
        cell = state.pop();
        dcell = state.pop() * cell;
        pushd(state, dcell);
        break;
    case token("_/"): // ( d n -- n )
        cell = state.pop();
        dcell = popd(state);
        state.push(static_cast<Cell>(dcell / cell));
        break;
    case token("_%"): // ( d n -- n )
        cell = state.pop();
        dcell = popd(state);
        state.push(static_cast<Cell>(dcell % cell));
        break;
    case token("_@"): // ( addr cell? -- n )
        if (state.pop())
            state.push(state.dict.read(state.pop()));
        else
            state.push(state.dict.readbyte(state.pop()));
        break;
    case token("_!"): // ( n addr cell? -- )
        cell = state.pop();
        if (auto addr = state.pop(); cell)
            state.dict.write(addr, state.pop());
        else
            state.dict.writebyte(addr, state.pop() & 0xFFu);
        break;
    case token(">r"):
        state.pushr(state.pop());
        break;
    case token("r>"):
        state.push(state.popr());
        break;
    case token("="):
        cell = state.pop();
        state.top() = state.top() == cell ? -1 : 0;
        break;
    case token("<"):
        cell = state.pop();
        state.top() = state.top() < cell ? -1 : 0;
        break;
    case token("&"):
        cell = state.pop();
        state.top() &= cell;
        break;
    case token("|"):
        cell = state.pop();
        state.top() |= cell;
        break;
    case token("^"):
        cell = state.pop();
        state.top() ^= cell;
        break;
    case token("<<"):
        cell = state.pop();
        reinterpret_cast<Addr&>(state.top()) <<= static_cast<Addr>(cell);
        break;
    case token(">>"):
        cell = state.pop();
        reinterpret_cast<Addr&>(state.top()) >>= static_cast<Addr>(cell);
        break;
    case token(":"): // Begins definition/compilation of new word.
        state.push(state.dict.alignhere());
        state.dict.write(Dictionary::CompToken, state.top());
        while (!state.dict.hasInput())
            state.input();
        state.dict.addDefinition(state.dict.input());
        state.compiling(true);
        break;
    case token("_'"): // Collects input word and finds execution token.
        while (!state.dict.hasInput())
            state.input();
        find(state, state.dict.input());
        break;
    case token("execute"):
        index = state.pop();
        goto execute;
    case token("exit"):
        ip = state.popr();
        state.verify(ip != 0, Error::exit);
        break;
    case token(";"): // Concludes word definition.
        state.dict.add(token("exit"));
        state.compiling(false);

        cell = state.pop();
        dcell = cell - state.dict.latest();
        if (dcell >= Dictionary::MaxDistance) {
            // Large distance to previous entry: store in dedicated cell.
            state.dict.write(static_cast<Addr>(cell) + sizeof(Cell),
                static_cast<Cell>(dcell));
            dcell = Dictionary::MaxDistance;
        }
        state.dict.write(cell,
            (state.dict.read(cell) & 0x1F) | static_cast<Cell>(dcell << 6));
        state.dict.latest(cell);
        break;
    case token("_jmp0"): // Jump if popped value equals zero.
        if (state.pop()) {
            state.beyondip();
            break;
        }
        [[fallthrough]];
    case token("_jmp"): // Unconditional jump.
        ip = state.beyondip();
        return;
    case token("depth"):
        state.push(static_cast<Cell>(state.size()));
        break;
    case token("_rdepth"):
        state.push(static_cast<Cell>(state.rsize()));
        break;
    case token("_in"): // Fetches more input from the user input source.
        state.input();
        break;
    case token("_ev"): // Evaluates words from current input source.
        {
        const auto st = state.save();
        ip = 0;
        Parser::parseSource(state);
        state.load(st);
        }
        break;
    case token("find"):
        cell = state.pop();
        find(state,
             Word::fromLength(static_cast<Addr>(cell + 1),
                              state.dict.readbyte(cell)));
        break;
    case token("_uma"): // ( d u u -- d ): Unsigned multiply-add.
        {
        const auto plus = state.pop();
        cell = state.pop();
        dcell = popd(state);
        dcell *= static_cast<Addr>(cell);
        dcell += static_cast<Addr>(plus);
        pushd(state, dcell);
        }
        break;
    case token("u<"):
        cell = state.pop();
        state.top() = static_cast<Addr>(state.top()) <
                      static_cast<Addr>(cell) ? -1 : 0;
        break;
    case token("um/mod"):
        cell = state.pop();
        dcell = popd(state);

        state.push(static_cast<Cell>(
            static_cast<DoubleAddr>(dcell) %
            static_cast<Addr>(cell)));
        state.push(static_cast<Cell>(
            static_cast<DoubleAddr>(dcell) /
            static_cast<Addr>(cell)));
        break;
    default: // Compacted literals (WordCount <= ins < Begin).
        state.push(ins - WordCount);
        break;
    }

    ip += sizeof(Cell);
}

LIBALEE_SECTION
Cell CoreWords::findi(State& state, Word word)
{
    return findi(word.begin(&state.dict), word.size());
}

LIBALEE_SECTION
void find(State& state, Word word)
{
    Cell tok = 0;
    Cell imm = 0;

    if (auto j = state.dict.find(word); j > 0) {
        tok = state.dict.getexec(j);
        imm = (state.dict.read(j) & Dictionary::Immediate) ? 1 : -1;
    } else if (tok = CoreWords::findi(state, word); tok >= 0) {
        imm = (tok == CoreWords::token(";")) ? 1 : -1;
    }

    state.push(tok);
    state.push(imm);
}

DoubleCell popd(State& s)
{
    DoubleCell dcell = s.pop();
    dcell <<= sizeof(Cell) * 8;
    dcell |= static_cast<Addr>(s.pop());
    return dcell;
}

void pushd(State& s, DoubleCell d)
{
    s.push(static_cast<Cell>(d));
    s.push(static_cast<Cell>(d >> (sizeof(Cell) * 8)));
}

