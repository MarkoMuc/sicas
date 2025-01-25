#ifndef STD_LIB
#define STD_LIB
#include <stdlib.h>
#endif

#ifndef STD_BOOL
#define STD_BOOL
#include <stdbool.h>
#endif

#ifndef STD_STRING
#define STD_STRING
#include <string.h>
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

// macros

#define SICSTR_RESIZE_MULTIPLIER 2
#define SICSTR_INITIAL_CAPACITY (size_t) 256

#define sicstr_fin(sicstr) sicstr_build((sicstr), '\0')

// structs

typedef struct {
  char* start;
  char* end;
} Slice;

typedef struct {
  const char* str;
  const size_t count;
} CSicstr;

typedef struct {
  char* str;
  size_t count;
  size_t capacity;
} Sicstr;

// ufuncs

Sicstr* sicstr_create(char *str, size_t count, size_t capacity);
bool sicstr_cmpr(Sicstr *sicstr1, Sicstr *sicstr2);
char sicstr_get(Sicstr *sicstr, size_t idx);
void sicstr_build(Sicstr *sicstr, char c);
void sicstr_free(Sicstr *sicstr);
void sicstr_free_destructive(Sicstr *sicstr);
void sicstr_init(Sicstr *sicstr);
void sicstr_merge(Sicstr *sicstr1, Sicstr *sicstr2);
void sicstr_replace(Sicstr *sicstr, char c, size_t idx);
void sicstr_rm(Sicstr *sicstr, size_t idx);
void sicstr_rm_char(Sicstr *sicstr, char c);
void sicstr_rm_substr(Sicstr *sicstr, char c);

CSicstr* csicstr_create(Sicstr *sicstr);
CSicstr* csicstr_create_destructive(Sicstr *sicstr);
bool csicstr_cmpr(CSicstr *cstr1, CSicstr *cstr2);
char csicstr_get(CSicstr *cstr, size_t idx);

Slice slice(Sicstr *sicstr);

bool __sicstr_cmpr(const char *str1, const size_t count1, const char *str2, const size_t count2);
