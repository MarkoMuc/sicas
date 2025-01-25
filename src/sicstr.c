#ifndef SICAS_STRING
#define SICAS_STRING
#include "../includes/sicstr.h"
#endif

bool sicstr_cmpr(Sicstr *sicstr1, Sicstr *sicstr2) {
  return __sicstr_cmpr(sicstr1->str, sicstr1->count, sicstr2->str, sicstr2->count);
}

void sicstr_free(Sicstr *sicstr) {
  if(!sicstr) {
    LOG_PANIC("Possible double free on sicstr.");
  }

  free(sicstr);
}

CSicstr* csicstr_create(Sicstr *sicstr) {
  if(!sicstr->str || !sicstr->count) {
    LOG_PANIC("Accesing out of bounds.");
  }

  CSicstr *cstr = malloc(sizeof(*cstr));
  if(!cstr) {
    LOG_PANIC("Failed to malloc csicstr.");
  }

  CSicstr temp = (CSicstr) {
        .str = sicstr->str, .count = sicstr->count};
  memcpy(cstr, &temp, sizeof(temp));

  return cstr;
}

CSicstr* csicstr_create_destructive(Sicstr *sicstr) {
  CSicstr *cstr = csicstr_create(sicstr);
  sicstr_free(sicstr);

  return cstr;
}

bool csicstr_cmpr(CSicstr *cstr1, CSicstr *cstr2) {
  return __sicstr_cmpr(cstr1->str, cstr1->count, cstr2->str, cstr2->count);
}

char csicstr_get(CSicstr *cstr, size_t idx) {
  if(idx >= cstr->count) {
    LOG_PANIC("Accesing out of bounds.");
  }

  return cstr->str[idx];
}

Slice slice(Sicstr *sicstr) {
  if(!sicstr->str || !sicstr->count) {
    LOG_PANIC("Creating a slice out of an empty string.");
  }
  return (Slice){.start = sicstr->str, .end = sicstr->str + sicstr->count};
}

bool __sicstr_cmpr(const char *str1, const size_t count1, const char *str2, const size_t count2) {
  if(!str1 || !str2 || !count1 || !count2) {
    LOG_PANIC("Failed during string comparison, one or more strings is empty.");
  }

  if(count1 != count2) {
    return false;
  }

  for(size_t i = 0; i < count1; i++) {
    if(str1[i] != str2[i]) {
      return false;
    }
  }

  return true;
}
