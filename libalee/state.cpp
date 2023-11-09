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

State::Context State::save()
{
    return context;
}

void State::load(const State::Context& ctx)
{
    context = ctx;
}

void State::execute1(Addr ins)
{
repeat:
    if (ins >= Dictionary::Begin) [[likely]] {
        // Subroutine call
        pushr(context.ip);
        context.ip = ins;
    } else {
        if (ins < CoreWords::WordCount) {
            if (CoreWords::run(ins, *this)) [[unlikely]] {
                ins = pop();
                goto repeat;
            }
        } else {
            push(static_cast<Cell>(ins - CoreWords::WordCount));
        }

        context.ip += sizeof(Cell);
    }
}

Error State::execute(Addr addr)
{
    auto stat = static_cast<Error>(setjmp(context.jmpbuf));

    if (stat == Error::none) {
        context.ip = 0;
        execute1(addr);

        if (context.ip >= Dictionary::Begin) {
            // longjmp will exit this loop.
            for (;;)
                execute1(dict.read(context.ip));
        } else {
            // addr was a CoreWord, all done now.
            context.ip = 0;
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
    context.ip = 0;
}

std::size_t State::size() const noexcept
{
    return dsp - dstack;
}

std::size_t State::rsize() const noexcept
{
    return rsp - rstack;
}

