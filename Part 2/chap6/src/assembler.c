#include "utils/file_utils.h"
#include "utils/instruction_t.h"
#include "parser.h"
#include "encoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    // Read lines from the ASM file
    int line_count = 0;
    char** lines = get_file_lines(asm_path, &line_count);
    if (!lines) {
        printf("Failed to read file: %s\n", asm_path);
        return 1;
    }

    // Generate output .hack file name
    char hack_path[512];
    generate_filename(asm_path, hack_path);

    FILE* out = open_file(hack_path, "w");
    if (!out) {
        printf("Failed to create output file: %s\n", hack_path);
        return 1;
    }

    // Parse, encode, and write each line
    for (int i = 0; i < line_count; i++) {
        // Skip empty lines
        if (lines[i][0] == '\0') continue;

        Instruction instr = parse_line(lines[i]);
        unsigned short binary = encode_instruction(instr);
        write_binary_line(out, binary);
    }

    close_file(out);

    // Free memory allocated by get_file_lines
    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(lines);

    printf("Output written to %s\n", hack_path);
    return 0;
}
