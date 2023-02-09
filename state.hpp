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
#include <iterator>

constexpr unsigned DataStackSize = 12;
constexpr unsigned ReturnStackSize = 12;

class State
{
    Cell dstack[DataStackSize] = {};
    Cell rstack[ReturnStackSize] = {};
    Cell *dsp = dstack - 1;
    Cell *rsp = rstack - 1;

public:
    bool compiling = false;
    Addr ip = 0;
    Pass pass = Pass::None;
    Dictionary& dict;

    constexpr State(Dictionary& d): dict(d) {}

    Cell beyondip() const {
	return dict.read(ip + 1);
    }

    void pushr(Cell value) {
	if (rsize() == ReturnStackSize)
            throw;
        *++rsp = value;
    }

    Cell popr() {
	if (rsize() == 0)
            throw;
	return *rsp--;
    }

    void push(Cell value) {
	if (size() == DataStackSize)
            throw;
        *++dsp = value;
    }

    Cell pop() {
	if (size() == 0)
            throw;
	return *dsp--;
    }

    Cell& top() {
	if (size() == 0)
            throw;
	return *dsp;
    }

    Cell& pick(std::size_t i) {
	if (i >= size())
	    throw;
	return *(dsp - i);
    }

    std::size_t size() const noexcept {
	return std::distance(dstack, static_cast<const Cell *>(dsp)) + 1;
    }

    std::size_t rsize() const noexcept {
	return std::distance(rstack, static_cast<const Cell *>(rsp)) + 1;
    }
};

#endif // ALEEFORTH_STATE_HPP

