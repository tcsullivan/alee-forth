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
#include <string_view>

struct State;

using Addr = uint16_t;
using Cell = int16_t;
using Func = void (*)(State&);

struct Word
{
    Addr start = 0;
    Addr end = 0;

    unsigned size() const noexcept {
        return end - start;
    }
};

enum class ParseStatus
{
    Finished,
    NotAWord
};

std::string_view to_string(ParseStatus);

#endif // ALEEFORTH_TYPES_HPP

