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

#include "ctype.hpp"
#include "dictionary.hpp"

#include <cstring>

void Dictionary::initialize()
{
    write(Base, 10);
    write(Here, Begin);
    write(Latest, Begin);
    write(Compiling, 0);
    write(Source, Input + sizeof(Cell));
}

Addr Dictionary::allot(Cell amount) noexcept
{
    Addr old = here();
    decltype(capacity()) neww = old + amount;

    if (neww < capacity()) {
        write(Here, static_cast<Addr>(neww));
    } else {
        // TODO
    }

    return old;
}

void Dictionary::add(Cell value) noexcept
{
    write(allot(sizeof(Cell)), value);
}

Addr Dictionary::aligned(Addr addr) const noexcept
{
    Addr unaligned = addr & (sizeof(Cell) - sizeof(uint8_t));
    if (unaligned) {
        addr += sizeof(Cell);
        addr -= unaligned;
    }

    return addr;
}

Addr Dictionary::alignhere() noexcept
{
    here(aligned(here()));
    return here();
}

void Dictionary::addDefinition(Word word) noexcept
{
    Cell wsize = word.size();
    add(wsize);

    for (auto w = word.start; w != word.wend; ++w)
        writebyte(allot(1), readbyte(w));

    alignhere();
}

Addr Dictionary::find(Word word) noexcept
{
    Addr lt = latest();
    for (;;) {
        const Addr l = read(lt);
        const Addr len = l & 0x1F;
        Word lw;
        lw.start = lt + sizeof(Cell);
        lw.wend = lw.start + len;

        if (equal(word, lw))
            return lt;
        else if (lt == Begin)
            break;
        else
            lt -= l >> 6;
    }

    return 0;
}

Addr Dictionary::getexec(Addr addr) noexcept
{
    const Addr len = read(addr) & 0x1Fu;
    addr += sizeof(Cell);
    addr += len;
    return aligned(addr);
}

Word Dictionary::input() noexcept
{
    const Addr src = read(Dictionary::Source);
    const Addr end = read(Dictionary::SourceLen);
    uint8_t idx = read(Dictionary::Input) & 0xFFu;

    Word word;
    word.start = src + idx;
    word.wend = word.start;

    while (idx < end) {
        auto ch = readbyte(word.wend);

        if (isspace(ch)) {
            if (word.size() > 0)
                break;

            ++word.start;
        } else if (ch == '\0') {
            break;
        }

        ++word.wend;
        ++idx;
    }

    writebyte(Dictionary::Input, ++idx);
    return word;
}

bool Dictionary::equal(Word word, const char *str, unsigned len) const noexcept
{
    return word.size() == len &&
           equal(word.begin(this), word.end(this), str);
}

bool Dictionary::equal(Word word, Word other) const noexcept
{
    return word.size() == other.size() &&
           equal(word.begin(this), word.end(this), other.begin(this));
}

bool Dictionary::eqchars(char c1, char c2)
{
    return c1 == c2 ||
        (isalpha(c1) && isalpha(c2) && (c1 | 32) == (c2 | 32));
}

