#include "utils/file_utils.h"
#include "utils/string_utils.h"
#include "utils/instruction_t.h"
#include "utils/tables.h"
#include "parser.h"
#include "encoder.h"
#include <stdio.h>
#include <string.h>

unsigned short lookup(MapEntry* table, int size, const char* key){
    for (int i = 0; i < size; i++) {
        if (str_equal(table[i].name, key)) return table[i].bits;
    }
    return 0;
}

unsigned short encode_instruction(Instruction instr) {
    if (instr.type == A_INSTRUCTION){
        return encode_a_instruction(instr.symbol);
    }else{
        return encode_c_instruction(instr.dest, instr.comp, instr.jump);
    }
}

unsigned short encode_a_instruction(const char* symbol) {
    int value = atoi(symbol);
    return (unsigned short)value;
}

unsigned short encode_c_instruction(const char* dest, const char* comp, const char* jump) {
    unsigned short code = 0b1110000000000000;
    code |= (lookup(comp_table, COMP_TABLE_SIZE, comp) << 6);
    code |= (lookup(dest_table, DEST_TABLE_SIZE, dest) << 3);
    code |= lookup(jump_table, JUMP_TABLE_SIZE, jump);
    return code;
}