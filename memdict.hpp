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

#ifndef ALEEFORTH_MEMDICT_HPP
#define ALEEFORTH_MEMDICT_HPP

#include "dictionary.hpp"

constexpr unsigned long int MemDictSize = 4096;

class MemDict : public Dictionary
{
    uint8_t dict[MemDictSize];

public:
    virtual Cell read(Addr addr) const final {
        return *reinterpret_cast<const Cell *>(dict + addr);
    }

    virtual int write(Addr addr, Cell value) final {
        *reinterpret_cast<Cell *>(dict + addr) = value;
        return 0;
    }

    virtual uint8_t readbyte(Addr addr) const final {
        return dict[addr];
    }

    virtual int writebyte(Addr addr, uint8_t value) final {
        dict[addr] = value;
        return 0;
    }
};

#endif // ALEEFORTH_MEMDICT_HPP

