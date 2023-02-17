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
#include "memdict.hpp"

#include <fstream>
#include <iostream>
#include <vector>

static bool okay = false;

static void parseLine(Parser&, State&, std::string_view);
static void parseFile(Parser&, State&, std::istream&);

static void readchar(State& state)
{
    auto len = state.dict.read(Dictionary::Input);
    Addr addr = Dictionary::Input + sizeof(Cell) +
                Dictionary::InputCells - len - 1;

    for (Addr i = 0; i < len; ++i, ++addr)
        state.dict.writebyte(addr, state.dict.readbyte(addr + 1));

    state.dict.writebyte(addr, std::cin.get());
    state.dict.write(Dictionary::Input, len + 1);
}

int main(int argc, char *argv[])
{
    MemDict dict;
    State state (dict, readchar);
    Parser parser;

    std::vector args (argv + 1, argv + argc);
    for (const auto& a : args) {
        std::ifstream file (a);
        parseFile(parser, state, file);
    }

    okay = true;
    parseFile(parser, state, std::cin);

    return 0;
}

void user_sys(State& state)
{
    switch (state.pop()) {
    case 0:
        std::cout << state.pop() << std::endl;
        break;
    case 1:
        std::cout << static_cast<char>(state.pop()) << std::endl;
        break;
    }
}

void parseLine(Parser& parser, State& state, std::string_view line)
{
    auto r = parser.parse(state, line);

    if (r == ParseStatus::Finished) {
        if (okay)
            std::cout << " ok" << std::endl;
    } else {
        std::cout << to_string(r) << ": " << line << std::endl;
    }
}

void parseFile(Parser& parser, State& state, std::istream& file)
{
    while (file.good()) {
        std::string line;
        std::getline(file, line);
        parseLine(parser, state, line);
    }
}

