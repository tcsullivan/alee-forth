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
#include "types.hpp"

Addr Word::size() const noexcept
{
    return wend - start;
}

Word::iterator Word::begin(const Dictionary *dict)
{
    return iterator(start, dict);
}

Word::iterator Word::end(const Dictionary *dict)
{
    return iterator(wend, dict);
}

Word::iterator& Word::iterator::operator++()
{
    addr++;
    return *this;
}

Word::iterator::value_type Word::iterator::operator*()
{
    return dict->readbyte(addr);
}

bool Word::iterator::operator!=(const iterator& other)
{
    return dict != other.dict || addr != other.addr;
}
