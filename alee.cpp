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

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

static void compile(State&);

static bool okay = false;

static void readchar(State&);
static void parseLine(State&, const std::string&);
static void parseFile(State&, std::istream&);

int main(int argc, char *argv[])
{
    MemDict dict;
    State state (dict, readchar);

    dict.initialize();

    {
        std::vector args (argv + 1, argv + argc);
        for (const auto& a : args) {
            std::ifstream file (a);
            parseFile(state, file);
        }
    }

    okay = true;
    parseFile(state, std::cin);

    return 0;
}

static void readchar(State& state)
{
    auto idx = state.dict.read(Dictionary::Input);
    auto addr = static_cast<Addr>(Dictionary::Input + sizeof(Cell) + idx);

    auto c = static_cast<char>(std::cin.get());
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

    for (Addr i = 0; file.good(); i++)
        state.dict.writebyte(i, file.get() & 0xFFu);
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
    case 1: // u.
        {
        Addr ucell = static_cast<Addr>(state.pop());
        std::to_chars(buf, buf + sizeof(buf), ucell,
                      state.dict.read(Dictionary::Base));
        std::cout << buf << ' ';
        }
        break;
    case 2: // emit
        std::cout << static_cast<char>(state.pop());
        break;
    case 3: // save
        save(state);
        break;
    case 4: // load
        load(state);
        break;
    case 5: // compile
        compile(state);
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

// Prints all compiled words, their start addresses, and their "disassembly".
// Hopefully, it won't be too difficult to translate these into LLVM IR.
void compile(State& state)
{
    auto& dict = state.dict;

    Addr latest = dict.latest();
    Addr attr = 0;
    do {
        Addr oldlen = attr >> 6;
        latest -= oldlen;

        attr = dict.read(latest);
        auto lw = Word::fromLength(latest + sizeof(Cell), attr & 0x1F);

        if (!(attr & Dictionary::Immediate)) {
            Addr start = dict.getexec(latest);
            Addr len = oldlen;
            len -= start;
            len += latest;

            std::for_each(lw.begin(&dict), lw.end(&dict), putchar);
            std::cout << " @ " << start << std::endl;

            for (Addr i = 0; i < len; i += sizeof(Cell)) {
                Addr addr = start;
                addr += i;
                std::cout << '\t' << (Addr)dict.read(addr) << ' ';
            }
            std::cout << std::endl;
        }
    } while (latest != Dictionary::Begin);
}

