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

#include <iostream>

// : variable create 0 , ;
// : create here constant ;
// : constant

int main()
{
    MemDict dict;
    State state (dict);
    Parser parser;

    for (;;) {
	std::string line;
	std::cout << state.size() << ' ' << state.compiling << "> ";
	std::getline(std::cin, line);

	ParseStatus r;
	std::string_view lv (line);
	do {
	    r = parser.parse(state, lv);
	} while (r == ParseStatus::Continue);

	if (r != ParseStatus::Finished)
	    std::cout << "r " << to_string(r) << std::endl;
    }

    return 0;
}

int user_sys(State& state)
{
    const auto value = state.pop();
    std::cout << value << std::endl;
    return 0;
}

