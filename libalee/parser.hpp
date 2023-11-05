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

#ifndef ALEEFORTH_PARSER_HPP
#define ALEEFORTH_PARSER_HPP

#include "types.hpp"

#include <string_view>

class Parser
{
public:
    static Error (*customParse)(State&, Word);

    /**
     * Parses (and evaluates) the given string using the given state.
     * The string is stored in the state's input buffer, then parseSource()
     * works through that using parseWord(). parseWord() will compile or
     * execute as necessary.
     */
    static Error parse(State&, const char *);

    /**
     * Parses (and evaluates) through the words stored in the state's input
     * buffer.
     */
    static Error parseSource(State&);

    static void processLiteral(State&, Cell);

private:
    /**
     * Parses the given word using the given state.
     */
    static Error parseWord(State&, Word);

    /**
     * Attempts to parse the given word into a number.
     */
    static Error parseNumber(State&, Word);
};

#endif // ALEEFORTH_PARSER_HPP

