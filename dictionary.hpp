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

struct Dictionary
{
    Addr here = 1;
    Addr latest = 0;

    virtual Cell read(Addr) const = 0;
    virtual int write(Addr, Cell) = 0;

    Addr allot(Cell amount) {
        Addr old = here;
        here += amount;
        return old;
    }

    void add(Cell value) {
        write(here++, value);
    }

    void addDefinition(std::string_view str) {
        add(str.size());
        for (char c : str)
            add(c);

        if (here & 1)
            allot(1);
    }

    bool issame(Addr addr, std::string_view str, std::size_t n) {
        if (str.size() != n)
            return false;

        for (char c : str) {
            if (read(addr++) != c)
                return false;
        }
    
        return true;
    }

    Addr find(std::string_view str) {
        if (latest == 0)
            return 0;

        auto lt = latest;
        do {
            const auto l = read(lt);
            const auto len = l & 0x1F;

            if (issame(lt + 1, str, len))
                return lt;
            else
                lt -= l >> 6;
        } while (lt);

        return 0;
    }

    Addr getexec(Addr addr) {
        const auto len = read(addr) & 0x1F;
        return ((addr + 1 + len) + 1) & ~1;
    }
};

#endif // ALEEFORTH_DICTIONARY_HPP

