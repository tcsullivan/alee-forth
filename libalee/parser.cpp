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
#include "ctype.hpp"
#include "parser.hpp"

#include <algorithm>
#include <cstring>

Error (*Parser::customParse)(State&, Word) = nullptr;

Error Parser::parse(State& state, const char *str)
{
    auto addr = Dictionary::Input;

    const auto len = static_cast<Cell>(std::strlen(str));
    state.dict.write(addr, 0);
    state.dict.write(Dictionary::SourceLen, len);

    addr += sizeof(Cell);
    while (*str)
        state.dict.writebyte(addr++, *str++);

    while (addr < Dictionary::Input + Dictionary::InputCells)
        state.dict.writebyte(addr++, '\0');

    return parseSource(state);
}

Error Parser::parseSource(State& state)
{
    auto err = Error::none;

    while (err == Error::none && state.dict.hasInput())
        err = parseWord(state, state.dict.input());

    return err;
}

Error Parser::parseWord(State& state, Word word)
{
    bool imm;
    Addr ins = state.dict.find(word);

    if (ins == 0) {
        auto cw = CoreWords::findi(state, word);

        if (cw < 0) {
            auto r = parseNumber(state, word);
            if (r != Error::none)
                return customParse ? customParse(state, word) : r;
            else
                return r;
        } else {
            ins = cw;
            imm = ins == CoreWords::Semicolon;
        }
    } else {
        imm = state.dict.read(ins) & Dictionary::Immediate;
        ins = state.dict.getexec(ins);
    }

    if (state.compiling() && !imm)
        state.dict.add(ins);
    else if (auto stat = state.execute(ins); stat != Error::none)
        return stat;

    return Error::none;
}

Error Parser::parseNumber(State& state, Word word)
{
    const auto base = state.dict.read(Dictionary::Base);
    DoubleCell result = 0;
    auto it = word.begin(&state.dict);

    bool inv = *it == '-';
    if (inv)
        ++it;

    const auto end = word.end(&state.dict);
    for (char c; it != end; ++it) {
        c = *it;

        if (isdigit(c)) {
            result *= base;
            result += c - '0';
        } else if (isalpha(c) && base > 10) {
            result *= base;
            result += 10 + c - (isupper(c) ? 'A' : 'a');
        } else {
            return Error::noword;
        }
    }

    if (inv)
        result *= -1;

    processLiteral(state, static_cast<Cell>(result));
    return Error::none;
}

void Parser::processLiteral(State& state, Cell value)
{
    if (state.compiling()) {
        constexpr auto ins = CoreWords::findi("_lit");

        const Cell maxlit = Dictionary::Begin - CoreWords::WordCount;
        if (value >= 0 && value < maxlit)
            value += CoreWords::WordCount;
        else
            state.dict.add(ins);

        state.dict.add(value);
    } else {
        state.push(value);
    }
}

