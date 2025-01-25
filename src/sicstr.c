#ifndef SICAS_STRING
#define SICAS_STRING
#include "../includes/sicstr.h"
#endif

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
        .str = sicstr->str,
        .count = sicstr->count,
        .size = sicstr->count + 1};
  memcpy(cstr, &temp, sizeof(temp));

  return cstr;
}

CSicstr* csicstr_create_destructive(Sicstr *sicstr) {
  CSicstr *cstr = csicstr_create(sicstr);
  sicstr_free(sicstr);

  return cstr;
}

bool csicstr_cmpr(CSicstr *cstr1, CSicstr *cstr2) {
  if(!cstr1->str || !cstr2->str ||
    !cstr1->count|| !cstr2->count) {
    LOG_PANIC("Accesing out of bounds.");
  }

  if(cstr1->count != cstr2->count) {
    return false;
  }

  const size_t size = cstr1->size;
  const char* str1 = cstr1->str;
  const char* str2 = cstr2->str;

  for(size_t i = 0; i < size; i++) {
    if(str1[i] != str2[i]) {
      return false;
    }
  }

  return true;
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
