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

static void readchar(State& state);
static void parseLine(Parser&, State&, const std::string&);
static void parseFile(Parser&, State&, std::istream&);

int main(int argc, char *argv[])
{
    MemDict dict;
    State state (dict, readchar);
    Parser parser;

    dict.write(Dictionary::Base, 10);
    dict.write(Dictionary::Latest, Dictionary::Begin);
    dict.write(Dictionary::Compiling, 0);
    dict.write(Dictionary::Postpone, 0);

    std::vector args (argv + 1, argv + argc);
    for (const auto& a : args) {
        std::ifstream file (a);
        parseFile(parser, state, file);
    }

    okay = true;
    parseFile(parser, state, std::cin);

    return 0;
}

static void readchar(State& state)
{
    auto len = state.dict.read(Dictionary::Input);
    Addr addr = Dictionary::Input + sizeof(Cell) +
                Dictionary::InputCells - len - 1;

    for (Addr i = 0; i < len; ++i, ++addr)
        state.dict.writebyte(addr, state.dict.readbyte(addr + 1));

    auto c = std::cin.get();
    state.dict.writebyte(addr, c ? c : ' ');
    state.dict.write(Dictionary::Input, len + 1);
}

static void save(State& state)
{
    std::ofstream file ("alee.dat", std::ios::binary);

    if (file.good()) {
        for (Addr i = 0; i < state.dict.here; ++i)
            file.put(state.dict.readbyte(i));
    }
}

static void load(State& state)
{
    std::ifstream file ("alee.dat", std::ios::binary);

    Addr i = 0;
    while (file.good())
        state.dict.writebyte(i++, file.get());

    state.dict.here = i - 1;
}

void user_sys(State& state)
{
    switch (state.pop()) {
    case 0:
        std::cout << state.pop() << ' ';
        break;
    case 1:
        std::cout << static_cast<char>(state.pop());
        break;
    case 2:
        save(state);
        break;
    case 3:
        load(state);
        break;
    }
}

void parseLine(Parser& parser, State& state, const std::string& line)
{
    if (auto r = parser.parse(state, line.c_str()); r == 0) {
        if (okay)
            std::cout << (state.compiling() ? "compiled" : "ok") << std::endl;
    } else {
        switch (r) {
        case Parser::UnknownWord:
            std::cout << "word not found in: " << line << std::endl;
            break;
        case static_cast<int>(State::Error::push):
            std::cout << "stack overflow" << std::endl;
            break;
        case static_cast<int>(State::Error::pushr):
            std::cout << "return stack overflow" << std::endl;
            break;
        case static_cast<int>(State::Error::popr):
            std::cout << "return stack underflow" << std::endl;
            break;
        case static_cast<int>(State::Error::pop):
        case static_cast<int>(State::Error::top):
        case static_cast<int>(State::Error::pick):
            std::cout << "stack underflow" << std::endl;
            break;
        default:
            std::cout << "error: " << r << std::endl;
            break;
        }
    }
}

void parseFile(Parser& parser, State& state, std::istream& file)
{
    while (file.good()) {
        std::string line;
        std::getline(file, line);
        if (line == "bye")
            exit(0);

        parseLine(parser, state, line);
    }
}

