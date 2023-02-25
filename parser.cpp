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

#include <charconv>
#include <cstring>

int Parser::parse(State& state, const char *str)
{
    const auto size = std::strlen(str);

    auto addr = Dictionary::Input;
    state.dict.write(addr, size + 1);

    addr += sizeof(Cell) + Dictionary::InputCells - size - 1;
    while (*str)
        state.dict.writebyte(addr++, *str++);
    state.dict.writebyte(addr, ' ');

    return parseSource(state);
}

int Parser::parseSource(State& state)
{
    auto word = state.dict.input();
    while (word.size() > 0) {
        if (auto ret = parseWord(state, word); ret)
            return ret;

        word = state.dict.input();
    }

    return 0;
}

int Parser::parseWord(State& state, Word word)
{
    int ins, imm;

    ins = state.dict.find(word);

    if (ins <= 0) {
        ins = CoreWords::findi(state, word);

        if (ins < 0) {
            return parseNumber(state, word);
        } else {
            imm = ins & CoreWords::Immediate;
            ins &= ~CoreWords::Immediate;
        }
    } else {
        imm = state.dict.read(ins) & CoreWords::Immediate;
        ins = state.dict.getexec(ins);
    }

    if (state.dict.read(Dictionary::Postpone)) {
        state.dict.add(ins);
        state.dict.write(Dictionary::Postpone, 0);
    } else if (state.compiling() && !imm) {
        state.dict.add(ins);
    } else {
        if (auto stat = state.execute(ins); stat != State::Error::none)
            return static_cast<int>(stat);
    }

    return 0;
}

int Parser::parseNumber(State& state, Word word)
{
    char buf[MaxCellNumberChars + 1];
    unsigned i;
    for (i = 0; i < std::min(MaxCellNumberChars, word.size()); ++i)
        buf[i] = state.dict.readbyte(word.start + i);
    buf[i] = '\0';

    auto base = state.dict.read(0);
    Cell l;
    auto [ptr, ec] = std::from_chars(buf, buf + i, l, base);

    if (ec == std::errc() && ptr == buf + i) {
        if (state.compiling()) {
            state.dict.add(CoreWords::findi("_lit"));
            state.dict.add(l);
        } else {
            state.push(l);
        }

        return 0;
    } else {
        return UnknownWord;
    }
}

