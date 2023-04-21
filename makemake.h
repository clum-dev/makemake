#ifndef __MAKEMAKE_H__
#define __MAKEMAKE_H__

#include "clum-lib/strings.h"

//
struct Source {

    String* name;       // src

    String* file;       // src.c
    String* header;     // src.h
    String* object;     // src.o

    String* objsTag;    // OBJS_src
    String* objsStr;    // src.c dep1.o dep2.o

    struct Sources* deps;      // dependencies
    
};

struct Source;
typedef struct Source Source;

//
struct Sources {

    Source** sources;
    size_t size;

};

struct Sources;
typedef struct Sources Sources;


Source* source_init(String* file, StringList* deps);
void source_free(Source* source);
void source_print(Source* source);
Source* source_clone(Source* source);

Sources* sources_init();
void sources_free(Sources* sources);
void sources_print(Sources* sources);
void sources_add(Sources* sources, Source* src);
Sources* sources_clone(Sources* sources);



#endif