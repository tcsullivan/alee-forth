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

#include <csetjmp>
#include <cstddef>

constexpr unsigned DataStackSize = 16;
constexpr unsigned ReturnStackSize = 16;

class State
{
public:
    enum class Error : int {
        none = 0,
        push,
        pop,
        pushr,
        popr,
        top,
        pick,
        exit
    };

    Addr ip = 0;
    Dictionary& dict;
    void (*input)(State&);

    std::jmp_buf jmpbuf = {};

    constexpr State(Dictionary& d, void (*i)(State&)):
        dict(d), input(i) {}

    bool compiling() const;
    void compiling(bool);

    Error execute(Addr);

    std::size_t size() const noexcept;
    std::size_t rsize() const noexcept;

    inline void push(Cell value) {
        if (dsp == dstack + DataStackSize)
            std::longjmp(jmpbuf, static_cast<int>(Error::push));
        *dsp++ = value;
    }

    inline Cell pop() {
        if (dsp == dstack)
            std::longjmp(jmpbuf, static_cast<int>(Error::pop));
        return *--dsp;
    }

    inline Cell beyondip() {
        ip += sizeof(Cell);
        return dict.read(ip);
    }

    inline void pushr(Cell value) {
        if (rsp == rstack + ReturnStackSize)
            std::longjmp(jmpbuf, static_cast<int>(Error::pushr));
        *rsp++ = value;
    }

    inline Cell popr() {
        if (rsp == rstack)
            std::longjmp(jmpbuf, static_cast<int>(Error::popr));
        return *--rsp;
    }

    inline Cell& top() {
        if (dsp == dstack)
            std::longjmp(jmpbuf, static_cast<int>(Error::top));
        return *(dsp - 1);
    }

    inline Cell& pick(std::size_t i) {
        if (dsp - i == dstack)
            std::longjmp(jmpbuf, static_cast<int>(Error::pick));
        return *(dsp - i - 1);
    }

private:
    Cell dstack[DataStackSize] = {};
    Cell rstack[ReturnStackSize] = {};
    Cell *dsp = dstack;
    Cell *rsp = rstack;

};

#endif // ALEEFORTH_STATE_HPP
