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
#include <cstring>

void Dictionary::initialize()
{
    write(Base, 10);
    write(Here, Begin);
    write(Latest, Begin);
    write(Compiling, 0);
}

Addr Dictionary::allot(Cell amount) noexcept
{
    Addr old = here();
    here(old + amount);
    return old;
}

void Dictionary::add(Cell value) noexcept
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

Addr Dictionary::alignhere() noexcept
{
    here(aligned(here()));
    return here();
}

void Dictionary::addDefinition(Word word) noexcept
{
    add(word.size());
    for (auto w = word.start; w != word.end; ++w)
        writebyte(allot(1), readbyte(w));

    alignhere();
}

Addr Dictionary::find(Word word) noexcept
{
    Addr lt = latest(), oldlt;
    do {
        oldlt = lt;
        const auto l = static_cast<Addr>(read(lt));
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

Addr Dictionary::getexec(Addr addr) noexcept
{
    const auto len = read(addr) & 0x1F;
    return aligned(addr + sizeof(Cell) + len);
}

Word Dictionary::input() noexcept
{
    auto idx = read(Dictionary::Input);
    auto ch = readbyte(Dictionary::Input + sizeof(Cell) + idx);

    if (ch) {
        Addr wordstart = Dictionary::Input + sizeof(Cell) + idx;
        Addr wordend = wordstart;

        do {
            ch = readbyte(wordend);

            if (isspace(ch) || ch == '\0') {
                if (wordstart != wordend) {
                    if (isspace(ch))
                        ++idx;
                    writebyte(Dictionary::Input, idx);
                    return {wordstart, wordend};
                } else if (ch == '\0') {
                    return {};
                }

                ++wordstart;
            }

            ++wordend;
            ++idx;
        } while (ch);
    }

    return {};
}

bool Dictionary::equal(Word word, const char *str, unsigned len) const noexcept
{
    if (word.size() != len)
        return false;

    for (auto w = word.start; w != word.end; ++w) {
        auto wc = readbyte(w);
        if (wc != *str) {
            if (isalpha(wc) && isalpha(*str) && (wc | 32) == (*str | 32)) {
                ++str;
                continue;
            }

            return false;
        }

        ++str;
    }

    return true;
}

bool Dictionary::equal(Word word, Word other) const noexcept
{
    if (word.size() != other.size())
        return false;

    auto w = word.start, o = other.start;
    while (w != word.end) {
        auto wc = readbyte(w), oc = readbyte(o);
        if (wc != oc) {
            if (isalpha(wc) && isalpha(oc) && (wc | 32) == (oc | 32)) {
                ++w, ++o;
                continue;
            }

            return false;
        }

        ++w, ++o;
    }

    return true;
}

