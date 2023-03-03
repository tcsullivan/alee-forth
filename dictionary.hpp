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

class Dictionary
{
public:
    constexpr static Addr Base       = 0;
    constexpr static Addr Here       = sizeof(Cell);
    constexpr static Addr Latest     = sizeof(Cell) * 2;
    constexpr static Addr Compiling  = sizeof(Cell) * 3;
    constexpr static Addr Input      = sizeof(Cell) * 4; // len data...
    constexpr static Addr InputCells = 82; // bytes!
    constexpr static Addr Begin      = sizeof(Cell) * 5 + InputCells;

    void initialize();

    Addr here() const noexcept { return read(Here); }
    void here(Addr l) noexcept { write(Here, l); }

    Addr latest() const noexcept { return read(Latest); }
    void latest(Addr l) noexcept { write(Latest, l); }

    virtual Cell read(Addr) const noexcept = 0;
    virtual void write(Addr, Cell) noexcept = 0;
    virtual uint8_t readbyte(Addr) const noexcept = 0;
    virtual void writebyte(Addr, uint8_t) noexcept = 0;

    Addr alignhere() noexcept;
    Addr aligned(Addr) const noexcept;
    Addr allot(Cell) noexcept;
    void add(Cell) noexcept;
    void addDefinition(Word) noexcept;

    Addr find(Word) noexcept;
    Addr getexec(Addr) noexcept;
    Word input() noexcept;

    bool equal(Word, const char *, unsigned) const noexcept;
    bool equal(Word, Word) const noexcept;
};

#endif // ALEEFORTH_DICTIONARY_HPP

