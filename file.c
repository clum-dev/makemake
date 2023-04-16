#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "file.h"


// Allocs and returns a lines struct
Lines* lines_init() {

    Lines* lines = (Lines*)malloc(sizeof(Lines));

    lines->lines = (Line*)malloc(sizeof(Line));
	lines->size = 0;

    return lines;
}

// Frees Lines struct
void lines_free(Lines* lines) {

	// printf("TEST FREE\tSIZE:%d\n", lines->size);

    for (int i = 0; i < lines->size; i++) {
        
		// printf("freeing %s\n", lines->lines[i].text);
		
		// free(lines->lines[i].text);
        // lines->lines[i].text = NULL;

		str_free(lines->lines[i].str);
    }

    free(lines->lines);
	lines->lines = NULL;

    free(lines);
    lines = NULL;
}


// Prints all parsed line args
void print_line_args(Lines lines, int index) {

	printf("[LINE]\t(index %d: linenum: %ld)\t", index, lines.lines[index].lineNum);

	if (lines.lines[index].str->text[0] == '#') {
		printf("\t[COMMENT]");
	}

	printf("\n");
}

// Prints all args of all stored lines
void print_all_line_args(Lines lines) {
	printf("\n=====\n");
	printf("Stored lines (size = %ld)\n", lines.size);
	for (int i = 0; i < lines.size; i++) {
		print_line_args(lines, i);
	}
	printf("=====\n");
}


// Attempts to open a file with a given filename
FILE* open_file(char* filename, char* mode) {
    
    FILE* file;
	// file = fopen(filename, "r");
	file = fopen(filename, mode);
	if (file == NULL) {
		fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
		exit(1);
	}

    return file;
}

// Returs true if a line is all whitespace
bool all_spaces(char* line) {

	int spaceCount = 0;
	int len = (int)strlen(line);

	// Count all whitespace chars (including tab)
	for (int i = 0; i < len; i++) {
		if (isspace(line[i])) {
			spaceCount++;
		}
	}

	// printf("len: %d\tspaces: %d\n", len, spaceCount);

	if (spaceCount == len) {
		return true;
	}

	return false;
}

// Parses a line string and stores line args in line struct
void store_line(Lines* lines, char* line, int lineNum) {
    
    // printf("line: %s\t(len:%d)\n", line, (int)strlen(line));

	// Don't bother storing blank lines
	if ((int)strlen(line) == 0 || all_spaces(line)) {
		return; 
	}

	// Realloc for each new line
	lines->lines = (Line*)realloc(lines->lines, sizeof(Line) * ++lines->size);
	if (lines->lines == NULL) {
		fprintf(stderr, "realloc err\n");
		exit(-1);
	}
	
	// Allocate text string
    // lines->lines[lines->size - 1].text = (char*)calloc(strlen(line) + 1, sizeof(char));
    // strcpy(lines->lines[lines->size - 1].text, line);

	lines->lines[lines->size - 1].str = str_init(line);

	// Store lineNum
	lines->lines[lines->size - 1].lineNum = lineNum;
}


// Gets a stringlist representation of the lines of a file
StringList* get_file_lines(FILE* file) {

	String* raw = dynamic_read(file, false);
	StringList* out;

	// Return new list if empty
	if (raw->len == 0) {
		out = strlist_init();
	} else {
	 	out = str_split(raw, "\n");
	}

	str_free(raw);
	return out;
}


// Dynamically reads a given file pointer
// If singleLine is true, then only one line is read
String* dynamic_read(FILE* file, bool singleLine) {
    
	String* str = str_init(NULL);
	char ch;
	size_t index = 0;

    while ((ch = fgetc(file)) != EOF) {
		
		// printf("%lu:\t%c\t(len: %lu)\n", index, ch, str->len);

		if (ch == '\n' && singleLine) {
			break;
		}

		if (index == 0) {
			char temp[1];
    		temp[0] = ch;
			str_set(str, temp);			

		} else {
			str_concat_char(str, ch);
		}

		index++;
    }

	// printf("string: %s\tsize: %lu\n", str->text, str->len);

    return str;
}