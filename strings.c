#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "strings.h"


#define STR_LIB_VERSION 0.1

// Debug for tracking changes made to this library
void str_lib_version() {
    float version = STR_LIB_VERSION;
    printf("StrLib by clum:\tVersion %f\n", version);
}


// Initialises a string struct (sets to given text val if given)
String* str_init(char* text) {

    String* str = (String*)malloc(sizeof(String));

    int len = 0;
    if (text != NULL) {
        len = strlen(text);
    }
    str->len = len;

    if (len == 0) {
        str->text = NULL;
    } else {
        str->text = calloc(len + 1, sizeof(char));
        strcpy(str->text, text);
    }

    return str;
}

// Frees a string struct
void str_free(String* str) {
    
    if (str != NULL) {
        if (str->text != NULL) {
            free(str->text);
            str->text = NULL;
        }
        str->len = 0;

        free(str);
        str = NULL;
    }     
}

// Prints a string struct
void str_print(String* str, bool newline) {
    if (str == NULL) {
        printf("\"(null)\"");
    } else {
        printf("\"%s\"", str->text);
    }
    if (newline) {
        printf("\n");
    }
}

// Sets the string value in a given string struct
void str_set(String* str, char* text) {

    int len = 0;
    if (text != NULL) {
        len = strlen(text);
    }
    str->len = len;
    str->text = (char*)realloc(str->text, sizeof(char) * (len + 1));

    if (len == 0) {
        strcpy(str->text, "\0");
    } else {
        strcpy(str->text, text);
    }
}

// Clears the text value stored in a given string struct
void str_clear(String* str) {
    str_set(str, NULL);
}

// Concatenates a string with another string
void str_concat_text(String* str, char* text) {

    if (text != NULL) {
        
        // Don't concat on a null string
        if (str->text == NULL) {
            str_set(str, text);
            
        } else {
            int newLen = strlen(text);

            // printf("curr len = %d\n", (int)str->len);
            // printf("new len = %d\n", (int)str->len + newLen);

            str->text = (char*)realloc(str->text, sizeof(char) * (str->len + newLen + 1));
            strncat(str->text, text, newLen);

            str->len = str->len + newLen;
        }       
    }
}

// Concatenates a single char to a given string
void str_concat_char(String* str, char chr) {
    char temp[1];
    temp[0] = chr;
    str_concat_text(str, temp);
}

// Converts string to lowercase -> creates a new string
String* str_to_lower(String* str) {
    String* out = str_init(str->text);
    
    for (size_t i = 0; i < out->len; i++) {
        out->text[i] = tolower(out->text[i]);
    }

    return out;
}

// Converts string to uppercase -> creates a new string
String* str_to_upper(String* str) {
    String* out = str_init(str->text);
    
    for (size_t i = 0; i < out->len; i++) {
        out->text[i] = toupper(out->text[i]);
    }

    return out;
}

// Checks if two strings are equal
bool str_equals(String* str1, String* str2, bool caseSensitive) {
    
    bool equals = false;
    
    if (!caseSensitive) {
        String* temp1 = str_to_lower(str1);
        String* temp2 = str_to_lower(str2);

        equals = !strcmp(temp1->text, temp2->text);

        str_free(temp1); 
        str_free(temp2);

    } else {
        equals = !strcmp(str1->text, str2->text);
    }
    
    return equals;
}

// Checks if two strings are equal
bool str_equals_text(String* str1, char* str2, bool caseSensitive) {

    if (str1 == NULL) printf("str1 is null\n");
    if (str2 == NULL) printf("str2 is null\n");

    String* temp = str_init(str2);
    bool out = str_equals(str1, temp, caseSensitive);
    str_free(temp);

    return out;
}


/* LIST STUFF */

// Initialises a stringlist struct
StringList* strlist_init() {

    StringList* list = (StringList*)malloc(sizeof(StringList));
    list->len = 0;

    list->strings = NULL;

    return list;
}

// Frees a stringlist struct
void strlist_free(StringList* list) {

    if (list != NULL) {

        if (list->strings != NULL) {

            for (int i = 0; i < list->len; i++) {
                str_free(list->strings[i]);
                list->strings[i] = NULL;
            }

            free(list->strings);
            list->strings = NULL;
        }

        list->len = 0;

        free(list);
        list = NULL;
    }
}

// Prints a stringlist struct
void strlist_print(StringList* list) {

    printf("StrList: (len = %d)\n", (int)list->len);
    for (int i = 0; i < list->len; i++) {
        str_print(list->strings[i], true);
    }

}

// Adds a string to a given stringlist
void strlist_add(StringList* list, String* text) {

    if (list->strings == NULL) {
        list->strings = (String**)malloc(sizeof(String*) * ++list->len);
    } else {
        list->strings = (String**)realloc(list->strings, sizeof(String*) * ++list->len);
    }

    list->strings[list->len - 1] = text;
}

// Splits a string (by given delimiters) into a stringlist
StringList* str_split(String* src, char* delims) {

    StringList* list = strlist_init();
    
    char* token = strtok(src->text, delims);
    size_t index = 0;

    while (token != NULL) {
      
      strlist_add(list, str_init(token));
      token = strtok(NULL, delims);
      index++;

    }

    return list;
}

// Joins a stringlist (by given separators) into a single string
String* strlist_join(StringList* list, char separator) {

    String* out;
    for (size_t i = 0; i < list->len; i++) {
        if (i == 0) {
            out = str_init(list->strings[i]->text);
        } else {
            str_concat_text(out, list->strings[i]->text);
        }
        
        if (separator != '\0' && i != list->len - 1) {
            str_concat_char(out, separator);
        }
    }

    return out;
}

// Clones a stringlist
StringList* strlist_clone(StringList* src) {

    if (src == NULL) {
        return NULL;
    }

    StringList* out = strlist_init();

    for (size_t i = 0; i < src->len; i++) {
        strlist_add(out, str_init(src->strings[i]->text));
    }

    return out;
}

bool strlist_contains(StringList* list, String* str, bool caseSensitive) {
    for (size_t i = 0; i < list->len; i++) {
        if (str_equals(list->strings[i], str, caseSensitive)) {
            return true;
        }
    }
    return false;
}