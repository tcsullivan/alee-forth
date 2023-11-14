//
/// @file state.hpp
/// @brief Provides object to manage execution and interpretation state.
//
// Alee Forth: A portable and concise Forth implementation in modern C++.
// Copyright (C) 2023  Clyne Sullivan <clyne@bitgloo.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ALEEFORTH_STATE_HPP
#define ALEEFORTH_STATE_HPP

#include "config.hpp"
#include "dictionary.hpp"
#include "types.hpp"

#include <csetjmp>
#include <cstddef>

/**
 * Size of the primary data stack, number of cells.
 */
constexpr unsigned DataStackSize = 64;

/**
 * Size of the return stack, number of cells.
 */
constexpr unsigned ReturnStackSize = 64;

/**
 * @class State
 * Object to track execution state.
 */
class State
{
    /** Input functions should add input to the input buffer when available. */
    using InputFunc = void (*)(State&);

    /** Context object that defines a state of execution. */
    struct Context {
        Addr ip = 0; /** Instruction pointer */
        std::jmp_buf jmpbuf = {}; /** setjmp() buffer for exiting execute() */
    };

public:
    /** Reference to dictionary used by this state. */
    Dictionary& dict;

    /**
     * Constructs a state object that uses the given dictionary and input
     * function.
     * @param d The dictionary to be used by this state
     * @param i The input collection function to be used by this state
     */
    constexpr State(Dictionary& d, InputFunc i):
        dict(d), inputfunc(i), context() {}

    /**
     * Begins execution starting from the given execution token.
     * If the token is a CoreWord, this function exits after its execution.
     * Otherwise, execution continues until the word's execution completes.
     * Encountering an error will cause this function to exit immediately.
     * @param addr The token to be executed
     * @return An error token to indicate if execution was successful
     */
    Error execute(Addr addr);

    /**
     * Clears the data and return stacks, sets ip to zero, and clears the
     * compiling flag.
     */
    void reset();

    /** Returns a reference to the instruction pointer. */
    LIBALEE_SECTION
    Addr& ip() noexcept {
        return context.ip;
    }

    /** Calls the user input function with this state as the argument. */
    LIBALEE_SECTION
    void input() noexcept {
        inputfunc(*this);
    }

    /** Returns true if currently in a compiling state. */
    bool compiling() const;
    /** Sets the compiling state. True if compiling, false if interpreting. */
    void compiling(bool);

    /** Returns the number of values stored on the data stack. */
    std::size_t size() const noexcept;
    /** Returns the number of values stored on the return stack. */
    std::size_t rsize() const noexcept;

    /**
     * Returns the current execution state. Usually followed by a load() call.
     */
    Context save();

    /**
     * Reloads the given execution state.
     */
    void load(const Context&);

    /**
     * Pushes the given value to the data stack.
     */
    LIBALEE_SECTION
    inline void push(Cell value) {
        verify(dsp < dstack + DataStackSize, Error::push);
        *dsp++ = value;
    }

    /**
     * Pops a value from the data stack and returns that value.
     */
    LIBALEE_SECTION
    inline Cell pop() {
        verify(dsp > dstack, Error::pop);
        return *--dsp;
    }

    /**
     * Pushes the given value to the return stack.
     */
    LIBALEE_SECTION
    inline void pushr(Cell value) {
        verify(rsp < rstack + ReturnStackSize, Error::pushr);
        *rsp++ = value;
    }

    /**
     * Pops a value from the return stack and returns that value.
     */
    LIBALEE_SECTION
    inline Cell popr() {
        verify(rsp > rstack, Error::popr);
        return *--rsp;
    }

    /**
     * Returns the value stored at the current data stack position.
     */
    LIBALEE_SECTION
    inline Cell& top() {
        verify(dsp > dstack, Error::top);
        return *(dsp - 1);
    }

    /**
     * Picks a value currently stored on the data stack.
     * @param i Index from current position to fetch from
     * @return The value stored at the given index
     */
    LIBALEE_SECTION
    inline Cell& pick(std::size_t i) {
        verify(dsp - i > dstack, Error::pick);
        return *(dsp - i - 1);
    }

    /**
     * Advances the instruction pointer and returns that cell's contents.
     */
    LIBALEE_SECTION
    inline Cell beyondip() {
        context.ip += sizeof(Cell);
        return dict.read(context.ip);
    }

    /**
     * Asserts the given condition is true, longjmp-ing if false.
     * Used as an exception handler and the method of exiting execution.
     * @param condition Condition to be tested
     * @param error Error code to report via longjmp() on false condition
     */
    LIBALEE_SECTION
    inline void verify(bool condition, Error error) {
        if (!condition)
            std::longjmp(context.jmpbuf, static_cast<int>(error));
    }

private:
    InputFunc inputfunc; /** User-provided function to collect user input. */
    Context context; /** State's current execution context. */

    Cell dstack[DataStackSize] = {}; /** Data stack */
    Cell rstack[ReturnStackSize] = {}; /** Return stack */
    Cell *dsp = dstack; /** Current data stack position */
    Cell *rsp = rstack; /** Current return stack position */
};

#endif // ALEEFORTH_STATE_HPP

