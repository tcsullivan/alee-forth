//
/// @file ctype.hpp
/// @brief Simple implementations of character comparison functions.
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

#ifndef ALEEFORTH_CTYPE_HPP
#define ALEEFORTH_CTYPE_HPP

#include <cstdint>

/** Determines the length of a null-terminated string. */
constexpr inline unsigned strlen(const char * const s) {
    unsigned i = 0;
    while (s[i]) i++;
    return i;
}

/** Tests if given character represents whitespace. */
constexpr inline bool isspace(uint8_t c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

/** Tests if given character is a numerical digit. */
constexpr inline bool isdigit(uint8_t c) {
    return c >= '0' && c <= '9';
}

/** Tests if given character is an uppercase letter. */
constexpr inline bool isupper(uint8_t c) {
    return c >= 'A' && c <= 'Z';
}

/** Tests if given character is a lowercase letter. */
constexpr inline bool islower(uint8_t c) {
    return c >= 'a' && c <= 'z';
}

/** Tests if given character is a letter. */
constexpr inline bool isalpha(uint8_t c) {
    return isupper(c) || (c >= 'a' && c <= 'z');
}

#endif // ALEEFORTH_CTYPE_HPP

