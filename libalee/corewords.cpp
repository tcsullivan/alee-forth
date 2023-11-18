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

#include <utility>

static void find(State&, Word);
static DoubleCell popd(State&);
static void pushd(State&, DoubleCell);

LIBALEE_SECTION
void CoreWords::run(Cell ins, State& state)
{
    Addr index = ins;
    auto& ip = state.ip();

execute:
    if (index >= Dictionary::Begin) {
        // must be calling a defined subroutine
        state.pushr(ip);
        ip = index;
        return;
    } else switch (index) {
    case token("_lit"): word_lit(state); break;// Execution semantics of `literal`.
    case token("drop"): word_drop(state); break;
    case token("dup"): word_dup(state); break;
    case token("swap"): word_swap(state); break;
    case token("pick"): word_pick(state); break;
    case token("sys"): word_sys(state); break; // Calls user-defined "system" handler.
    case token("+"): word_add(state); break;
    case token("-"): word_sub(state); break;
    case token("m*"): word_mul(state); break; // ( n n -- d )
    case token("_/"): word_div(state); break; // ( d n -- n )
    case token("_%"): word_mod(state); break; // ( d n -- n )
    case token("_@"): word_peek(state); break; // ( addr cell? -- n )
    case token("_!"): word_poke(state); break; // ( n addr cell? -- )
    case token(">r"): word_rpush(state); break;
    case token("r>"): word_rpop(state); break;
    case token("="): word_eq(state); break;
    case token("<"): word_lt(state); break;
    case token("&"): word_and(state); break;
    case token("|"): word_or(state); break;
    case token("^"): word_xor(state); break;
    case token("<<"): word_shl(state); break;
    case token(">>"): word_shr(state); break;
    case token(":"): word_colon(state); break; // Begins definition/compilation of new word.
    case token("_'"): word_tick(state); break; // Collects input word and finds execution token.
    case token("execute"):
        // TODO reimplement
        index = state.pop();
        goto execute;
    case token("exit"): word_exit(state); break;
    case token(";"): word_semic(state); break; // Concludes word definition.
    case token("_jmp0"): word_jmp0(state); break; // Jump if popped value equals zero.
    case token("_jmp"): word_jmp(state); break; // Unconditional jump.
    case token("depth"): word_depth(state); break;
    case token("_rdepth"): word_rdepth(state); break;
    case token("_in"): word_in(state); break; // Fetches more input from the user input source.
    case token("_ev"): word_ev(state); break; // Evaluates words from current input source.
    case token("find"): word_find(state); break;
    case token("_uma"): word_uma(state); break; // ( d u u -- d ): Unsigned multiply-add.
    case token("u<"): word_ult(state); break;
    case token("um/mod"): word_ummod(state); break;
    }

    ip += sizeof(Cell);
}

LIBALEE_SECTION
Cell CoreWords::findi(State& state, Word word)
{
    return findi(word.begin(&state.dict), word.size());
}

LIBALEE_SECTION
void find(State& state, Word word)
{
    Cell tok = 0;
    Cell imm = 0;

    if (auto j = state.dict.find(word); j > 0) {
        tok = state.dict.getexec(j);
        imm = (state.dict.read(j) & Dictionary::Immediate) ? 1 : -1;
    } else if (tok = CoreWords::findi(state, word); tok >= 0) {
        imm = (tok == CoreWords::token(";")) ? 1 : -1;
    }

    state.push(tok);
    state.push(imm);
}

DoubleCell popd(State& s)
{
    DoubleCell dcell = s.pop();
    dcell <<= sizeof(Cell) * 8;
    dcell |= static_cast<Addr>(s.pop());
    return dcell;
}

void pushd(State& s, DoubleCell d)
{
    s.push(static_cast<Cell>(d));
    s.push(static_cast<Cell>(d >> (sizeof(Cell) * 8)));
}

