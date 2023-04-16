#include <stdio.h>
#include <stdlib.h>

#include "makemake.h"
#include "file.h"
#include "strings.h"

/**
 * TODO:    automatic makefile maker
 *  -   generate bash scripts for running the program
 * 
*/


//
String* get_objs_tag(Source* source) {
    String* out = str_init("OBJS");
    str_concat_char(out, '_');
    str_concat_text(out, source->name->text);

    return out;
}

//
String* get_objs_str(Source* source) {

    String* out = str_init(source->objsTag->text);

    str_concat_char(out, '=');
    str_concat_text(out, source->file->text);
    
    for (size_t i = 0; i < source->deps->size; i++) {
        str_concat_char(out, ' ');
        str_concat_text(out, source->deps->sources[i]->object->text);
    }
    
    return out;
}

//
Source* source_init(String* file, StringList* deps) {
    
    Source* source = (Source*)malloc(sizeof(Source));

    source->file = str_init(file->text);

    StringList* split = str_split(file, ".");
    source->name = str_init(split->strings[0]->text);
    source->header = str_init(split->strings[0]->text);
    source->object = str_init(split->strings[0]->text);
    strlist_free(split);

    str_concat_text(source->header, ".h");
    str_concat_text(source->object, ".o");

    source->deps = sources_init();
    if (deps != NULL) {
        for (size_t i = 0; i < deps->len; i++) {
            sources_add(source->deps, source_init(deps->strings[i], NULL));
        }
    }

    source->objsTag = get_objs_tag(source);
    // source->objsStr = get_objs_str(source);
    source->objsStr = NULL;

    return source;
}

//
void source_free(Source* source) {

    if (source != NULL) {

        if (source->name != NULL) {
            str_free(source->name);
        }

        if (source->file != NULL) {
            str_free(source->file);
        }

        if (source->header != NULL) {
            str_free(source->header);
        }

        if (source->object != NULL) {
            str_free(source->object);
        }

        if (source->deps != NULL) {
            sources_free(source->deps);
        }

        if (source->objsStr != NULL) {
            str_free(source->objsStr);
        }

        if (source->objsTag != NULL) {
            str_free(source->objsTag);
        }

        free(source);
        source = NULL;
    }
}

//
void source_print(Source* source) {

    str_print(source->file, true);
    if (source->deps != NULL) {
        for (size_t i = 0; i < source->deps->size; i++) {
            printf("-> ");
            source_print(source->deps->sources[i]);
        }
    }
}

//
Source* source_clone(Source* source) {

    Source* out = (Source*)malloc(sizeof(Source));
    out->name = str_init(source->name->text);
    out->file = str_init(source->file->text);
    out->header = str_init(source->header->text);
    out->object = str_init(source->object->text);
    out->objsStr = str_init(source->objsStr->text);

    out->deps = sources_clone(source->deps);

    return out;
}


//
Sources* sources_init() {

    Sources* sources = (Sources*)malloc(sizeof(Sources));
    sources->size = 0;
    sources->sources = NULL;

    return sources;
}

//
void sources_free(Sources* sources) {

    if (sources != NULL) {
        
        if (sources->sources != NULL) {
            for (size_t i = 0; i < sources->size; i++) {
                source_free(sources->sources[i]);
            }
            free(sources->sources);
            sources->sources = NULL;
        }
        
        free(sources);
        sources = NULL;
    }
}

//
void sources_print(Sources* sources) {

    for (size_t i = 0; i < sources->size; i++) {
        source_print(sources->sources[i]);
        printf("\n");
    }

}

//
void sources_add(Sources* sources, Source* src) {

    if (sources->size == 0) {
        sources->sources = (Source**)malloc(sizeof(Source*) * ++sources->size);
    } else {
        sources->sources = (Source**)realloc(sources->sources, sizeof(Source*) * ++sources->size);
    }

    sources->sources[sources->size - 1] = src;
}

//
Sources* sources_clone(Sources* sources) {

    Sources* out = (Sources*)malloc(sizeof(Sources));
    for (size_t i = 0; i < sources->size; i++) {
        sources_add(out, source_clone(sources->sources[i]));
    }

    return out;
}

//
StringList* get_deps(StringList* tokens, size_t index) {

    StringList* out = strlist_init();

    size_t i = index;
    // Add all other files until next source
    while (i < tokens->len && !str_equals_text(tokens->strings[i], "src", false)) {
        strlist_add(out, str_init(tokens->strings[i++]->text));
    }

    return out;
}

