#ifndef __FILE_H__
#define __FILE_H__

#define MAX_LINE_LENGTH 256

#include <stdbool.h>
#include <stdlib.h>

#include "lines.h"

void lines_free(Lines* lines);
bool all_spaces(char* line);
FILE* open_file(char* filename, char* mode);
// Lines* get_file_lines_OLD(FILE* file);
StringList* get_file_lines(FILE* file);

void print_all_line_args(Lines lines);
String* dynamic_read(FILE* file, bool singleLine);


#endif