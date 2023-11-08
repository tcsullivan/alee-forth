//
/// @file parser.hpp
/// @brief Provides functions to parse text for interpretation/execution.
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

#ifndef ALEEFORTH_PARSER_HPP
#define ALEEFORTH_PARSER_HPP

#include "types.hpp"

#include <string_view>

/**
 * @class Parser
 * @brief Provides routines for parsing Forth code.
 */
class Parser
{
public:
    /**
     * Pointer to a user-provided function that
     */
    static Error (*customParse)(State&, Word);

    /**
     * Parses and evaluates the given string using the given state.
     * The string is stored in the state's input buffer before parseSource()
     * is called.
     * @param state The state to parse and evaluate with.
     * @param str The string to parse.
     * @return Error token to indicate if parsing was successful.
     * @see parseSource(State&)
     */
    static Error parse(State& state, const char *str);

    /**
     * Parses through and compiles or evaluates the words stored in the state's
     * input source.
     * @param state The state to parse with.
     * @return Error token to indicate if parsing was successful.
     * @see parseWord(State&, Word)
     */
    static Error parseSource(State& state);

    /**
     * Parses the given value and either pushes it to the stack or compiles
     * that functionality.
     * @param state The state to give the value to.
     * @param value The value to process.
     */
    static void processLiteral(State& state, Cell value);

private:
    /**
     * Parses the given word using the given state.
     * @return Error token to indicate if parsing was successful.
     */
    static Error parseWord(State&, Word);

    /**
     * Attempts to parse the given word into a number.
     * @param state The state object with the dictionary containing the word.
     * @param word The dictionary-stored word (number) to parse.
     * @return Error token to indicate if parsing was successful.
     */
    static Error parseNumber(State& state, Word word);
};

#endif // ALEEFORTH_PARSER_HPP

