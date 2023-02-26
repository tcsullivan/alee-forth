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

#include <msp430.h>

static char strbuf[32];

static void readchar(State& state);
static void serput(int c);
static void serputs(const char *s);
static void printint(int n, char *buf);

int main()
{
    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P1SEL |= BIT1 | BIT2;
    P1SEL2 |= BIT1 | BIT2;

    UCA0CTL1 = UCSWRST;
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;
    UCA0CTL1 &= (uint8_t)~UCSWRST;

    __enable_interrupt();

    static MemDict dict;
    State state (dict, readchar);
    Parser parser;

    dict.write(Dictionary::Base, 10);
    dict.write(Dictionary::Latest, Dictionary::Begin);
    dict.write(Dictionary::Compiling, 0);
    dict.write(Dictionary::Postpone, 0);

    serputs("alee forth\n\r");

    auto ptr = strbuf;
    while (1) {
        if (IFG2 & UCA0RXIFG) {
            char c = UCA0RXBUF;
            serput(c);

            if (c == '\r') {
                *ptr = '\0';

                serputs("\n\r");

                if (auto r = parser.parse(state, strbuf); r == 0) {
                    serputs(state.compiling() ? " compiled" : " ok");
                } else {
                    switch (r) {
                    case Parser::UnknownWord:
                        serputs("unknown word...");
                        break;
                    default:
                        break;
                    }
                }

                serputs("\n\r");

                ptr = strbuf;
            } else if (c == '\b') {
                if (ptr > strbuf)
                    --ptr;
            } else if (ptr < strbuf + sizeof(strbuf)) {
                if (c >= 'A' && c <= 'Z')
                    c += 32;
                *ptr++ = c;
            }
        }

    }

    return 0;
}

static void readchar(State& state)
{
    auto len = state.dict.read(Dictionary::Input);
    Addr addr = Dictionary::Input + sizeof(Cell) +
                Dictionary::InputCells - len - 1;

    for (Cell i = 0; i < len; ++i, ++addr)
        state.dict.writebyte(addr, state.dict.readbyte(addr + 1));

    while (!(IFG2 & UCA0RXIFG));
    state.dict.writebyte(addr, UCA0RXBUF);
    state.dict.write(Dictionary::Input, len + 1);
}

void serput(int c)
{
    while (!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = (char)c;
}

void serputs(const char *s)
{
    while (*s)
        serput(*s++);
}

void printint(int n, char *buf)
{
    char *ptr = buf;
    bool neg = n < 0;

    if (neg)
        n = -n;

    do {
        *ptr++ = (char)(n % 10) + '0';
    } while ((n /= 10));

    if (neg)
        serput('-');

    do {
        serput(*--ptr);
    } while (ptr > buf);
    serput(' ');
}

void user_sys(State& state)
{
    switch (state.pop()) {
    case 0:
        printint(state.pop(), strbuf);
        break;
    case 1:
        serput(state.pop());
        break;
    default:
        break;
    }
}
