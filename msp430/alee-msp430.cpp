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
#include "lzss.h"
static const
#include "msp430fr2476_all.h"

#include <cstring>
#include <msp430.h>

#include "splitmemdictrw.hpp"

alignas(sizeof(Cell))
__attribute__((section(".lodict")))
#include "core.fth.h"

static char strbuf[80];

static void readchar(State& state);
static void serput(int c);
static void serputs(const char *s);
static void printint(DoubleCell n, char *buf);
static Error findword(State&, Word);

static void initGPIO();
static void initClock();
static void initUART();
static void Software_Trim();
#define MCLK_FREQ_MHZ (8)    // MCLK = 8MHz

//__attribute__((section(".hidict")))
//static uint8_t hidict[16384];

static bool inISR = false;
static Addr isr_list[24] = {};
static SplitMemDictRW<sizeof(alee_dat), 16384> dict (alee_dat, 0x10000);

int main()
{
    WDTCTL = WDTPW | WDTHOLD;
    initGPIO();
    initClock();
    initUART();
    SYSCFG0 = FRWPPW;

    (void)alee_dat_len;
    State state (dict, readchar);
    Parser::customParse = findword;

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
    case 10:
        { auto index = state.pop() - 20;
          isr_list[index] = state.pop(); }
        break;
    case 11:
        { auto addr = state.pop();
          *reinterpret_cast<uint8_t *>(addr) = state.pop() & 0xFFu; }
        break;
    case 12:
        state.push(*reinterpret_cast<uint8_t *>(state.pop()));
        break;
    case 13:
        { auto addr = state.pop();
          *reinterpret_cast<uint16_t *>(addr) = state.pop() & 0xFFFFu; }
        break;
    case 14:
        state.push(*reinterpret_cast<uint16_t *>(state.pop()));
        break;
    case 15:
        if (!inISR)
            _bis_SR_register(state.pop());
        else
            _bis_SR_register_on_exit(state.pop());
        break;
    case 16:
        if (!inISR)
            _bic_SR_register(state.pop());
        else
            _bic_SR_register_on_exit(state.pop());
        break;
    default:
        break;
    }
}

#define LZSS_MAGIC_SEPARATOR (0xFB)

static char lzword[32];
static int lzwlen;
static char lzbuf[32];
static char *lzptr;

Error findword(State& state, Word word)
{
    char *ptr = lzword;
    for (auto it = word.begin(&state.dict); it != word.end(&state.dict); ++it) {
        *ptr = *it;
        if (islower(*ptr))
            *ptr -= 32;
        ++ptr;
    }
    lzwlen = (int)(ptr - lzword);

    lzptr = lzbuf;
    lzssinit(msp430fr2476_all_lzss, msp430fr2476_all_lzss_len);

    auto ret = decode([](int c) {
        if (c != LZSS_MAGIC_SEPARATOR) {
            *lzptr++ = (char)c;
        } else {
            if (lzwlen == lzptr - lzbuf - 2 && strncmp(lzword, lzbuf, lzptr - lzbuf - 2) == 0) {
                lzwlen = (*(lzptr - 2) << 8) | *(lzptr - 1);
                return 1;
            } else {
                lzptr = lzbuf;
            }
        }
        return 0;
    });

    if (ret == EOF) {
        return Error::noword;
    } else {
        Parser::processLiteral(state, (Cell)lzwlen);
        return Error::none;
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

void alee_isr_handle(unsigned index)
{
    const Addr isr = isr_list[index];

    if (isr != 0) {
        State isrstate (dict, readchar);
        inISR = true;
        isrstate.execute(isr);
        inISR = false;
    }
}

#define DEFINE_ISR(VVV, III) \
    __attribute__((interrupt(VVV))) \
    void VVV##_ISR() { alee_isr_handle(III); }

DEFINE_ISR(ECOMP0_VECTOR, 0)
DEFINE_ISR(PORT6_VECTOR, 1)
DEFINE_ISR(PORT5_VECTOR, 2)
DEFINE_ISR(PORT4_VECTOR, 3)
DEFINE_ISR(PORT3_VECTOR, 4)
DEFINE_ISR(PORT2_VECTOR, 5)
DEFINE_ISR(PORT1_VECTOR, 6)
DEFINE_ISR(ADC_VECTOR, 7)
DEFINE_ISR(EUSCI_B1_VECTOR, 8)
DEFINE_ISR(EUSCI_B0_VECTOR, 9)
DEFINE_ISR(EUSCI_A1_VECTOR, 10)
DEFINE_ISR(EUSCI_A0_VECTOR, 11)
DEFINE_ISR(WDT_VECTOR, 12)
DEFINE_ISR(RTC_VECTOR, 13)
DEFINE_ISR(TIMER0_B1_VECTOR, 14)
DEFINE_ISR(TIMER0_B0_VECTOR, 15)
DEFINE_ISR(TIMER3_A1_VECTOR, 16)
DEFINE_ISR(TIMER3_A0_VECTOR, 17)
DEFINE_ISR(TIMER2_A1_VECTOR, 18)
DEFINE_ISR(TIMER2_A0_VECTOR, 19)
DEFINE_ISR(TIMER1_A1_VECTOR, 20)
DEFINE_ISR(TIMER1_A0_VECTOR, 21)
DEFINE_ISR(TIMER0_A1_VECTOR, 22)
DEFINE_ISR(TIMER0_A0_VECTOR, 23)

