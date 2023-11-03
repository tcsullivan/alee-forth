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
#include "libalee/ctype.hpp"

#include <msp430.h>

#include "splitmemdictrw.hpp"

alignas(sizeof(Cell))
__attribute__((section(".text")))
#include "core.fth.h"

static char strbuf[80];

static void readchar(State& state);
static void serput(int c);
static void serputs(const char *s);
static void printint(DoubleCell n, char *buf);

static void initGPIO();
static void initClock();
static void initUART();
static void Software_Trim();
#define MCLK_FREQ_MHZ (8)    // MCLK = 8MHz

//__attribute__((section(".upper.bss")))
//static uint8_t hidict[16384];

int main()
{
    WDTCTL = WDTPW | WDTHOLD;
    initGPIO();
    initClock();
    initUART();
    SYSCFG0 = FRWPPW;

    (void)alee_dat_len;
    static SplitMemDictRW<sizeof(alee_dat), /*sizeof(hidict)*/16384> dict (alee_dat, 0x10000/*(uint32_t)hidict*/);
    State state (dict, readchar);

    serputs("alee forth\n\r");

    auto ptr = strbuf;
    while (1) {
        if (UCA0IFG & UCRXIFG) {
            auto c = static_cast<char>(UCA0RXBUF);
            serput(c);

            if (c == '\r') {
                *ptr = '\0';

                serputs("\n\r");

                if (auto r = Parser::parse(state, strbuf); r == Error::none) {
                    serputs(state.compiling() ? " compiled" : " ok");
                } else {
                    switch (r) {
                    case Error::noword:
                        serputs("unknown word...");
                        break;
                    default:
                        serputs("error...");
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
}

void readchar(State& state)
{
    auto idx = state.dict.read(Dictionary::Input);
    Addr addr = Dictionary::Input + sizeof(Cell) + idx;

    while (!(UCA0IFG & UCRXIFG));
    auto c = static_cast<uint8_t>(UCA0RXBUF);
    if (isupper(c))
        c += 32;
    state.dict.writebyte(addr, c ? c : ' ');
}

void serput(int c)
{
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = static_cast<char>(c);
}

void serputs(const char *s)
{
    while (*s)
        serput(*s++);
}

void printint(DoubleCell n, char *buf)
{
    char *ptr = buf;
    bool neg = n < 0;

    if (neg)
        n = -n;

    do {
        *ptr++ = static_cast<char>((n % 10) + '0');
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
    case 0: // .
        printint(state.pop(), strbuf);
        break;
    case 1: // unused
        state.push(static_cast<Addr>(state.dict.capacity() - state.dict.here()));
        break;
    case 2: // emit
        serput(state.pop());
        break;
    case 3:
        { auto addr = state.pop();
          *reinterpret_cast<uint8_t *>(addr) = state.pop() & 0xFFu; }
        break;
    case 4:
        state.push(*reinterpret_cast<uint8_t *>(state.pop()));
        break;
    default:
        break;
    }
}

void initGPIO()
{
    // Unnecessary, but done by TI example
    P1DIR = 0xFF; P2DIR = 0xFF;
    P1REN = 0xFF; P2REN = 0xFF;
    P1OUT = 0x00; P2OUT = 0x00;

    // Set LED pins to outputs
    P6DIR |= BIT0 | BIT1 | BIT2;
    P6OUT |= BIT0 | BIT1 | BIT2;
    P5DIR |= BIT5 | BIT6 | BIT7;
    P5OUT |= BIT5 | BIT6 | BIT7;

    // Setup buttons w/ pullups
    P3DIR &= ~BIT4; P3REN |= BIT4; P3OUT |= BIT4;
    P2DIR &= ~BIT3; P2REN |= BIT3; P2OUT |= BIT3;

    // XT1 pins (P2.0 and P2.1)
    //P2SEL1 |= BIT0 | BIT1;

    // Allow GPIO configurations to be applied
    PM5CTL0 &= ~LOCKLPM5;

    // Safety measure, prevent unwarranted interrupts
    P5IFG = 0;
    P6IFG = 0;
}

void initClock()
{
    __bis_SR_register(SCG0);                 // disable FLL
    CSCTL3 |= SELREF__REFOCLK;               // Set REFO as FLL reference source
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_3;// DCOFTRIM=3, DCO Range = 8MHz
    CSCTL2 = FLLD_0 + 243;                   // DCODIV = 8MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                // enable FLL
    Software_Trim();                        // Software Trim to get the best DCOFTRIM value

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                               // default DCODIV as MCLK and SMCLK source

//    // ACLK to XT1
//    do
//    {
//        CSCTL7 &= ~(XT1OFFG | DCOFFG);                // Clear XT1 and DCO fault flag
//        SFRIFG1 &= ~OFIFG;
//    }while (SFRIFG1 & OFIFG);                         // Test oscillator fault flag
//
//    CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK;  // set ACLK = XT1CLK = 32768Hz
//                                               // DCOCLK = MCLK and SMCLK source
//
//    // Now that osc is running enable fault interrupt
//    SFRIE1 |= OFIE;
}

void initUART()
{
    // Configure UART pins
    P5SEL0 |= BIT1 | BIT2;                    // set 2-UART pin as second function
    SYSCFG3|=USCIA0RMP;                       //Set the remapping source
    // Configure UART
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;

    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 17-4: UCBRSx = 0x49
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;

    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
}

void Software_Trim()
{
    unsigned int oldDcoTap = 0xffff;
    unsigned int newDcoTap = 0xffff;
    unsigned int newDcoDelta = 0xffff;
    unsigned int bestDcoDelta = 0xffff;
    unsigned int csCtl0Copy = 0;
    unsigned int csCtl1Copy = 0;
    unsigned int csCtl0Read = 0;
    unsigned int csCtl1Read = 0;
    unsigned int dcoFreqTrim = 3;
    unsigned char endLoop = 0;

    do
    {
        CSCTL0 = 0x100;                         // DCO Tap = 256
        do
        {
            CSCTL7 &= ~DCOFFG;                  // Clear DCO fault flag
        }while (CSCTL7 & DCOFFG);               // Test DCO fault flag

        __delay_cycles((unsigned int)3000 * MCLK_FREQ_MHZ);// Wait FLL lock status (FLLUNLOCK) to be stable
                                                           // Suggest to wait 24 cycles of divided FLL reference clock
        while((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        csCtl0Read = CSCTL0;                   // Read CSCTL0
        csCtl1Read = CSCTL1;                   // Read CSCTL1

        oldDcoTap = newDcoTap;                 // Record DCOTAP value of last time
        newDcoTap = csCtl0Read & 0x01ff;       // Get DCOTAP value of this time
        dcoFreqTrim = (csCtl1Read & 0x0070)>>4;// Get DCOFTRIM value

        if(newDcoTap < 256)                    // DCOTAP < 256
        {
            newDcoDelta = 256 - newDcoTap;     // Delta value between DCPTAP and 256
            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256)) // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }
        else                                   // DCOTAP >= 256
        {
            newDcoDelta = newDcoTap - 256;     // Delta value between DCPTAP and 256
            if(oldDcoTap < 256)                // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }

        if(newDcoDelta < bestDcoDelta)         // Record DCOTAP closest to 256
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    }while(endLoop == 0);                      // Poll until endLoop == 1

    CSCTL0 = csCtl0Copy;                       // Reload locked DCOTAP
    CSCTL1 = csCtl1Copy;                       // Reload locked DCOFTRIM
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)); // Poll until FLL is locked
}

