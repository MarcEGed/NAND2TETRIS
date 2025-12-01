#include <stdio.h>
#include "string_utils.h"

size_t strlen_custom(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') len++;
    return len;
}

char* trim(char* str) {
    if (!str) return NULL;

    while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n'){ //leading
        str++;
    } 

    char* end = str + strlen_custom(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {//trailing
        *end = '\0';
        end--;
    }
    return str;
}

char* remove_comments(char* str) {
    if (!str){
        return NULL;
    }

    char* p = str;
    while (*p) {
        if (*p == '/' && *(p + 1) == '/') {
            *p = '\0';
            break;
        }
        p++;
    }
    return str;
}

char start_char(const char* str) {
    if (!str){
        return 0;
    } 
    return str[0];
}

char** split(const char* str, char sep, int* out_count) {
    if (!str) return NULL;

    // Count tokens
    int count = 1;
    for (const char* p = str; *p; p++) {
        if (*p == sep) count++;
    }

    char** result = malloc(sizeof(char*) * count);
    if (!result) return NULL;

    // Make a mutable copy
    size_t len = strlen_custom(str);
    char* temp = malloc(len + 1);
    if (!temp) {
        free(result);
        return NULL;
    }
    for (size_t i = 0; i <= len; i++) temp[i] = str[i];

    int index = 0;
    char* token = temp;
    for (size_t i = 0; i <= len; i++) {
        if (temp[i] == sep || temp[i] == '\0') {
            temp[i] = '\0';
            result[index++] = token;
            token = temp + i + 1;
        }
    }

    if (out_count) *out_count = index;
    return result;
}

int str_equal(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}
