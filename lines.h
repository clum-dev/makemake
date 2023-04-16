#ifndef __LINES_H__
#define __LINES_H__

#include <stdlib.h>

#include "strings.h"

// Struct for storing a line from a given file
typedef struct Line {

    String* str;
    size_t lineNum;

} Line;

// Struct for storing all lines a given file
typedef struct Lines {

    Line* lines;
    size_t size;
 
} Lines;


#endif