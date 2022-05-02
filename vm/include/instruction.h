#pragma once
#include "common.h"

// This class represents the opcode of every mnemonic.
enum class Opcode: u8 {
    NOP,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    VAR,
    PUSH,
    POP,
    JMP,
    CMP,
    JGE,
    JLE,
    JGT,
    JLT,
    JEQ,
    JNE,
    CCALL,
    CPY,
    EXIT
};

struct Instruction {
    Opcode opcode;

    union {
        s32 signed_integer;
        u32 unsigned_integer;
        float floating_number;
    } value;

    Instruction();
};