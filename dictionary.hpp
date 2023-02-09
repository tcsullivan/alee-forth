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

#ifndef ALEEFORTH_DICTIONARY_HPP
#define ALEEFORTH_DICTIONARY_HPP

#include "types.hpp"

#include <cstddef>
#include <string_view>

class Dictionary
{
public:
    Addr here = 1; // address zero will be used for BASE.
    Addr latest = 0;

    virtual Cell read(Addr) const = 0;
    virtual int write(Addr, Cell) = 0;

    Addr allot(Cell);
    void add(Cell);
    void addDefinition(std::string_view);
    Addr find(std::string_view);
    Addr getexec(Addr);

private:
    bool issame(Addr, std::string_view, std::size_t);
};

#endif // ALEEFORTH_DICTIONARY_HPP

