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

#include "ctype.hpp"
#include "types.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

/**
 * Dictionary entry format:
 *  - 1 information byte
 *    bits  0..4: Length of name (L)
 *    bit      5: Immediate?
 *    bits 6..15: Distance to next entry (negative)
 *  - L bytes of name
 *  - 0+ bytes for address alignment
 *  - 0+ bytes of entry's data...
 */

struct Dictionary
{
    /**
     * The beginning of the dictionary is used for "internal" variables.
     */
    constexpr static Addr Base       = 0;
    constexpr static Addr Here       = sizeof(Cell);
    constexpr static Addr Latest     = sizeof(Cell) * 2;
    constexpr static Addr Compiling  = sizeof(Cell) * 3;
    constexpr static Addr CompToken  = sizeof(Cell) * 4;
    constexpr static Addr Source     = sizeof(Cell) * 5;
    constexpr static Addr SourceLen  = sizeof(Cell) * 6;
    constexpr static Addr Input      = sizeof(Cell) * 7; // len data...
    constexpr static Addr InputCells = 80; // bytes!
    constexpr static Addr Begin      = sizeof(Cell) * 8 + InputCells;

    constexpr static Cell Immediate = (1 << 5);

    /**
     * Dictionary data can be stored on any read-write interface.
     * You must create a dictionary class that inherits Dictionary and
     * implement these functions. See `memdict.hpp` for a simple block-of-
     * memory implementation.
     */
    virtual Cell read(Addr) const noexcept = 0;
    virtual void write(Addr, Cell) noexcept = 0;
    virtual uint8_t readbyte(Addr) const noexcept = 0;
    virtual void writebyte(Addr, uint8_t) noexcept = 0;
    virtual unsigned long int capacity() const noexcept = 0;

    /**
     * Does initial dictionary setup, required before use for execution.
     */
    void initialize();

    Addr here() const noexcept { return read(Here); }
    void here(Addr l) noexcept { write(Here, l); }

    Addr latest() const noexcept { return read(Latest); }
    void latest(Addr l) noexcept { write(Latest, l); }

    // Aligns the given address.
    static Addr aligned(Addr);
    // Aligns `here`.
    Addr alignhere() noexcept;
    // Advances `here` by the given number of bytes.
    Addr allot(Cell) noexcept;
    // Stores value to `here`, then adds sizeof(Cell) to `here`.
    void add(Cell) noexcept;

    /**
     * Uses add() to store a new definition entry starting at `here`.
     * The entry does not become active until a semicolon is executed.
     */
    void addDefinition(Word) noexcept;

    /**
     * Searches the dictionary for an entry for the given word.
     * Returns zero if not found.
     */
    Addr find(Word) noexcept;

    /**
     * Given the address of a dictionary entry, produces the execution token
     * for that entry.
     */
    Addr getexec(Addr) noexcept;

    /**
     * Reads the next word from the input buffer.
     * Returns an empty word if the buffer is empty or entirely read.
     */
    Word input() noexcept;
    bool hasInput() const noexcept;

    /**
     * Checks if this dictionary's word is equivalent to the given string/size.
     */
    bool equal(Word, const char *, unsigned) const noexcept;

    /**
     * Checks if two words in this dictionary's word are equivalent.
     */
    bool equal(Word, Word) const noexcept;

    // Used for case-insensitive comparison between two iterators.
    template<typename Iter1, typename Iter2>
    constexpr static bool equal(Iter1 b1, Iter1 e1, Iter2 b2) {
        return std::equal(b1, e1, b2, eqchars);
    }

    virtual ~Dictionary() {};

private:
    // Case-insensitive comparison.
    constexpr static bool eqchars(char c1, char c2) {
        if (isalpha(static_cast<uint8_t>(c1)))
            c1 |= 32;
        if (isalpha(static_cast<uint8_t>(c2)))
            c2 |= 32;

        return c1 == c2;
    }

};

#endif // ALEEFORTH_DICTIONARY_HPP

