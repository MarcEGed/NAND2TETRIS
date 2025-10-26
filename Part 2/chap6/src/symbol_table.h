#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 255

typedef struct {
    char name[MAX_SYMBOLS];
    unsigned short bits;
} Map;

extern Map sTable[MAX_SYMBOLS];
extern int sTable_size;

void sTable_init();
void sTable_add_entry(const char* symbol, int address);
int sTable_contains(const char* symbol);
int sTable_get_address(const char* symbol);

#endif