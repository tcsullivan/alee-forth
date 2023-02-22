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
    if (addr & (sizeof(Cell) - sizeof(uint8_t)))
        addr = (addr + sizeof(Cell)) & ~(sizeof(Cell) - sizeof(uint8_t));

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
    for (unsigned i = 0; i < word.size(); ++i)
        writebyte(allot(1), readbyte(word.start + i));

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
    const auto len = read(Dictionary::Input);
    if (len == 0)
        return {};

    Addr wordstart = Dictionary::Input + sizeof(Cell) + Dictionary::InputCells
                     - len;
    Addr wordend = wordstart;

    auto cnt = len;
    while (cnt) {
        auto b = readbyte(wordend);
        if (isspace(b)) {
            if (wordstart != wordend) {
                Word word {wordstart, wordend};
                writebyte(Dictionary::Input, cnt - 1);
                return word;
            } else {
                ++wordstart;
            }
        }

        ++wordend;
        --cnt;
    }

    return {};
}

bool Dictionary::equal(Word word, std::string_view sv) const
{
    if (sv.size() != word.end - word.start)
        return false;

    for (unsigned i = 0; i < sv.size(); ++i) {
        if (sv[i] != readbyte(word.start + i))
            return false;
    }

    return true;
}

bool Dictionary::equal(Word word, Word other) const
{
    if (word.size() != other.size())
        return false;

    for (unsigned i = 0; i < word.size(); ++i) {
        if (readbyte(word.start + i) != readbyte(other.start + i))
            return false;
    }

    return true;
}

