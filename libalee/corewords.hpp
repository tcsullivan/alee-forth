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

#include "types.hpp"
#include "state.hpp"

void user_sys(State&);

class CoreWords
{
public:
    constexpr static std::size_t WordCount = 32;

    constexpr static Cell Immediate = (1 << 5);

    constexpr static int Semicolon = 26;

    static int findi(const char *);
    static int findi(State&, Word);
    static void run(unsigned int, State&);

private:
    constexpr static char wordsarr[] =
        "_lit\0drop\0dup\0swap\0pick\0sys\0"
        "+\0-\0m*\0_/\0_%\0"
        "_@\0_!\0>r\0r>\0=\0"
        "<\0&\0|\0^\0"
        "<<\0>>\0:\0_'\0execute\0"
        "exit\0;\0_jmp0\0_jmp\0"
        "depth\0_rdepth\0_in\0";
};

#endif // ALEEFORTH_COREWORDS_HPP
