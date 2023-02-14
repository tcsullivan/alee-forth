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

#include "dictionary.hpp"

Addr Dictionary::allot(Cell amount)
{
    Addr old = here;
    here += amount;
    return old;
}

void Dictionary::add(Cell value)
{
    write(allot(sizeof(Cell)), value);
}

Addr Dictionary::alignhere()
{
    if (here & (sizeof(Cell) - sizeof(uint8_t)))
        here = (here + sizeof(Cell)) & ~(sizeof(Cell) - sizeof(uint8_t));

    return here;
}

void Dictionary::addDefinition(std::string_view str)
{
    add(str.size());
    for (char c : str)
        add(c);
}

bool Dictionary::issame(Addr addr, std::string_view str, std::size_t n)
{
    if (str.size() != n)
        return false;

    for (char c : str) {
        if (read(addr) != c)
            return false;

        addr += sizeof(Cell);
    }

    return true;
}

Addr Dictionary::find(std::string_view str)
{
    if (latest == 0)
        return 0;

    Addr lt = latest, oldlt;
    do {
        oldlt = lt;
        const auto l = read(lt);
        const auto len = l & 0x1F;

        if (issame(lt + sizeof(Cell), str, len))
            return lt;
        else
            lt -= l >> 6;
    } while (lt != oldlt);

    return 0;
}

Addr Dictionary::getexec(Addr addr)
{
    const auto len = read(addr) & 0x1F;
    return addr + (1 + len) * sizeof(Cell);
}

