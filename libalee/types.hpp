//
/// @file types.hpp
/// @brief Defines common types used throughout Alee Forth.
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

#ifndef ALEEFORTH_TYPES_HPP
#define ALEEFORTH_TYPES_HPP

#include <cstdint>
#include <iterator>

/** Dictionary address type. Must be equivalent to "unsigned Cell". */
using Addr = uint16_t;
/** Data cell type. Dictionary is basically an array of this type. */
using Cell = int16_t;
/** Double-width cell type. Must be twice the size of Cell. */
using DoubleCell = int32_t;
/** Double-width addr type. Must be twice the size of Addr. Used by um/mod. */
using DoubleAddr = uint32_t;

struct Dictionary;
struct State;

/**
 * Error enum to report success or failure of parsing or execution.
 */
enum class Error : int {
    none = 0, /** No error */
    push,  /** Could not push (data stack overflow) */
    pop,   /** Could not pop (data stack underflow) */
    pushr, /** Could not push (return stack overflow) */
    popr,  /** Could not pop (return stack underflow) */
    top,   /** Could not fetch data stack top (data stack underflow) */
    pick,  /** Could not pick data stack value (data stack underflow) */
    exit,  /** No error, exited from State::execute() */
    noword /** Parsing failed because the word was not found */
};

/**
 * @class Word
 * Stores the beginning and end indices of a dictionary-defined word.
 */
class Word
{
    /** Beginning (inclusive) index */
    Addr start;
    /** End (exclusive) index */
    Addr wend;

public:
    struct iterator;

    /**
     * Constructs a word with the given start and end indices.
     * @param s Start/beginning index
     * @param e (past-the-)end index
     */
    constexpr explicit Word(Addr s = 0, Addr e = 0):
        start(s), wend(e) {}

    /**
     * Constructs a word using beginning index and length.
     * @param s Beginning/start index of word
     * @param l Count of bytes until end of word
     * @return Resulting Word object
     */
    LIBALEE_SECTION
    static constexpr Word fromLength(Addr s, Addr l) {
        return Word(s, s + l);
    }

    /** Returns the size of this word in bytes. */
    Addr size() const noexcept;

    /**
     * Creates a beginning iterator for the word.
     * @param dict Pointer to dictionary object containing this word
     * @return Iterator pointing to this word's beginning
     */
    iterator begin(const Dictionary *dict);

    /**
     * Creates an end iterator for the word.
     * @param dict Pointer to dictionary object containing this word
     * @return Iterator pointing to past-the-end of this word
     */
    iterator end(const Dictionary *dict);

    /**
     * Forward-iterator for iterating through the letters of this word.
     */
    struct iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = uint8_t;
        using pointer = void;
        using reference = void;
        using difference_type = Cell;

        /** Iterator's current address within its containing dictionary. */
        Addr addr;
        /** Pointer to dictionary that contains this word. */
        const Dictionary *dict;

        /**
         * Constructs a word iterator.
         * @param a The address the iterator points to
         * @param d The dictionary that contains this word
         */
        constexpr iterator(Addr a, const Dictionary *d):
            addr(a), dict(d) {}

        /** Prefix increment */
        iterator& operator++();
        /** Postfix increment */
        iterator operator++(int);
        /** Returns value pointed to by iterator */
        value_type operator*();
        /** Iterator comparison function (case-insensitive) */
        bool operator!=(const iterator&);
    };
};

#endif // ALEEFORTH_TYPES_HPP

