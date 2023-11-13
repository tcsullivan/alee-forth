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

#ifndef ALEEFORTH_SPLITMEMDICTRW_HPP
#define ALEEFORTH_SPLITMEMDICTRW_HPP

#include "libalee/alee.hpp"

#include <algorithm>

template<unsigned long int LON, unsigned long int HIN>
class SplitMemDictRW : public Dictionary
{
    uint8_t *lodict;
    uint32_t hidict;

public:
    constexpr explicit SplitMemDictRW(uint8_t *lo, uint32_t hi):
        lodict(lo), hidict(hi) {}

    constexpr SplitMemDictRW(const SplitMemDictRW<LON, HIN>& spd):
        SplitMemDictRW(spd.lodict, spd.hidict) {}
    constexpr auto& operator=(const SplitMemDictRW<LON, HIN>& spd) {
        *this = SplitMemDictRW(spd.lodict, hidict);
        return *this;
    }

    virtual Cell read(Addr addr) const noexcept final {
        if (addr < LON)
            return *reinterpret_cast<const Cell *>(lodict + addr);
        else
            return _data20_read_short(hidict + addr - LON);
    }

    virtual void write(Addr addr, Cell value) noexcept final {
        if (addr < LON)
            *reinterpret_cast<Cell *>(lodict + addr) = value;
        else
            _data20_write_short(hidict + addr - LON, value);
    }

    virtual uint8_t readbyte(Addr addr) const noexcept final {
        if (addr < LON)
            return lodict[addr];
        else
            return _data20_read_char(hidict + addr - LON);
    }

    virtual void writebyte(Addr addr, uint8_t value) noexcept final {
        if (addr < LON)
            lodict[addr] = value;
        else
            _data20_write_char(hidict + addr - LON, value);
    }

    virtual unsigned long int capacity() const noexcept final {
        return LON + HIN;
    }

private:
    virtual ~SplitMemDictRW() override {};
};

#endif // ALEEFORTH_SPLITMEMDICTRW_HPP

