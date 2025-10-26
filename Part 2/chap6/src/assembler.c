#include "utils/file_utils.h"
#include "utils/instruction_t.h"
#include "parser.h"
#include "encoder.h"
#include "symbol_table.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_numeric(const char* s) {
    if (!s || !s[0]) return 0;
    for (int i = 0; s[i]; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

void generate_filename(const char* asm_path, char* hack_path) {
    strcpy(hack_path, asm_path);
    char* dot = strrchr(hack_path, '.');
    if (dot) {
        strcpy(dot, ".hack");
    } else {
        strcat(hack_path, ".hack");
    }
}

void write_binary_line(FILE* out, unsigned short code) {
    for (int i = 15; i >= 0; i--) {
        fputc((code & (1 << i)) ? '1' : '0', out);
    }
    fputc('\n', out);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s file.asm\n", argv[0]);
        return 1;
    }

    const char* asm_path = argv[1];

    int line_count = 0;
    char** lines = get_file_lines(asm_path, &line_count);
    if (!lines) {
        printf("Failed to read file: %s\n", asm_path);
        return 1;
    }

    char hack_path[512];
    generate_filename(asm_path, hack_path);

    // ==========================
    // FIRST PASS: LABELS
    // ==========================
    sTable_init();
    int rom_addr = 0;

    for (int i = 0; i < line_count; i++) {
        Instruction instr = parse_line(lines[i]);

        if (instr.type == L_INSTRUCTION) {
            if (!sTable_contains(instr.symbol)) {
                sTable_add_entry(instr.symbol, rom_addr);
            }
        } else if (instr.type == A_INSTRUCTION || instr.type == C_INSTRUCTION) {
            rom_addr++;
        }
    }

    // ==========================
    // SECOND PASS: ENCODE
    // ==========================
    FILE* out = open_file(hack_path, "w");
    if (!out) {
        printf("Failed to create output file: %s\n", hack_path);
        for (int i = 0; i < line_count; i++) free(lines[i]);
        free(lines);
        return 1;
    }

    int next_var_addr = 16; // RAM start for variables

    for (int i = 0; i < line_count; i++) {
        Instruction instr = parse_line(lines[i]);

        if (instr.type == L_INSTRUCTION) continue; // skip labels

        if (instr.type == A_INSTRUCTION) {
            if (is_numeric(instr.symbol)) {
                instr.value = atoi(instr.symbol);
            } else {
                if (!sTable_contains(instr.symbol)) {
                    sTable_add_entry(instr.symbol, next_var_addr++);
                }
                instr.value = sTable_get_address(instr.symbol);
            }
        }

        unsigned short binary;
        if (instr.type == A_INSTRUCTION) {
            binary = encode_a_instruction(instr.value);
        } else if (instr.type == C_INSTRUCTION) {
            binary = encode_c_instruction(instr.dest, instr.comp, instr.jump);
        }

        write_binary_line(out, binary);
    }

    close_file(out);

    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(lines);

    printf("Output written to %s\n", hack_path);
    return 0;
}
