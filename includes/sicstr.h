#ifndef STD_LIB
#define STD_LIB
#include <stdlib.h>
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

// structs

typedef struct {
  char* start;
  char* end;
} Slice;

typedef struct {
  const char* str;
  const size_t count;
  //FIXME: In theory capacity should be equal to count
  const size_t capacity;
} CSicstr;

typedef struct {
  char* str;
  size_t count;
  size_t capacity;
} Sicstr;

void sicstr_build(Sicstr *sicstr, char c);
void sicstr_cmpr(Sicstr *sicstr1, Sicstr *sicstr2);
void sicstr_create(Sicstr *sicstr, char *str);
void sicstr_free(Sicstr *sicstr);
void sicstr_free_destructive(Sicstr *sicstr);
void sicstr_get(Sicstr *sicstr, size_t idx);
void sicstr_init(Sicstr *sicstr);
void sicstr_replace(Sicstr *sicstr, char c, size_t idx);
void sicstr_rm(Sicstr *sicstr, size_t idx);
void sicstr_rm_char(Sicstr *sicstr, char c);
void sicstr_rm_substr(Sicstr *sicstr, char c);
void sicstr_merge(Sicstr *sicstr1, Sicstr *sicstr2);

void csicstr_cmpr(CSicstr *cstr1, CSicstr *cstr2);
void csicstr_create(CSicstr *cstr, Sicstr *sicstr);
void csicstr_get(CSicstr *cstr, size_t idx);

Slice slice(Sicstr *sicstr);
