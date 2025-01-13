#ifndef STD_INT
#define STD_INT
#include <stdint.h>
#endif

#ifndef STD_STRINGS
#define STD_STRINGS
#include <strings.h>
#endif

#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "tokenizer.h"
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

// mfunc

uint8_t mnemonic_get_opcode(enum ttype operation);
uint8_t mnemonic_get_reg(char *reg);