//
Sources* get_sources(StringList* tokens) {

    Sources* out = sources_init();

    for (size_t i = 0; i < tokens->len; i++) {
        
        // Get source
        if (str_equals_text(tokens->strings[i], "src", false)) {
            
            int next = i + 1;
            if (next == tokens->len) {
                printf("ERROR: handle src bounds check");
            } else {
                String* file = tokens->strings[++i];
                sources_add(out, source_init(file, NULL));
            }

        } 
        
        // Get dependencies
        if (str_equals_text(tokens->strings[i], "using", false)) {
            
            int next = i + 1;
            if (next == tokens->len) {
                printf("ERROR: handle src bounds check");
            } else {
                StringList* deps = get_deps(tokens, ++i);

                for (size_t i = 0; i < deps->len; i++) {
                    sources_add(out->sources[out->size - 1]->deps, source_init(deps->strings[i], NULL));
                }
                strlist_free(deps);
            }
        } 
    }

    // get objs strings for all sources
    for (size_t i = 0; i < out->size; i++) {
        out->sources[i]->objsStr = get_objs_str(out->sources[i]);
    }


    return out;

}


//
void write_source(FILE* fp, Source* source, bool isDep) {

    if (!isDep) {
        fprintf(fp, "%s: $(%s)\n", source->name->text, source->objsTag->text);
        fprintf(fp, "\tgcc $(CFLAGS) $(%s) -o %s\n", source->objsTag->text, source->name->text);
    } else {
        fprintf(fp, "%s: %s %s\n", source->object->text, source->file->text, source->header->text);
        fprintf(fp, "\tgcc $(CFLAGS) -c %s\n", source->file->text);
    }
    fprintf(fp, "\n");

    for (size_t i = 0; i < source->deps->size; i++) {
        write_source(fp, source->deps->sources[i], true);
    }

}

// 
void remove_dupe_deps(Sources* sources) {

    StringList* noDupes = strlist_init();

    for (size_t i = 0; i < sources->size; i++) {
        for (size_t j = 0; j < sources->sources[i]->deps->size; j++) {
            if (!strlist_contains(noDupes, sources->sources[i]->deps->sources[j]->name, true)) {
                strlist_add(noDupes, str_init(sources->sources[i]->deps->sources[j]->name->text));
            } else {
                source_free(sources->sources[i]->deps->sources[j]);
                sources->sources[i]->deps->size--;
            }
        }
    }

    strlist_free(noDupes);
}

//
void write_makefile(Sources* sources, char* makepath) {

    FILE* fp = open_file(makepath, "w");

    // preamble
    fprintf(fp, "CFLAGS=-Wall -Werror -pedantic -std=gnu99 -g\n");
    fprintf(fp, ".PHONY=all clean\n");
    fprintf(fp, ".DEFAULT_GOAL:=all\n\n");

    // obj strings
    for (size_t i = 0; i < sources->size; i++) {
        fprintf(fp, "%s\n", sources->sources[i]->objsStr->text);
    }

    fprintf(fp, "\n");

    // all
    String* all = str_init("all:");
    for (size_t i = 0; i < sources->size; i++) {
        str_concat_char(all, ' ');
        str_concat_text(all, sources->sources[i]->name->text);
    }
    fprintf(fp, "%s\n\n", all->text);
    str_free(all);

    //
    remove_dupe_deps(sources);

    // sources
    for (size_t i = 0; i < sources->size; i++) {
        write_source(fp, sources->sources[i], false);
        fprintf(fp, "\n");
    }


    // clean
    fprintf(fp, "clean:\n");
    for (size_t i = 0; i < sources->size; i++) {
        fprintf(fp, "\trm -f %s\n", sources->sources[i]->name->text);
        for (size_t j = 0; j < sources->sources[i]->deps->size; j++) {
            fprintf(fp, "\trm -f %s\n", sources->sources[i]->deps->sources[j]->object->text);
        }
    }
    
    fclose(fp);
}


//
int main(int argc, char** argv) {

    char* path = argv[1];
    char* makepath = argv[2];

    FILE* fp = open_file(path, "r");
    String* text = dynamic_read(fp, false);

    StringList* tokens = str_split(text, ", \n");
    Sources* sources = get_sources(tokens);
    
    printf("sources:\n\n");
    sources_print(sources);
    printf("------------------\n");

    write_makefile(sources, makepath);

    strlist_free(tokens);
    str_free(text);
    sources_free(sources);

    fclose(fp);

    return 0;
}