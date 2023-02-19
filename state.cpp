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

struct pop {};
struct push {};
struct popr {};
struct pushr {};
struct top {};
struct pick {};

bool State::compiling() const
{
    return dict.read(Dictionary::Compiling);
}

void State::compiling(bool yes)
{
    dict.write(Dictionary::Compiling, yes);
}

void State::execute(Addr addr)
{
    if (addr < Dictionary::Begin) {
        // Must be a core-word
        CoreWords::run(addr, *this);
    } else {
        pushr(0);
        ip = addr - sizeof(Cell);

        do {
            ip += sizeof(Cell);
            CoreWords::run(dict.read(ip), *this);
        } while (ip);
    }
}

Cell State::beyondip() const
{
    return dict.read(ip + sizeof(Cell));
}

void State::pushr(Cell value)
{
    if (rsize() == ReturnStackSize)
        throw ::pushr();
    *++rsp = value;
}

Cell State::popr()
{
    if (rsize() == 0)
        throw ::popr();
    return *rsp--;
}

void State::push(Cell value)
{
    if (size() == DataStackSize)
        throw ::push();
    *++dsp = value;
}

Cell State::pop()
{
    if (size() == 0)
        throw ::pop();
    return *dsp--;
}

Cell& State::top()
{
    if (size() == 0)
        throw ::top();
    return *dsp;
}

Cell& State::pick(std::size_t i)
{
    if (i >= size())
        throw ::pick();
    return *(dsp - i);
}

std::size_t State::size() const noexcept
{
    return std::distance(dstack, static_cast<const Cell *>(dsp)) + 1;
}

std::size_t State::rsize() const noexcept
{
    return std::distance(rstack, static_cast<const Cell *>(rsp)) + 1;
}

