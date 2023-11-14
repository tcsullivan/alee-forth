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

Error (*Parser::customParse)(State&, Word) = nullptr;

LIBALEE_SECTION
Error Parser::parse(State& state, const char *str)
{
    auto addr = Dictionary::Input;

    // Set source and input length
    const auto len = static_cast<Cell>(strlen(str));
    state.dict.write(addr, 0);
    state.dict.write(Dictionary::SourceLen, len);

    // Fill input buffer with string contents
    addr += sizeof(Cell);
    while (*str)
        state.dict.writebyte(addr++, *str++);

    // Zero the remaining input buffer
    while (addr < Dictionary::Input + Dictionary::InputCells)
        state.dict.writebyte(addr++, '\0');

    return parseSource(state);
}

LIBALEE_SECTION
Error Parser::parseSource(State& state)
{
    auto err = Error::none;

    while (err == Error::none && state.dict.hasInput())
        err = parseWord(state, state.dict.input());

    return err;
}

LIBALEE_SECTION
Error Parser::parseWord(State& state, Word word)
{
    bool imm;
    Addr ins;

    // Search order: dictionary, core word-set, number, custom parse.
    ins = state.dict.find(word);
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
            imm = ins == CoreWords::token(";");
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

LIBALEE_SECTION
Error Parser::parseNumber(State& state, Word word)
{
    const auto base = state.dict.read(Dictionary::Base);
    DoubleCell result = 0;
    auto it = word.begin(&state.dict);

    bool inv = *it == '-';
    if (inv)
        ++it;

    const auto end = word.end(&state.dict);
    for (uint8_t c; it != end; ++it) {
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

LIBALEE_SECTION
void Parser::processLiteral(State& state, Cell value)
{
    if (state.compiling()) {
        constexpr auto ins = CoreWords::token("_lit");

        // Literal compression: opcodes between WordCount and Begin are unused,
        // so we assign literals to them to save space. Opcode "WordCount"
        // pushes zero to the stack, "WordCount + 1" pushes a one, etc.
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

