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

#ifndef ALEEFORTH_TYPES_HPP
#define ALEEFORTH_TYPES_HPP

#include <cstdint>
#include <iterator>

struct Dictionary;
struct State;

using Addr = uint16_t;
using Cell = int16_t;
using DoubleCell = int32_t;
using Func = void (*)(State&);

constexpr unsigned int MaxCellNumberChars = 6; // -32768

enum class Error : int {
    none = 0,
    push,
    pop,
    pushr,
    popr,
    top,
    pick,
    exit,
    noword
};

struct Word
{
    struct iterator;

    Addr start = 0;
    Addr wend = 0;

    iterator begin(const Dictionary *);
    iterator end(const Dictionary *);
    unsigned size() const noexcept;

    struct iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = uint8_t;
        using pointer = void;
        using reference = void;
        using difference_type = Cell;

        Addr addr;
        const Dictionary *dict;

        constexpr iterator(Addr a, const Dictionary *d):
            addr(a), dict(d) {}

        iterator& operator++();
        value_type operator*();
        bool operator!=(const iterator&);
    };
};

#endif // ALEEFORTH_TYPES_HPP

