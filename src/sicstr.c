#ifndef SICAS_STRING
#define SICAS_STRING
#include "../includes/sicstr.h"
#endif

Sicstr* sicstr_create(char *str, const size_t count, const size_t capacity) {
  if(!str || !count || !capacity) {
    LOG_PANIC("Failed to create sicstr directly, string is null.");
  }

  Sicstr* sicstr = malloc(sizeof(*sicstr));

  if(!sicstr) {
    LOG_PANIC("Failed to allocate sicstr.");
  }

  *sicstr = (Sicstr) {
    .str = str,
    .count = count,
    .capacity = capacity
  };

  return sicstr;
}

Sicstr* sicstr_full_init() {
  Sicstr *sicstr = malloc(sizeof(*sicstr));
  if(!sicstr) {
    LOG_PANIC("Failed to allocate sicstr.");
  }

  sicstr_init(sicstr);

  return sicstr;
}

bool sicstr_cmpr(const Sicstr *sicstr1, const Sicstr *sicstr2) {
  return __sicstr_cmpr(sicstr1->str, sicstr1->count, sicstr2->str, sicstr2->count);
}

char sicstr_get(const Sicstr *sicstr, const size_t idx) {
  if(idx >= sicstr->count) {
    LOG_PANIC("Accesing out of bounds.");
  }

  return sicstr->str[idx];
}

char* sicstr_dump(const Sicstr *sicstr) {
  if(!sicstr || !sicstr->str) {
    LOG_PANIC("String is not initalized.");
  }

  return sicstr->str;
}

void sicstr_build(Sicstr *sicstr, const char c) {
  if(!sicstr) {
    LOG_PANIC("Failed to add character '%c', sicstr is NULL.", c);
  }

  if(sicstr->count >= sicstr->capacity) {
    sicstr->capacity *= SICSTR_RESIZE_MULTIPLIER;
    sicstr->str = realloc(sicstr->str, sizeof(*sicstr->str) * sicstr->capacity);

    if (!sicstr->str) {
      LOG_PANIC("Error while expanding the sic string from %ld to %ld.\n",
                sicstr->count, sicstr->capacity);
    }
  }

  sicstr->str[sicstr->count] = c;

  if(c != '\0') {
    sicstr->count++;
  }
}

void sicstr_free(Sicstr *sicstr) {
  if(!sicstr) {
    LOG_PANIC("Possible double free on sicstr.");
  }

  free(sicstr);
}

void sicstr_free_destructive(Sicstr *sicstr) {
  if(!sicstr) {
    LOG_PANIC("Possible double free on sicstr.");
  }

  if(!sicstr->str) {
    LOG_PANIC("Possible double free on contents of sicstr.");
  }

  free(sicstr->str);
  free(sicstr);
}

void sicstr_init(Sicstr *sicstr) {
  if(!sicstr) {
    LOG_PANIC("Failed to initialize sicstr.");
  }

  *sicstr = (Sicstr) {
    .str = malloc(sizeof(*sicstr->str) * SICSTR_INITIAL_CAPACITY),
    .count = 0,
    .capacity = SICSTR_INITIAL_CAPACITY
  };

  if(!sicstr->str) {
    LOG_PANIC("Failed to allocate string.");
  }
}

void sicstr_merge(Sicstr *sicstr1, Sicstr *sicstr2) {
  if(!sicstr1 || !sicstr1->str || !sicstr2 || !sicstr2->str ||
    !sicstr1->count || !sicstr2->count) {
    LOG_PANIC("Failed to merge strings, one or more string is null.");
  }

  if(sicstr1->str[sicstr1->count] != '\0' || sicstr2->str[sicstr2->count] != '\0') {
    LOG_PANIC("Failed to merge, one or more strings are not finished.");
  }

  if((sicstr2->count + sicstr1->count - 1) > sicstr1->capacity) {
    sicstr1->capacity += (sicstr2->count + sicstr1->count - 1) - sicstr1->capacity;
    sicstr1->str = realloc(sicstr1->str, sizeof(*sicstr1->str) * sicstr1->capacity);
  }

  memcpy(&(sicstr1->str[sicstr1->count]), sicstr2->str, sizeof(*sicstr2->str) * sicstr2->count);

  sicstr_free_destructive(sicstr2);
}

void sicstr_replace(Sicstr *sicstr, const char c, const size_t idx) {
  if (!sicstr || idx < 0) {
    LOG_PANIC("Error while replacing character in sicstring.");
  }

  if (idx >= sicstr->count) {
    LOG_PANIC("Error while replacing character at index %ld current count is %ld.\n",
              idx, sicstr->count);
  }

  sicstr->str[idx] = c;
}

void sicstr_reset(Sicstr *sicstr) {
  if(!sicstr) {
    LOG_PANIC("Sicstr is not initalized.");
  }

  sicstr->count = 0;
}

void sicstr_rm(Sicstr *sicstr, const size_t idx) {
  if (!sicstr|| idx < 0) {
    LOG_PANIC("Error while removing a char from sicstr.");
  }

  if (idx >= sicstr->count) {
    LOG_PANIC("Error while removing a char at index %ld capacity is %ld.\n",
              idx, sicstr->count);
  }

  const size_t count = sicstr->count;
  for (size_t i = idx + 1; i < count; i++) {
    sicstr->str[i - 1] = sicstr->str[i];
  }

  sicstr->count -= 1;
}

void sicstr_rm_char(Sicstr *sicstr, const char c) {
  if (!sicstr) {
    LOG_PANIC("Error while removing a char from sicstr.");
  }

  const size_t count = sicstr->count;
  size_t disp = 0;
  for (size_t i = 0; i < count; i++) {
    if(sicstr->str[i] == c) {
      disp++;
      continue;
    }
    sicstr->str[i - disp] = sicstr->str[disp];
  }

  sicstr->count -= disp;
}

void sicstr_rm_substr(Sicstr *sicstr, const char *c) {
  LOG_PANIC("Function not yet implemented.");
}

CSicstr* csicstr_create(const Sicstr *sicstr) {
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

bool csicstr_cmpr(const CSicstr *cstr1, const CSicstr *cstr2) {
  return __sicstr_cmpr(cstr1->str, cstr1->count, cstr2->str, cstr2->count);
}

char csicstr_get(const CSicstr *cstr, const size_t idx) {
  if(idx >= cstr->count) {
    LOG_PANIC("Accesing out of bounds.");
  }

  return cstr->str[idx];
}

Slice slice(const Sicstr *sicstr) {
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
