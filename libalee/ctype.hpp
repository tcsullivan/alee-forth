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

#ifndef ALEEFORTH_CTYPE_HPP
#define ALEEFORTH_CTYPE_HPP

/**
 * We implement our own character comparison functions to keep them lean.
 */

#include <cstdint>

bool isspace(uint8_t);
bool isdigit(uint8_t);
bool isalpha(uint8_t);
bool isupper(uint8_t);

#endif // ALEEFORTH_CTYPE_HPP

