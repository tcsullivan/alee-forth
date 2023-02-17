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

#ifndef ALEEFORTH_STATE_HPP
#define ALEEFORTH_STATE_HPP

#include "dictionary.hpp"
#include "types.hpp"

#include <cstddef>

constexpr unsigned DataStackSize = 12;
constexpr unsigned ReturnStackSize = 12;

class State
{
    Cell dstack[DataStackSize] = {};
    Cell rstack[ReturnStackSize] = {};
    Cell *dsp = dstack - 1;
    Cell *rsp = rstack - 1;

public:
    Addr ip = 0;
    Dictionary& dict;
    void (*input)(State&);

    constexpr State(Dictionary& d, void (*i)(State&)):
        dict(d), input(i) {}

    bool compiling() const;
    void compiling(bool);

    void execute(Addr);

    Cell beyondip() const;

    void pushr(Cell);
    Cell popr();

    void push(Cell);
    Cell pop();

    Cell& top();
    Cell& pick(std::size_t);

    std::size_t size() const noexcept;
    std::size_t rsize() const noexcept;
};

#endif // ALEEFORTH_STATE_HPP

