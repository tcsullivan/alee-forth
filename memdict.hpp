//
/// @file memdict.hpp
/// @brief Simple dictionary implementation using a large memory block.
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

#ifndef ALEEFORTH_MEMDICT_HPP
#define ALEEFORTH_MEMDICT_HPP

#include "libalee/alee.hpp"

#ifndef MEMDICTSIZE
/** Default dictionary size in bytes. */
#define MEMDICTSIZE (65536)
#endif

/** Size in bytes of a MemDict. */
constexpr unsigned long int MemDictSize = MEMDICTSIZE;

/**
 * @class MemDict
 * Dictionary implementation that uses a large block of memory.
 */
class MemDict : public Dictionary
{
    /** Block of memory to contain dictionary's contents. */
    uint8_t dict[MemDictSize] = {0};

public:
    /** Returns the value of the cell at the given address. */
    virtual Cell read(Addr addr) const noexcept final {
        return *reinterpret_cast<const Cell *>(dict + addr);
    }

    /** Writes the given value to the cell at the given address. */
    virtual void write(Addr addr, Cell value) noexcept final {
        *reinterpret_cast<Cell *>(dict + addr) = value;
    }

    /** Returns the value of the byte at the given address. */
    virtual uint8_t readbyte(Addr addr) const noexcept final {
        return dict[addr];
    }

    /** Writes the given value to the byte at the given address. */
    virtual void writebyte(Addr addr, uint8_t value) noexcept final {
        dict[addr] = value;
    }

    /** Returns the size of the dictionary's memory block. */
    virtual unsigned long int capacity() const noexcept final {
        return sizeof(dict);
    }
};

#endif // ALEEFORTH_MEMDICT_HPP

