#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdlib.h>

char* trim(char* str);
char* remove_comments(char* str);
char start_char(const char* str);
char** split(const char* str, char sep, int* out_count);
int str_equal(const char* a, const char* b);
int is_numeric(const char* str);
size_t len_str(const char* str);

#endif