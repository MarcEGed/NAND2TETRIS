#ifndef ENCODER_H
#define ENCODER_H

#include "instruction_t.h"
#include "tables.h"   // for comp_table, dest_table, jump_table
#include "string_utils.h" // if you use str_equal()
#include <stdint.h>

typedef struct SymbolTable SymbolTable;

unsigned short encode_instruction(Instruction instr);

unsigned short encode_a_instruction(int value);
unsigned short encode_c_instruction(const char* dest, const char* comp, const char* jump);

unsigned short comp_to_bits(const char* comp);
unsigned short dest_to_bits(const char* dest);
unsigned short jump_to_bits(const char* jump);

#endif