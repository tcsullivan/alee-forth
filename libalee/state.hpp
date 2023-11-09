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

#define verify(C, E)

constexpr unsigned DataStackSize = 64;
constexpr unsigned ReturnStackSize = 64;

class State
{
    using InputFunc = void (*)(State&);

    struct Context {
        Addr ip = 0;
        std::jmp_buf jmpbuf = {};
    };

public:
    Dictionary& dict;

    constexpr State(Dictionary& d, InputFunc i):
        dict(d), inputfunc(i), context() {}

    /**
     * Begins execution at the given execution token.
     * If the token is a CoreWord, this function exits after its execution.
     * Otherwise, execution continues until the word's execution completes.
     * Encountering an error will cause this function to exit immediately.
     */
    Error execute(Addr);

    /**
     * Clears the data and return stacks, sets ip to zero, and clears the
     * compiling flag.
     */
    void reset();

    void exit() {
        std::longjmp(context.jmpbuf, static_cast<int>(Error::exit));
    }

    Addr& ip() noexcept {
        return context.ip;
    }

    void input() noexcept {
        inputfunc(*this);
    }

    bool compiling() const;
    void compiling(bool);

    std::size_t size() const noexcept;
    std::size_t rsize() const noexcept;

    /**
     * Saves execution state so that a new execution can begin.
     * Used for EVALUATE.
     */
    Context save();

    /**
     * Reloads the given execution state.
     */
    void load(const Context&);

    inline void push(Cell value) {
        verify(dsp < dstack + DataStackSize, Error::push);
        *dsp++ = value;
    }

    inline const Cell& pop() {
        verify(dsp > dstack, Error::pop);
        return *--dsp;
    }

    inline DoubleCell popd() {
        DoubleCell dcell = pop();
        dcell <<= sizeof(Cell) * 8;
        dcell |= static_cast<Addr>(pop());
        return dcell;
    }

    inline void pushd(DoubleCell d) {
        push(static_cast<Cell>(d));
        push(static_cast<Cell>(d >> (sizeof(Cell) * 8)));
    }

    inline void pushr(Cell value) {
        verify(rsp < rstack + ReturnStackSize, Error::pushr);
        *rsp++ = value;
    }

    inline const Cell& popr() {
        verify(rsp > rstack, Error::popr);
        return *--rsp;
    }

    inline Cell& pick(std::size_t i) {
        verify(dsp - i > dstack, Error::pick);
        return *(dsp - i - 1);
    }

    inline Cell& top() {
        return pick(0);
    }

    // Advances the instruction pointer and returns that cell's contents.
    inline Cell beyondip() {
        context.ip += sizeof(Cell);
        return dict.read(context.ip);
    }

//    inline void verify(bool condition, Error error) {
//        if (!condition)
//            std::longjmp(context.jmpbuf, static_cast<int>(error));
//    }

private:
    InputFunc inputfunc; // User-provided function to collect "stdin" input.
    Context context;

    Cell dstack[DataStackSize] = {};
    Cell rstack[ReturnStackSize] = {};
    Cell *dsp = dstack;
    Cell *rsp = rstack;
};

#endif // ALEEFORTH_STATE_HPP

