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

constexpr unsigned DataStackSize = 8;
constexpr unsigned ReturnStackSize = 8;

struct State
{
    Addr ip = 0;
    Dictionary& dict;
    void (*input)(State&);

    Cell dstack[DataStackSize] = {};
    Cell rstack[ReturnStackSize] = {};
    Cell *dsp = dstack - 1;
    Cell *rsp = rstack - 1;

    constexpr State(Dictionary& d, void (*i)(State&)):
        dict(d), input(i) {}

    bool compiling() const;
    void compiling(bool);

    void execute(Addr);

    std::size_t size() const noexcept;
    std::size_t rsize() const noexcept;

    inline void push(Cell value) {
        if (dsp == dstack + DataStackSize - 1)
            throw exc_push();
        *++dsp = value;
    }

    inline Cell pop() {
        if (dsp < dstack)
            throw exc_pop();
        return *dsp--;
    }

    inline Cell beyondip() {
        ip += sizeof(Cell);
        return dict.read(ip);
    }

    inline void pushr(Cell value) {
        if (rsp == rstack + ReturnStackSize - 1)
            throw exc_pushr();
        *++rsp = value;
    }

    inline Cell popr() {
        if (rsp < rstack)
            throw exc_popr();
        return *rsp--;
    }

    inline Cell& top() {
        if (dsp < dstack)
            throw exc_top();
        return *dsp;
    }

    inline Cell& pick(std::size_t i) {
        if (dsp - i < dstack)
            throw exc_pick();
        return *(dsp - i);
    }

    struct exc_pop {};
    struct exc_push {};
    struct exc_popr {};
    struct exc_pushr {};
    struct exc_top {};
    struct exc_pick {};
};

#endif // ALEEFORTH_STATE_HPP

