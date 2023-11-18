//
/// @file dictionary.hpp
/// @brief Defines the dictionary interface and common functionality.
//
// Alee Forth: A portable and concise Forth implementation in modern C++.
// Copyright (C) 2023  Clyne Sullivan <clyne@bitgloo.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ALEEFORTH_DICTIONARY_HPP
#define ALEEFORTH_DICTIONARY_HPP

#include "config.hpp"
#include "types.hpp"
#include "ctype.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

/**
 * @class Dictionary
 * @brief Provides an interface and essential funcitonality for a dictionary.
 * @details The core read and write functionality is left virtual so that
 * dictionaries can be stored in any medium. So, this class cannot be used
 * directly; the programmer must define a dictionary class that inherits this
 * one.
 * 
 * Dictionary entry format (for a 16-bit implementation):
 *  - One information cell:
 *    - bits 0..4: Length of name
 *    - bit 5: Set if word is immediate
 *    - bits 6..15: Distance (backwards) to the next entry
 *    - If bits 6..15 are all one-bits then "long" distance in following cell.
 *  - "Length" bytes of name
 *  - Zero or more bytes for address alignment
 *  - Zero or more bytes of the definition's contents
 */
class Dictionary
{
public:
    /** Stores Numerical base to use for input/output. */
    constexpr static Addr Base       = 0;
    /** Stores the current `here` address. */
    constexpr static Addr Here       = sizeof(Cell);
    /** Stores the address of the most recently defined word. */
    constexpr static Addr Latest     = sizeof(Cell) * 2;
    /** Stores a boolean indication of compiling state. */
    constexpr static Addr Compiling  = sizeof(Cell) * 3;
    /** Stores the address of the last execution token determined by colon. */
    constexpr static Addr CompToken  = sizeof(Cell) * 4;
    /** Stores the address of the current interpreter input source. */
    constexpr static Addr Source     = sizeof(Cell) * 5;
    /** Stores the size in bytes of the interpreter input source. */
    constexpr static Addr SourceLen  = sizeof(Cell) * 6;
    /** Stores the dictionary's input buffer (a counted string). */
    constexpr static Addr Input      = sizeof(Cell) * 7;
    /** Stores the size of the dictionary's input buffer in bytes. */
    constexpr static Addr InputCells = 80;
    /** Stores the dictionary's "beginning" i.e. where new definitions begin. */
    constexpr static Addr Begin      = sizeof(Cell) * 8 + InputCells;

    /** "Immediate" marker bit for a word's definition. */
    constexpr static Cell Immediate = (1 << 5);
    /** TODO */
    constexpr static Cell Native = (1 << 6);
    /** TODO */
    constexpr static Cell DistancePos = 7;
    /** Maximum "short" distance between two definitions. */
    constexpr static Cell MaxDistance = (1 << (sizeof(Cell) * 8 - DistancePos)) - 1;

    /** Returns the value of the cell at the given address. */
    virtual Cell read(Addr) const noexcept = 0;

    /** Writes the given value to the cell at the given address. */
    virtual void write(Addr, Cell) noexcept = 0;

    /** Returns the byte stored at the given address. */
    virtual uint8_t readbyte(Addr) const noexcept = 0;

    /** Writes the given byte to the given address. */
    virtual void writebyte(Addr, uint8_t) noexcept = 0;

    /** Returns the total capacity of the dictionary in bytes. */
    virtual unsigned long int capacity() const noexcept = 0;

    /**
     * Initializes essential dictionary values.
     * Must be called before dictionary use.
     */
    void initialize();

    /**
     * Gets the address stored in `here`.
     */
    LIBALEE_SECTION
    Addr here() const noexcept { return read(Here); }

    /**
     * Sets the address stored in `here`.
     */
    LIBALEE_SECTION
    void here(Addr l) noexcept { write(Here, l); }

    /**
     * Gets the value of `latest`.
     */
    LIBALEE_SECTION
    Addr latest() const noexcept { return read(Latest); }

    /**
     * Sets the value of `latest`.
     */
    LIBALEE_SECTION
    void latest(Addr l) noexcept { write(Latest, l); }

    /**
     * Aligns the given address to the next Cell boundary if necessary.
     * @param addr The address to align.
     * @return The resulting aligned address.
     */
    static Addr aligned(Addr addr);

    /**
     * Aligns `here` to the next Cell boundary if necessary.
     * @return The new aligned address stored in `here`.
     */
    Addr alignhere() noexcept;

    /**
     * Allocates memory by returning and then increasing the current `here`.
     * @param count The number of bytes to increase `here` by.
     * @return The address stored in `here` before the increase.
     */
    Addr allot(Cell count) noexcept;

    /**
     * Stores the given value to `here` then calls allot to "save" that cell.
     * @param value The value to store.
     * @see allot(Cell)
     */
    void add(Cell value) noexcept;

    /**
     * Stores the beginning of a new word definition in the dictionary.
     * The word must eventually have its definition concluded via semicolon.
     * @param word The dictionary-stored name of the new word.
     */
    void addDefinition(Word word) noexcept;

    /**
     * Searches the dictionary for an entry for the given word.
     * @param word The dictionary-stored word to search for.
     * @return The beginning address of the word or zero if not found.
     */
    Addr find(Word word) noexcept;

    /**
     * Produces the execution token for the given dictionary entry.
     * @param addr The beginning address of a defined word.
     * @return The execution token for the given word.
     * @see find(Word)
     */
    Addr getexec(Addr addr) noexcept;

    /**
     * Reads the next word from the input buffer.
     * @return The next word or an empty word if one is not available.
     */
    Word input() noexcept;

    /**
     * Returns true if the dictionary's input buffer has  immediately available
     * data.
     */
    bool hasInput() const noexcept;

    /**
     * Checks if the dictionary-stored word is equivalent to the given string.
     * @param word Dictionary-stored word to compare against.
     * @param str String to compare to.
     * @param size Size of the string to compare to.
     * @return True if the two words are equivalent.
     */
    bool equal(Word word, const char *str, unsigned size) const noexcept;

    /**
     * Checks if two words stored in this dictionary are equivalent.
     * @param word1 First word to compare
     * @param word2 Second word to compare
     * @return True if the words are equivalent.
     */
    bool equal(Word word1, Word word2) const noexcept;

    /**
     * Generic equality comparison using our own case-insensitive comparator.
     * Arguments and return value identical to std::equal.
     */
    template<typename Iter1, typename Iter2>
    LIBALEE_SECTION
    constexpr static bool equal(Iter1 b1, Iter1 e1, Iter2 b2) {
        return std::equal(b1, e1, b2, eqchars);
    }

    virtual ~Dictionary() {};

private:
    /**
     * Case-insensitive character comparison used for dictionary lookup.
     * @return True if the characters are equivalent.
     */
    LIBALEE_SECTION
    constexpr static bool eqchars(char c1, char c2) {
        if (isalpha(static_cast<uint8_t>(c1)))
            c1 |= 32;
        if (isalpha(static_cast<uint8_t>(c2)))
            c2 |= 32;

        return c1 == c2;
    }
};

#endif // ALEEFORTH_DICTIONARY_HPP

