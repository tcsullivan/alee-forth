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
    auto addr = Dictionary::Input;
    state.dict.write(addr, 0);
    state.dict.write(Dictionary::SourceLen, std::strlen(str));

    addr += sizeof(Cell);
    while (*str)
        state.dict.writebyte(addr++, *str++);

    while (addr < Dictionary::Input + Dictionary::InputCells)
        state.dict.writebyte(addr++, '\0');

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

        if (ins < 0)
            return parseNumber(state, word);
        else
            imm = ins == CoreWords::Semicolon;
    } else {
        imm = state.dict.read(ins) & CoreWords::Immediate;
        ins = state.dict.getexec(ins);
    }

    if (state.compiling() && !imm)
        state.dict.add(ins);
    else if (auto stat = state.execute(ins); stat != State::Error::none)
        return static_cast<int>(stat);

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
    DoubleCell dl;
    auto [ptr, ec] = std::from_chars(buf, buf + i, dl, base);
    Cell l = static_cast<Cell>(dl);

    if (ec == std::errc() && ptr == buf + i) {
        if (state.compiling()) {
            auto ins = CoreWords::findi("_lit");

            //if (l >= 0 && l < 0xFF) {
            //    state.dict.add(ins | ((l + 1) << 8));
            //} else {
                state.dict.add(ins);
                state.dict.add(l);
            //}
        } else {
            state.push(l);
        }

        return 0;
    } else {
        return UnknownWord;
    }
}

