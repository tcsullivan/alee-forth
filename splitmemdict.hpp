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

#ifndef ALEEFORTH_SPLITMEMDICT_HPP
#define ALEEFORTH_SPLITMEMDICT_HPP

#include "alee.hpp"

#include <algorithm>

#ifndef MEMDICTSIZE
#define MEMDICTSIZE (65536)
#endif
constexpr unsigned long int MemDictSize = MEMDICTSIZE;

template<unsigned long int RON>
class SplitMemDict : public Dictionary
{
    const uint8_t *rodict;
    uint8_t rwdict[MemDictSize - Dictionary::Begin] = {0};
    uint8_t extra[Dictionary::Begin];

    Addr convertAddress(Addr addr) const noexcept {
        return static_cast<Addr>(addr - (addr >= RON) * RON);
    }

public:
    constexpr explicit SplitMemDict(const uint8_t *rod):
        rodict(rod)
    {
        std::copy(rodict, rodict + sizeof(extra), extra);
    }

    constexpr SplitMemDict(const SplitMemDict<RON>& spd):
        SplitMemDict(spd.rodict) {}
    constexpr auto& operator=(const SplitMemDict<RON>& spd) {
        *this = SplitMemDict(spd.rodict);
        return *this;
    }

    virtual Cell read(Addr addr) const noexcept final {
        const uint8_t *dict;
        if (addr < RON)
            dict = addr < Dictionary::Begin ? extra : rodict;
        else
            dict = rwdict;

        return *reinterpret_cast<const Cell *>(dict + convertAddress(addr));
    }

    virtual void write(Addr addr, Cell value) noexcept final {
        if (addr >= RON)
            *reinterpret_cast<Cell *>(rwdict + addr - RON) = value;
        else if (addr < Dictionary::Begin)
            *reinterpret_cast<Cell *>(extra + addr) = value;
    }

    virtual uint8_t readbyte(Addr addr) const noexcept final {
        const uint8_t *dict;
        if (addr < RON)
            dict = addr < Dictionary::Begin ? extra : rodict;
        else
            dict = rwdict;

        return dict[convertAddress(addr)];
    }

    virtual void writebyte(Addr addr, uint8_t value) noexcept final {
        if (addr >= RON)
            rwdict[addr - RON] = value;
        else if (addr < Dictionary::Begin)
            extra[addr] = value;
    }

    virtual unsigned long int capacity() const noexcept final {
        return RON + sizeof(extra) + sizeof(rwdict);
    }
};

#endif // ALEEFORTH_SPLITMEMDICT_HPP

