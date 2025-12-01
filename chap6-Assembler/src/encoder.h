#ifndef ENCODER_H
#define ENCODER_H

#include "instruction_t.h"
#include "tables.h"   // for comp_table, dest_table, jump_table
#include "string_utils.h"
#include <stdint.h>

typedef struct SymbolTable SymbolTable;

unsigned short encode_a_instruction(int value);
unsigned short encode_c_instruction(const char* dest, const char* comp, const char* jump);

#endif