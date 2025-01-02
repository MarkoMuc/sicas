#ifndef STD_INT
#define STD_INT
#include <stdint.h>
#endif

#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "tokenizer.h"
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

uint8_t get_opcode(enum ttype operation);
