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

#include "memdict.hpp"
#include "parser.hpp"

#include <fstream>
#include <iostream>
#include <vector>

static void parseLine(Parser&, State&, std::string_view);
static void parseFile(Parser&, State&, std::istream&);

int main(int argc, char *argv[])
{
    MemDict dict;
    State state (dict);
    Parser parser;

    std::vector args (argv + 1, argv + argc);
    for (const auto& a : args) {
        std::ifstream file (a);
        parseFile(parser, state, file);
    }

    //std::cout << state.size() << ' ' << state.compiling << "> ";
    parseFile(parser, state, std::cin);

    return 0;
}

int user_sys(State& state)
{
    switch (state.pop()) {
    case 0:
        std::cout << state.pop() << std::endl;
        break;
    }

    return 0;
}

void parseLine(Parser& parser, State& state, std::string_view line)
{
    ParseStatus r;
    do {
        r = parser.parse(state, line);
    } while (r == ParseStatus::Continue);

    if (r != ParseStatus::Finished)
        std::cout << "r " << to_string(r) << std::endl;
}

void parseFile(Parser& parser, State& state, std::istream& file)
{
    while (file.good()) {
        std::string line;
        std::getline(file, line);
        parseLine(parser, state, line);
    }
}

