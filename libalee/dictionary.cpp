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

#include "alee.hpp"

LIBALEE_SECTION
void Dictionary::initialize()
{
    write(Base, 10);
    write(Here, Begin);
    write(Latest, Begin);
    write(Compiling, 0);
    write(Source, Input + sizeof(Cell));
}

LIBALEE_SECTION
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

LIBALEE_SECTION
void Dictionary::add(Cell value) noexcept
{
    write(allot(sizeof(Cell)), value);
}

LIBALEE_SECTION
Addr Dictionary::aligned(Addr addr)
{
    return (addr + (sizeof(Cell) - 1)) & ~(sizeof(Cell) - 1);
}

LIBALEE_SECTION
Addr Dictionary::alignhere() noexcept
{
    here(aligned(here()));
    return here();
}

LIBALEE_SECTION
void Dictionary::addDefinition(Word word) noexcept
{
    Cell wsize = word.size();
    add(wsize);

    if (alignhere() - latest() >= ((1 << (sizeof(Cell) * 8 - 6)) - 1))
        add(0);

    auto addr = allot(wsize);
    auto it = word.begin(this);
    const auto end = word.end(this);

    while (it != end)
        writebyte(addr++, *it++);

    alignhere();
}

LIBALEE_SECTION
Addr Dictionary::find(Word word) noexcept
{
    Addr lt = latest();

    for (;;) {
        const Addr l = read(lt);
        const Addr len = l & 0x1F;
        Word lw;

        if ((l >> 6) < 1023) {
            lw = Word::fromLength(lt + sizeof(Cell), len);
            if (equal(word, lw))
                return lt;
            else if (lt == Begin)
                break;
            else
                lt -= l >> 6;
        } else {
            lw = Word::fromLength(lt + 2 * sizeof(Cell), len);
            if (equal(word, lw))
                return lt;
            else if (lt == Begin)
                break;
            else
                lt -= static_cast<Addr>(read(lt + sizeof(Cell)));
        }
    }

    return 0;
}

LIBALEE_SECTION
Addr Dictionary::getexec(Addr addr) noexcept
{
    const Addr l = read(addr);
    const Addr len = l & 0x1Fu;

    addr += sizeof(Cell);
    if ((l >> 6) == 1023)
        addr += sizeof(Cell);

    addr += len;
    return aligned(addr);
}

LIBALEE_SECTION
bool Dictionary::hasInput() const noexcept
{
    const Addr src = read(Dictionary::Source);
    const Addr end = read(Dictionary::SourceLen);
    uint8_t idx = read(Dictionary::Input) & 0xFFu;

    while (idx < end) {
        auto ch = readbyte(src + idx);

        if (ch == '\0') {
            break;
        } else if (!isspace(ch)) {
            return true;
        }

        ++idx;
    }

    return false;
}

LIBALEE_SECTION
Word Dictionary::input() noexcept
{
    const Addr src = read(Dictionary::Source);
    const Addr end = read(Dictionary::SourceLen);
    uint8_t idx = read(Dictionary::Input) & 0xFFu;

    Addr wstart = src + idx;
    Addr wend = wstart;

    while (idx < end) {
        auto ch = readbyte(wend);

        if (isspace(ch)) {
            if (wend - wstart > 0)
                break;

            ++wstart;
        } else if (ch == '\0') {
            break;
        }

        ++wend;
        ++idx;
    }

    writebyte(Dictionary::Input, ++idx);
    return Word(wstart, wend);
}

LIBALEE_SECTION
bool Dictionary::equal(Word word, const char *str, unsigned len) const noexcept
{
    return word.size() == len && equal(word.begin(this), word.end(this), str);
}

LIBALEE_SECTION
bool Dictionary::equal(Word word, Word other) const noexcept
{
    return word.size() == other.size() && equal(word.begin(this), word.end(this), other.begin(this));
}

