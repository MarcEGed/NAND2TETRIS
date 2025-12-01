#include "symbol_table.h"
#include <string.h>
#include <stdio.h>

Map sTable[MAX_SYMBOLS];
int sTable_size = 0;

void sTable_init() {
    sTable_size = 0;
    sTable_add_entry("SP", 0);
    sTable_add_entry("LCL", 1);
    sTable_add_entry("ARG", 2);
    sTable_add_entry("THIS", 3);
    sTable_add_entry("THAT", 4);
    for(int i=0;i<16;i++){
        char r[4];
        sprintf(r, "R%d", i);
        sTable_add_entry(r, i);
    }
    sTable_add_entry("SCREEN", 16384);
    sTable_add_entry("KBD", 24576);
}

int sTable_contains(const char* symbol){
    for(int i=0;i<sTable_size;i++)
        if(strcmp(sTable[i].name, symbol) == 0) return 1;
    return 0;
}

int sTable_get_address(const char* symbol){
    for(int i=0;i<sTable_size;i++)
        if(strcmp(sTable[i].name, symbol) == 0) return sTable[i].bits;
    return -1;
}

void sTable_add_entry(const char* symbol, int addr){
    strcpy(sTable[sTable_size].name, symbol);
    sTable[sTable_size].bits = addr;
    sTable_size++;
}
