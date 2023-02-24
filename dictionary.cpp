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

#include <cctype>

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

Addr Dictionary::aligned(Addr addr) const noexcept
{
    auto unaligned = addr & (sizeof(Cell) - sizeof(uint8_t));
    if (unaligned)
        addr += sizeof(Cell) - unaligned;

    return addr;
}

Addr Dictionary::alignhere()
{
    here = aligned(here);
    return here;
}

void Dictionary::addDefinition(Word word)
{
    add(word.size());
    for (auto w = word.start; w != word.end; ++w)
        writebyte(allot(1), readbyte(w));

    alignhere();
}

Addr Dictionary::find(Word word)
{
    Addr lt = latest(), oldlt;
    do {
        oldlt = lt;
        const Cell l = read(lt);
        const Addr len = l & 0x1F;
        const Word lw {
            static_cast<Addr>(lt + sizeof(Cell)),
            static_cast<Addr>(lt + sizeof(Cell) + len)
        };

        if (equal(word, lw))
            return lt;
        else
            lt -= l >> 6;
    } while (lt != oldlt);

    return 0;
}

Addr Dictionary::getexec(Addr addr)
{
    const auto len = read(addr) & 0x1F;
    return aligned(addr + sizeof(Cell) + len);
}

Word Dictionary::input()
{
    auto len = read(Dictionary::Input);
    if (len != 0) {
        Addr wordstart = Dictionary::Input + sizeof(Cell) + Dictionary::InputCells
                         - len;
        Addr wordend = wordstart;

        while (len) {
            auto b = readbyte(wordend);

            if (isspace(b)) {
                if (wordstart != wordend) {
                    writebyte(Dictionary::Input, len - 1);
                    return {wordstart, wordend};
                }

                ++wordstart;
            }

            ++wordend;
            --len;
        }
    }

    return {};
}

bool Dictionary::equal(Word word, std::string_view sv) const
{
    if (word.size() != sv.size())
        return false;

    for (auto w = word.start; w != word.end; ++w) {
        if (readbyte(w) != sv.front())
            return false;

        sv = sv.substr(1);
    }

    return true;
}

bool Dictionary::equal(Word word, Word other) const
{
    if (word.size() != other.size())
        return false;

    auto w = word.start, o = other.start;
    while (w != word.end) {
        if (readbyte(w) != readbyte(o))
            return false;

        ++w, ++o;
    }

    return true;
}

