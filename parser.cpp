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

#include <cctype>
#include <cstdlib>

#include <iostream>

ParseStatus Parser::parse(State& state, std::string_view& str)
{
    auto addr = Dictionary::Input;
    state.dict.write(addr, str.size() + 1);

    addr += sizeof(Cell) + Dictionary::InputCells - str.size() - 1;
    for (char c : str)
        state.dict.writebyte(addr++, c);
    state.dict.writebyte(addr, ' ');

    return parseSource(state);
}

ParseStatus Parser::parseSource(State& state)
{
    auto word = state.dict.input();
    while (word.size() > 0) {
        if (auto ret = parseWord(state, word); ret != ParseStatus::Finished)
            return ret;

        word = state.dict.input();
    }

    return ParseStatus::Finished;
}

ParseStatus Parser::parseWord(State& state, Word word)
{
    int ins, imm;

    ins = CoreWords::findi(state, word);
    if (ins < 0) {
        ins = state.dict.find(word);

        if (ins <= 0) {
            return parseNumber(state, word);
        } else {
            imm = state.dict.read(ins) & CoreWords::Immediate;
            ins = state.dict.getexec(ins);
        }
    } else {
        imm = ins & CoreWords::Compiletime;
        ins &= ~CoreWords::Compiletime;
    }

    if (state.dict.read(Dictionary::Postpone)) {
        state.dict.add(ins);
        state.dict.write(Dictionary::Postpone, 0);
    } else if (state.compiling() && !imm) {
        state.dict.add(ins);
    } else {
        state.execute(ins);
    }

    return ParseStatus::Finished;
}

ParseStatus Parser::parseNumber(State& state, Word word)
{
    char buf[word.size() + 1];
    for (unsigned i = 0; i < word.size(); ++i)
        buf[i] = state.dict.readbyte(word.start + i);
    buf[word.size()] = '\0';

    char *p;
    const auto base = state.dict.read(0);
    const Cell l = std::strtol(buf, &p, base);

    if (std::distance(buf, p) == word.size()) {
        if (state.compiling()) {
            state.dict.add(CoreWords::findi("_lit"));
            state.dict.add(l);
        } else {
            state.push(l);
        }

        return ParseStatus::Finished;
    } else {
        std::cout << "word not found: " << buf << std::endl;
        return ParseStatus::NotAWord;
    }
}

