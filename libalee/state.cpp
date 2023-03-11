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

#include <cstring>
#include <iterator>

bool State::compiling() const
{
    return dict.read(Dictionary::Compiling);
}

void State::compiling(bool yes)
{
    dict.write(Dictionary::Compiling, yes);
}

std::pair<Addr, std::jmp_buf> State::save()
{
    std::pair<Addr, std::jmp_buf> st;
    st.first = ip;
    std::memcpy(st.second, jmpbuf, sizeof(std::jmp_buf));
    return st;
}

void State::load(const std::pair<Addr, std::jmp_buf>& st)
{
    ip = st.first;
    std::memcpy(jmpbuf, st.second, sizeof(std::jmp_buf));
}

Error State::execute(Addr addr)
{
    auto stat = static_cast<Error>(setjmp(jmpbuf));

    if (stat == Error::none) {
        CoreWords::run(addr, *this);

        if (ip >= Dictionary::Begin) {
            // longjmp will exit this loop.
            for (;;)
                CoreWords::run(dict.read(ip), *this);
        } else {
            // addr was a CoreWord, all done now.
            ip = 0;
        }
    } else if (stat == Error::exit) {
        stat = Error::none;
    }

    return stat;
}

void State::reset()
{
    while (size())
        pop();
    while (rsize())
        popr();

    dict.write(Dictionary::Compiling, 0);
    ip = 0;
}

std::size_t State::size() const noexcept
{
    return std::distance(dstack, static_cast<const Cell *>(dsp));
}

std::size_t State::rsize() const noexcept
{
    return std::distance(rstack, static_cast<const Cell *>(rsp));
}

