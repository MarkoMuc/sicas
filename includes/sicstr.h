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
#define sicstr_lst(sicstr) sicstr_get((sicstr), (sicstr)->count - 1)
#define sicstr_fst(sicstr) sicstr_get((sicstr), 0)

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

Sicstr* sicstr_create(char *str, const size_t count, const size_t capacity);
Sicstr* sicstr_full_init();
bool sicstr_cmpr(const Sicstr *sicstr1, const Sicstr *sicstr2);
char sicstr_get(const Sicstr *sicstr, const size_t idx);
char* sicstr_dump(const Sicstr *sicstr);
void sicstr_build(Sicstr *sicstr, const char c);
void sicstr_build_str(Sicstr *sicstr, const char* c);
void sicstr_free(Sicstr *sicstr);
void sicstr_free_destructive(Sicstr *sicstr);
void sicstr_free_destructive_stack(Sicstr *sicstr);
void sicstr_init(Sicstr *sicstr);
void sicstr_merge(Sicstr *sicstr1, Sicstr *sicstr2);
void sicstr_replace(Sicstr *sicstr, const char c, const size_t idx);
void sicstr_reset(Sicstr *sicstr);
void sicstr_rm(Sicstr *sicstr, const size_t idx);
void sicstr_rm_char(Sicstr *sicstr, const char c);
void sicstr_rm_substr(Sicstr *sicstr, const char *c);

CSicstr* csicstr_create(const Sicstr *sicstr);
CSicstr* csicstr_create_destructive(Sicstr *sicstr);
bool csicstr_cmpr(const CSicstr *cstr1, const CSicstr *cstr2);
char csicstr_get(const CSicstr *cstr, const size_t idx);

Slice slice(const Sicstr *sicstr);

bool __sicstr_cmpr(const char *str1, const size_t count1, const char *str2, const size_t count2);
