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

#include <iterator>

LIBALEE_SECTION
bool State::compiling() const
{
    return dict.read(Dictionary::Compiling);
}

LIBALEE_SECTION
void State::compiling(bool yes)
{
    dict.write(Dictionary::Compiling, yes);
}

LIBALEE_SECTION
State::Context State::save()
{
    return context;
}

LIBALEE_SECTION
void State::load(const State::Context& ctx)
{
    context = ctx;
}

LIBALEE_SECTION
Error State::execute(Addr addr)
{
    auto stat = static_cast<Error>(setjmp(context.jmpbuf));

    if (stat == Error::none) {
        CoreWords::run(addr, *this);

        if (context.ip >= Dictionary::Begin) {
            // longjmp will exit this loop.
            for (;;)
                CoreWords::run(dict.read(context.ip), *this);
        } else {
            // addr was a CoreWord, all done now.
            context.ip = 0;
        }
    } else if (stat == Error::exit) {
        stat = Error::none;
    }

    return stat;
}

LIBALEE_SECTION
void State::reset()
{
    while (size())
        pop();
    while (rsize())
        popr();

    dict.write(Dictionary::Compiling, 0);
    context.ip = 0;
}

LIBALEE_SECTION
std::size_t State::size() const noexcept
{
    return dsp - dstack;
}

LIBALEE_SECTION
std::size_t State::rsize() const noexcept
{
    return rsp - rstack;
}

