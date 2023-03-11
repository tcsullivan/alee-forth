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
#include "splitmemdict.hpp"

#include <charconv>
#include <fstream>
#include <iostream>
#include <vector>

#include "core.fth.h"

static bool okay = false;

static void readchar(State& state);
static void parseLine(State&, const std::string&);
static void parseFile(State&, std::istream&);

int main(int argc, char *argv[])
{
    SplitMemDict<alee_dat_len> dict (alee_dat);
    State state (dict, readchar);

    std::vector args (argv + 1, argv + argc);
    for (const auto& a : args) {
        std::ifstream file (a);
        parseFile(state, file);
    }

    okay = true;
    parseFile(state, std::cin);

    return 0;
}

static void readchar(State& state)
{
    auto idx = state.dict.read(Dictionary::Input);
    Addr addr = Dictionary::Input + sizeof(Cell) + idx;

    auto c = std::cin.get();
    if (isupper(c))
        c += 32;
    state.dict.writebyte(addr, c ? c : ' ');
}

static void save(State& state)
{
    std::ofstream file ("alee.dat", std::ios::binary);

    if (file.good()) {
        for (Addr i = 0; i < state.dict.here(); ++i)
            file.put(state.dict.readbyte(i));
    }
}

static void load(State& state)
{
    std::ifstream file ("alee.dat", std::ios::binary);

    Addr i = 0;
    while (file.good())
        state.dict.writebyte(i++, file.get());
}

void user_sys(State& state)
{
    char buf[32] = {0};

    switch (state.pop()) {
    case 0: // .
        std::to_chars(buf, buf + sizeof(buf), state.pop(),
                      state.dict.read(Dictionary::Base));
        std::cout << buf << ' ';
        break;
    case 1: // emit
        std::cout << static_cast<char>(state.pop());
        break;
    case 2: // save
        save(state);
        break;
    case 3: // load
        load(state);
        break;
    case 4: // u.
        {
        Addr ucell = static_cast<Addr>(state.pop());
        std::to_chars(buf, buf + sizeof(buf), ucell,
                      state.dict.read(Dictionary::Base));
        std::cout << buf << ' ';
        }
        break;
    default:
        break;
    }
}

void parseLine(State& state, const std::string& line)
{
    if (auto r = Parser::parse(state, line.c_str()); r == Error::none) {
        if (okay)
            std::cout << (state.compiling() ? " compiled" : " ok") << std::endl;
    } else {
        switch (r) {
        case Error::noword:
            std::cout << "word not found in: " << line << std::endl;
            break;
        case Error::push:
            std::cout << "stack overflow" << std::endl;
            break;
        case Error::pushr:
            std::cout << "return stack overflow" << std::endl;
            break;
        case Error::popr:
            std::cout << "return stack underflow" << std::endl;
            break;
        case Error::pop:
        case Error::top:
        case Error::pick:
            std::cout << "stack underflow" << std::endl;
            break;
        default:
            std::cout << "unknown error" << std::endl;
            break;
        }

        state.reset();
    }
}

void parseFile(State& state, std::istream& file)
{
    while (file.good()) {
        std::string line;
        std::getline(file, line);

        if (line == "bye")
            exit(0);

        parseLine(state, line);
    }
}

