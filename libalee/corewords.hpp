//
/// @file corewords.hpp
/// @brief Manages the fundamental word-set and its execution.
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

#ifndef ALEEFORTH_COREWORDS_HPP
#define ALEEFORTH_COREWORDS_HPP

#include "types.hpp"
#include "state.hpp"

#include <cstring>

/**
 * To be implemented by the user, this function is called when the `sys` word
 * is executed.
 * @param state Current execution state object.
 */
void user_sys(State& state);

/**
 * @class CoreWords
 * @brief Provides the fundamental word-set and manages its execution.
 */
class CoreWords
{
public:
    /**
     * Count of total fundamental words.
     */
    constexpr static Cell WordCount = 37;

    /**
     * Token/index of the semicolon word (";").
     */
    constexpr static Cell Semicolon = 26;

    /**
     * Searches for the token/index of the given word if it is part of the
     * fundamental word-set.
     * @param state Current execution state object.
     * @param word Word (stored in state's dictionary memory) to look up.
     * @return The token/index of the word or -1 if not found.
     */
    static Cell findi(State& state, Word word);

    /**
     * Looks up the token/index of the given fundamental word.
     * Can evaluate at compile-time.
     * @param word The word to look up.
     * @return The token/index of the word or -1 if not found.
     */
    consteval static Cell findi(const char *word) {
        return findi(word, std::strlen(word));
    }

    /**
     * Executes the given execution token using the given state.
     * @param token Any valid execution token (word, fundamental, constant...).
     * @param state The state object to execute with.
     */
    static void run(Cell token, State& state);

    /**
     * String lookup table for the fundamental word-set.
     */
    constexpr static char wordsarr[] =
        "_lit\0drop\0dup\0swap\0pick\0sys\0"
        "+\0-\0m*\0_/\0_%\0"
        "_@\0_!\0>r\0r>\0=\0"
        "<\0&\0|\0^\0"
        "<<\0>>\0:\0_'\0execute\0"
        "exit\0;\0_jmp0\0_jmp\0"
        "depth\0_rdepth\0_in\0_ev\0find\0"
        "_uma\0u<\0um/mod\0";

private:
    /**
     * Generic implementation of findi(). Private; use public implementations.
     * @param it Beginning iterator of the word to search for.
     * @param size Size of the searched-for word i.e. end == it + size.
     * @return The token/index of the word or -1 if not found.
     */
    template<typename Iter>
    constexpr static Cell findi(Iter it, std::size_t size)
    {
        const char *ptr = CoreWords::wordsarr;

        for (Cell wordsi = 0; wordsi < WordCount; ++wordsi) {
            std::size_t wordsize = std::strlen(ptr);

            if (wordsize == size && Dictionary::equal(ptr, ptr + wordsize, it))
                return wordsi;

            ptr += wordsize + 1;
        }

        return -1;
    }
};

#endif // ALEEFORTH_COREWORDS_HPP