void CoreWords::word_lit(State& state) { // Execution semantics of `literal`.
    state.push(state.beyondip());
}
void CoreWords::word_drop(State& state) {
    state.pop();
}
void CoreWords::word_dup(State& state) {
    state.push(state.top());
}
void CoreWords::word_swap(State& state) {
    std::swap(state.top(), state.pick(1));
}
void CoreWords::word_pick(State& state) {
    state.push(state.pick(state.pop()));
}
void CoreWords::word_sys(State& state) { // Calls user-defined "system" handler.
    user_sys(state);
}
void CoreWords::word_add(State& state) {
    auto cell = state.pop();
    state.top() += cell;
}
void CoreWords::word_sub(State& state) {
    auto cell = state.pop();
    state.top() -= cell;
}
void CoreWords::word_mul(State& state) { // ( n n -- d )
    auto cell = state.pop();
    auto dcell = state.pop() * cell;
    pushd(state, dcell);
}
void CoreWords::word_div(State& state) { // ( d n -- n )
    auto cell = state.pop();
    auto dcell = popd(state);
    state.push(static_cast<Cell>(dcell / cell));
}
void CoreWords::word_mod(State& state) { // ( d n -- n )
    auto cell = state.pop();
    auto dcell = popd(state);
    state.push(static_cast<Cell>(dcell % cell));
}
void CoreWords::word_peek(State& state) { // ( addr cell? -- n )
    if (state.pop())
        state.push(state.dict.read(state.pop()));
    else
        state.push(state.dict.readbyte(state.pop()));
}
void CoreWords::word_poke(State& state) { // ( n addr cell? -- )
    auto cell = state.pop();
    if (auto addr = state.pop(); cell)
        state.dict.write(addr, state.pop());
    else
        state.dict.writebyte(addr, state.pop() & 0xFFu);
}
void CoreWords::word_rpush(State& state) {
    state.pushr(state.pop());
}
void CoreWords::word_rpop(State& state) {
    state.push(state.popr());
}
void CoreWords::word_eq(State& state) {
    auto cell = state.pop();
    state.top() = state.top() == cell ? -1 : 0;
}
void CoreWords::word_lt(State& state) {
    auto cell = state.pop();
    state.top() = state.top() < cell ? -1 : 0;
}
void CoreWords::word_and(State& state) {
    auto cell = state.pop();
    state.top() &= cell;
}
void CoreWords::word_or(State& state) {
    auto cell = state.pop();
    state.top() |= cell;
}
void CoreWords::word_xor(State& state) {
    auto cell = state.pop();
    state.top() ^= cell;
}
void CoreWords::word_shl(State& state) {
    auto cell = state.pop();
    reinterpret_cast<Addr&>(state.top()) <<= static_cast<Addr>(cell);
}
void CoreWords::word_shr(State& state) {
    auto cell = state.pop();
    reinterpret_cast<Addr&>(state.top()) >>= static_cast<Addr>(cell);
}
void CoreWords::word_colon(State& state) { // Begins definition/compilation of new word.
    state.push(state.dict.alignhere());
    state.dict.write(Dictionary::CompToken, state.top());
    while (!state.dict.hasInput())
        state.input();
    state.dict.addDefinition(state.dict.input());
    state.compiling(true);
}
void CoreWords::word_tick(State& state) { // Collects input word and finds execution token.
    while (!state.dict.hasInput())
        state.input();
    find(state, state.dict.input());
}
void CoreWords::word_execute(State& state) {
    /*index =*/ state.pop();
    /* TODO goto execute; */
}
void CoreWords::word_exit(State& state) {
    state.ip() = state.popr();
    state.verify(state.ip() != 0, Error::exit);
}
void CoreWords::word_semic(State& state) { // Concludes word definition.
    state.dict.add(token("exit"));
    state.compiling(false);

    auto cell = state.pop();
    auto dcell = cell - state.dict.latest();
    if (dcell >= Dictionary::MaxDistance) {
        // Large distance to previous entry: store in dedicated cell.
        state.dict.write(static_cast<Addr>(cell) + sizeof(Cell),
            static_cast<Cell>(dcell));
        dcell = Dictionary::MaxDistance;
    }
    state.dict.write(cell,
        (state.dict.read(cell) & 0x1F) | static_cast<Cell>(dcell << Dictionary::DistancePos));
    state.dict.latest(cell);
}
void CoreWords::word_jmp0(State& state) { // Jump if popped value equals zero.
    if (state.pop()) {
        state.beyondip();
    } else {
        state.ip() = static_cast<Addr>(state.beyondip() - sizeof(Cell));
    }
}
void CoreWords::word_jmp(State& state) { // Unconditional jump.
    state.ip() = static_cast<Addr>(state.beyondip() - sizeof(Cell));
}
void CoreWords::word_depth(State& state) {
    state.push(static_cast<Cell>(state.size()));
}
void CoreWords::word_rdepth(State& state) {
    state.push(static_cast<Cell>(state.rsize()));
}
void CoreWords::word_in(State& state) { // Fetches more input from the user input source.
    state.input();
}
void CoreWords::word_ev(State& state) { // Evaluates words from current input source.
    const auto st = state.save();
    state.ip() = 0;
    Parser::parseSource(state);
    state.load(st);
}
void CoreWords::word_find(State& state) {
    auto cell = state.pop();
    find(state,
         Word::fromLength(static_cast<Addr>(cell + 1),
                          state.dict.readbyte(cell)));
}
void CoreWords::word_uma(State& state) { // ( d u u -- d ): Unsigned multiply-add.
    const auto plus = state.pop();
    auto cell = state.pop();
    auto dcell = popd(state);
    dcell *= static_cast<Addr>(cell);
    dcell += static_cast<Addr>(plus);
    pushd(state, dcell);
}
void CoreWords::word_ult(State& state) {
    auto cell = state.pop();
    state.top() = static_cast<Addr>(state.top()) <
                  static_cast<Addr>(cell) ? -1 : 0;
}
void CoreWords::word_ummod(State& state) {
    auto cell = state.pop();
    auto dcell = popd(state);

    state.push(static_cast<Cell>(
        static_cast<DoubleAddr>(dcell) %
        static_cast<Addr>(cell)));
    state.push(static_cast<Cell>(
        static_cast<DoubleAddr>(dcell) /
        static_cast<Addr>(cell)));
}

