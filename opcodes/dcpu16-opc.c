#include <stdio.h>
#include "sysdep.h"
#include "opcode/dcpu16.h"

const struct dcpu16_opcode dcpu16_opcode_table[] = 
  {
    { "set", 0x0001, 2 },
    { "add", 0x0002, 2 },
    { "sub", 0x0003, 2 },
    { "mul", 0x0004, 2 },
    { "div", 0x0005, 2 },
    { "mod", 0x0006, 2 },
    { "shl", 0x0007, 2 },
    { "shr", 0x0008, 2 },
    { "and", 0x0009, 2 },
    { "bor", 0x000a, 2 },
    { "xor", 0x000b, 2 },
    { "ife", 0x000c, 2 },
    { "ifn", 0x000d, 2 },
    { "ifg", 0x000e, 2 },
    { "ifb", 0x000f, 2 },
    { "jsr", 0x0010, 1 },
    { "jmp", 0x01c1, 1 },
    { "bra", 0x01c2, 1 },
    { NULL, 0, 0 }
  };

const struct dcpu16_operand dcpu16_operand_table[] =
  {
    { "a",      0x00, 0, 0 },
    { "b",      0x01, 0, 0 },
    { "c",      0x02, 0, 0 },
    { "x",      0x03, 0, 0 },
    { "y",      0x04, 0, 0 },
    { "z",      0x05, 0, 0 },
    { "i",      0x06, 0, 0 },
    { "j",      0x07, 0, 0 },
    { "[a]",    0x08, 0, 0 },
    { "[b]",    0x09, 0, 0 },
    { "[c]",    0x0a, 0, 0 },
    { "[x]",    0x0b, 0, 0 },
    { "[y]",    0x0c, 0, 0 },
    { "[z]",    0x0d, 0, 0 },
    { "[i]",    0x0e, 0, 0 },
    { "[j]",    0x0f, 0, 0 },
    { "[sp++]", 0x18, 0, 0 },
    { "[sp]",   0x19, 0, 0 },
    { "[--sp]", 0x1a, 0, 0 },
    { "pop",    0x18, 0, 0 },
    { "peek",   0x19, 0, 0 },
    { "push",   0x1a, 0, 0 },
    { "sp",     0x1b, 0, 0 },
    { "pc",     0x1c, 0, 0 },
    { "o",      0x1d, 0, 0 },
    { NULL, 0, 0, 0 }
  };
