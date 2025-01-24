#ifndef SICAS_STRING
#define SICAS_STRING
#include "../includes/sicstr.h"
#endif

Slice slice(Sicstr *sicstr) {
  if(!sicstr->str || sicstr->count == 0) {
    LOG_PANIC("Creating a slice out of an empty string.");
  }
  return (Slice){.start = sicstr->str, .end = sicstr->str + sicstr->count};
}
