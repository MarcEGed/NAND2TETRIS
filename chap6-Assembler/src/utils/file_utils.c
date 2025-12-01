#include "file_utils.h"
#include <stdlib.h>
#include <string.h>

/**
     * @brief Creates an empty file at the specified path.
     *
     * @param path Null-terminated string: Full file path and name.
     * @return int 1 on success (file created/truncated and closed), 0 on failure.
 */
int create_file(const char* path){
    FILE* fptr;

    fptr = fopen(path, "w");
    if (fptr == NULL){
        printf("File %s couldn't be created.\n", path);
        return 0;
    }

    if (fclose(fptr) != 0) {
        printf("Error closing file %s.\n", path);
        return 0;
    }

    return 1;
}

/**
     * @brief deletes a file at the specified path.
     *
     * @param path Null-terminated string: Full file path and name.
     * @return int 1 on success (deleted file), 0 on failure.
 */
int remove_file(const char* path){
    if (remove(path) == 0){
        return 1;
    }
    return 0;
}

/**
     * @brief returns a file's content at the specified path as a string.
     *
     * @param path Null-terminated string: Full file path and name.
     * @return char* buffer on success, 0/NULL on failure.
 */
char* read_file(const char* path){
    FILE* fptr;
    long file_size = 0;
    size_t read_len = 0;
    char* buffer;

    fptr = fopen(path, "rb");
    if (fptr == NULL){
        printf("File %s couldn't be opened.\n", path);
        return 0;
    }

    if (fseek(fptr, 0, SEEK_END) != 0) {
        fprintf(stderr, "ERROR: Failed to seek end of file: %s\n", path);
        fclose(fptr);
        return NULL;
    }

    file_size = ftell(fptr);
    if (file_size == -1) {
        fprintf(stderr, "ERROR: Failed to determine file size: %s\n", path);
        fclose(fptr);
        return NULL;
    }

    rewind(fptr);

    buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate %ld bytes for file buffer.\n", file_size + 1);
        fclose(fptr);
        return NULL;
    }

    read_len = fread(buffer, 1, file_size, fptr);
    if (read_len != file_size) {
        fprintf(stderr, "ERROR: Read only %zu bytesfrom the expected %ld bytes from file: %s\n",
                read_len, file_size, path);
        free(buffer);
        fclose(fptr);
        return NULL;
    }

    buffer[file_size] = '\0';
    fclose(fptr);
    return buffer;
}

/**
     * @brief appends content to file at the specified path.
     *
     * @param path Null-terminated string: Full file path and name.
     * @param data Null-terminated string: data to add to file.
     * @return int 1 on success, 0 on failure.
 */
int write_file(const char* path, const char* data){
    FILE* fptr;

    fptr = fopen(path, "a");
    if (fptr == NULL){
        printf("File %s couldn't be created.\n", path);
        return 0;
    }

    if (fputs(data, fptr) == EOF) {
        printf("ERROR: Failed to write data to file %s.\n", path);
        fclose(fptr);
        return 0;
    }

    if (fclose(fptr) != 0) {
        printf("Error closing file %s.\n", path);
        return 0;
    }

    return 1;
}

/**
     * @brief open file at the specified path.
     *
     * @param path Null-terminated string: Full file path and name.
     * @param mode Null-terminated string: mode to open file.
     * @return fptr on success, 0 on failure.
 */
FILE* open_file(const char* path, const char* mode){
    FILE* fptr;

    fptr = fopen(path, mode);
    if (fptr == NULL){
        printf("ERRPR: Failed to open file '%s' with mode '%s'.\n", path, mode);
        return 0;
    }
    return fptr;
}

/**
     * @brief close file at the specified path.
     *
     * @param path Null-terminated string: Full file path and name.
     * @return 1 on success, 0 on failure.
 */
int close_file(FILE* file){
    if (fclose(file) != 0){
        printf("Error closing file.\n");
        return 0;
    }
    return 1;
}

/**
     * @brief checks existence of file at the specified path.
     *
     * @param path Null-terminated string: Full file path and name.
     * @return 1 on success, 0 on failure.
 */
int file_exists(const char* path){
    FILE* fptr;
    if ((fptr = fopen(path, "r"))){
        fclose(fptr);
        return 1;
    }
    return 0;
}

/**
     * @brief makes an array of lines of the file's content
     *
     * @param path Null-terminated string: Full file path and name.
     * @param out_line_count int: number of line in our result
     * @return result on success, 0/NULL on failure.
 */
char** get_file_lines(const char* path, int* out_line_count) {
    char* file_content = read_file(path);
    if (!file_content) return NULL;

    int lines = 0;
    for (char* p = file_content; *p; p++) {
        if (*p == '\n') lines++;
    }
    if (file_content[0] != '\0' && file_content[strlen(file_content) - 1] != '\n') {
        lines++;
    }

    char** result = malloc(sizeof(char*) * lines);
    if (!result) {
        free(file_content);
        return NULL;
    }

    int line_index = 0;
    char* start = file_content;
    for (char* p = file_content; *p; p++) {
        if (*p == '\n') {
            *p = '\0';
            result[line_index++] = start;
            start = p + 1;
        }
    }

    if (*start != '\0') {
        result[line_index++] = start;
    }

    if (out_line_count) *out_line_count = line_index;
    return result;
}
