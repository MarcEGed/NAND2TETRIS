#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>

int create_file(const char* path);
int remove_file(const char* path);
char* read_file(const char* path);
int write_file(const char* path, const char* data);
FILE* open_file(const char* path, const char* mode);
int close_file(FILE* file);
int file_exists(const char* path);
char** get_file_lines(const char* path, int* out_line_count);


#endif