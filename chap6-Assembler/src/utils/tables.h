#ifndef TABLES_H
#define TABLES_H

typedef struct {
    const char* name;
    unsigned short bits;
} MapEntry;

#define COMP_TABLE_SIZE 28
#define DEST_TABLE_SIZE 8
#define JUMP_TABLE_SIZE 8

extern MapEntry comp_table[];
extern MapEntry dest_table[];
extern MapEntry jump_table[];

#endif