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
#include "state.hpp"

#include <iterator>

bool State::compiling() const
{
    return dict.read(Dictionary::Compiling);
}

void State::compiling(bool yes)
{
    dict.write(Dictionary::Compiling, yes);
}

State::Error State::execute(Addr addr)
{
    auto stat = setjmp(jmpbuf);
    if (!stat) {
        if (addr < CoreWords::WordCount) {
            CoreWords::run(addr, *this);
        } else {
            pushr(0);
            ip = addr - sizeof(Cell);

            do {
                ip += sizeof(Cell);
                CoreWords::run(dict.read(ip), *this);
            } while (ip);
        }
    } else {
        return static_cast<State::Error>(stat);
    }

    return State::Error::none;
}

std::size_t State::size() const noexcept
{
    return std::distance(dstack, static_cast<const Cell *>(dsp)) + 1;
}

std::size_t State::rsize() const noexcept
{
    return std::distance(rstack, static_cast<const Cell *>(rsp)) + 1;
}

