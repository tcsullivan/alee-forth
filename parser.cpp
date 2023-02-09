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
#include "executor.hpp"
#include "parser.hpp"

#include <cstdlib>

ParseStatus Parser::parse(State& state, std::string_view& str)
{
    const auto end = str.find_first_of(" \t\n\r");
    const auto sub = str.substr(0, end);
    if (sub.empty())
        return ParseStatus::Finished;

    if (state.pass != Pass::None) {
        switch (state.pass) {
        case Pass::Comment:
            if (str.front() == ')')
                state.pass = Pass::None;

            str = str.substr(1);
            break;
        case Pass::Colon:
            state.pass = Pass::None;
                state.compiling = true;
            state.dict.addDefinition(sub);
            break;
        case Pass::Constant:
            state.pass = Pass::None;
            state.compiling = true;
            state.dict.addDefinition(sub);
            state.dict.add(CoreWords::HiddenWordLiteral);
            state.dict.add(state.pop());
            state.dict.add(CoreWords::findi(";"));
            CoreWords::run(CoreWords::findi(";"), state);
            break;
        default:
            break;
        }
    } else {
        if (auto i = CoreWords::findi(sub); i >= 0) {
            if (state.compiling)
                state.dict.add(i);
            if (!state.compiling || sub.front() == ';')
                CoreWords::run(i, state);
        } else if (auto j = state.dict.find(sub); j > 0) {
            auto e = state.dict.getexec(j);

            if (state.compiling) {
                if (state.dict.read(j) & CoreWords::Immediate) {
                    state.compiling = false;
                    Executor::fullexec(state, e);
                    state.compiling = true;
                } else {
                    state.dict.add(CoreWords::HiddenWordJump);
                    state.dict.add(e);
                }
            } else {
                Executor::fullexec(state, e);
            }
        } else {
            char *p;
            const auto base = state.dict.read(0);
            const auto l = static_cast<Cell>(std::strtol(sub.data(), &p, base));

            if (p != sub.data()) {
                if (state.compiling) {
                    state.dict.add(CoreWords::HiddenWordLiteral);
                    state.dict.add(l);
                } else {
                    state.push(l);
                }
            } else {
                return ParseStatus::Error;
            }
        }

        if (end == std::string_view::npos)
            return ParseStatus::Finished;
    }

    const auto next = str.find_first_not_of(" \t\n\r", end);

    if (next == std::string_view::npos) {
        return ParseStatus::Finished;
    } else {
        str = str.substr(next);
        return ParseStatus::Continue;
    }
}

