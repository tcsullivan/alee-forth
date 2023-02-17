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

ParseStatus Parser::parse(State& state, std::string_view& str)
{
    auto addr = Dictionary::Input;
    state.dict.write(addr, str.size() + 1);

    addr += sizeof(Cell) + Dictionary::InputCells - str.size() - 1;
    for (char c : str)
        state.dict.writebyte(addr++, c);
    state.dict.writebyte(addr, '\0');

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
    if (auto i = CoreWords::findi(state, word); i >= 0) {
        if (state.compiling())
            state.dict.add(i & ~CoreWords::CoreImmediate);
        else if (state.dict.equal(word, ":"))
            state.compiling(true);

        if (!state.compiling() || (i & CoreWords::CoreImmediate))
            CoreWords::run(i & ~CoreWords::CoreImmediate, state);
    } else if (auto j = state.dict.find(word); j > 0) {
        auto e = state.dict.getexec(j);

        if (state.compiling()) {
            if (state.dict.read(j) & CoreWords::Immediate) {
                state.compiling(false);
                state.execute(e);
                state.compiling(true);
            } else {
                state.dict.add(CoreWords::HiddenWordJump);
                state.dict.add(e);
            }
        } else {
            state.execute(e);
        }
    } else {
        char buf[word.size() + 1];
        for (unsigned i = 0; i < word.size(); ++i)
            buf[i] = state.dict.readbyte(word.start + i);
        buf[word.size()] = '\0';

        char *p;
        const auto base = state.dict.read(0);
        const Cell l = std::strtol(buf, &p, base);

        if (std::distance(buf, p) == word.size()) {
            if (state.compiling()) {
                state.dict.add(CoreWords::HiddenWordLiteral);
                state.dict.add(l);
            } else {
                state.push(l);
            }
        } else {
            return ParseStatus::NotAWord;
        }
    }

    return ParseStatus::Finished;
}

