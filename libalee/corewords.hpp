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

#ifndef ALEEFORTH_COREWORDS_HPP
#define ALEEFORTH_COREWORDS_HPP

#include "config.hpp"
#include "types.hpp"
#include "dictionary.hpp"

/**
 * To be implemented by the user, this function is called when the `sys` word
 * is executed.
 */
void user_sys(State&);

class CoreWords
{
public:
    constexpr static Cell WordCount = 37;
    constexpr static Cell Semicolon = 26;

    /**
     * Finds execution token that corresponds to the given word.
     * Returns -1 if not found.
     */
    static Cell findi(State&, Word);
    consteval static Cell findi(const char *word) {
        return findi(word, strlen(word));
    }

    /**
     * Executes the given CoreWord execution token using the given state.
     */
    static void run(Cell, State&);

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
    template<typename Iter>
    LIBALEE_SECTION
    constexpr static Cell findi(Iter it, std::size_t size)
    {
        const char *ptr = CoreWords::wordsarr;

        for (Cell wordsi = 0; wordsi < WordCount; ++wordsi) {
            std::size_t wordsize = strlen(ptr);

            if (wordsize == size && Dictionary::equal(ptr, ptr + wordsize, it))
                return wordsi;

            ptr += wordsize + 1;
        }

        return -1;
    }
};

#endif // ALEEFORTH_COREWORDS_HPP

