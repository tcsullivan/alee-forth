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
#include <cstdint>
#include <string_view>

class Dictionary
{
public:
    constexpr static Addr Base       = 0;
    constexpr static Addr Compiling  = Base + sizeof(Cell);
    constexpr static Addr Input      = Compiling + sizeof(Cell); // len data...
    constexpr static Addr InputCells = 80; // bytes!
    constexpr static Addr Begin      = Input + sizeof(Cell) + InputCells;

    Addr here = Begin;
    Addr latest = Begin;

    virtual Cell read(Addr) const = 0;
    virtual void write(Addr, Cell) = 0;
    virtual uint8_t readbyte(Addr) const = 0;
    virtual void writebyte(Addr, uint8_t) = 0;

    Addr alignhere();
    Addr allot(Cell);
    void add(Cell);
    void addDefinition(Word);

    Addr find(Word);
    Addr getexec(Addr);
    Word input();

    bool equal(Word, std::string_view) const;
    bool equal(Word, Word) const;
};

#endif // ALEEFORTH_DICTIONARY_HPP

