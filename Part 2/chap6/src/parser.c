#include "utils/file_utils.h"
#include "utils/string_utils.h"
#include "utils/instruction_t.h"

#include <stdio.h>
#include <string.h>
#include "parser.h"

int isA_inst(char* line){
    if (start_char(line) == '@'){
        return 1;
    }
    return 0;
}

int isL_inst(char* line) {
    line = trim(line);
    return (line[0] == '(' && line[strlen(line) - 1] == ')');
}

Instruction parse_line(char* line){
    Instruction instr = {0};

    line = trim(line);
    line = remove_comments(line);

    if (isA_inst(line)){
        instr.type = A_INSTRUCTION;
        int i = 1, j = 0;
        while (line[i] != '\0' && line[i] != '\n' && j < (int)(sizeof(instr.symbol) - 1)) {
            instr.symbol[j++] = line[i++];
        }
        instr.symbol[j] = '\0';

    }else if (isL_inst(line)){
        instr.type = L_INSTRUCTION;
        size_t len = strlen(line);
        if (len > 2){
            strncpy(instr.symbol, line+1, len-2);
            instr.symbol[len-2] = '\0';
        }
    }else{
        instr.type = C_INSTRUCTION;
        instr.dest[0] = '\0'; 
        instr.comp[0] = '\0';
        instr.jump[0] = '\0';

        // Split by '=' first
        int eq_count = 0;
        char** eq_parts = split(line, '=', &eq_count);

        char* dest_part = "";
        char* comp_jump_part = "";

        if (eq_count == 2) {
            dest_part = eq_parts[0];
            comp_jump_part = eq_parts[1];
        } else { // no '='
            comp_jump_part = eq_parts[0];
        }

        // Split comp/jump by ';'
        int sc_count = 0;
        char** sc_parts = split(comp_jump_part, ';', &sc_count);

        char* comp_part = "";
        char* jump_part = "";

        if (sc_count == 2) {
            comp_part = sc_parts[0];
            jump_part = sc_parts[1];
        } else {
            comp_part = sc_parts[0];
        }

        int i;
        for (i = 0; dest_part[i] && i < (int)(sizeof(instr.dest) - 1); i++)
            instr.dest[i] = dest_part[i];
        instr.dest[i] = '\0';

        for (i = 0; comp_part[i] && i < (int)(sizeof(instr.comp) - 1); i++)
            instr.comp[i] = comp_part[i];
        instr.comp[i] = '\0';

        for (i = 0; jump_part[i] && i < (int)(sizeof(instr.jump) - 1); i++)
            instr.jump[i] = jump_part[i];
        instr.jump[i] = '\0';
    }

    return instr;
}
