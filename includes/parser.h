#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "tokenizer.h"
#endif

#ifndef STD_INT
#define STD_INT
#include <stdint.h>
#endif

#ifndef STD_STRING
#define STD_STRING
#include <string.h>
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif


#define tokst_size tokvec_size

enum itype { LOAD, STORE, ARIT, LOGI, JUMP, DEV, REG, MISC, LABEL };
enum ftype { ZERO = 0b0, ONE = 0b1, TWO = 0b10, THREE = 0b100, FOUR = 0b1000 };

typedef struct {
  enum itype type;
  enum ftype format;
  int opcode;
  TokenVector *vec;
} Instruction;

typedef TokenVector TokenStack;

uint8_t parse_regs(TokenVector *tokens, TokenVector *new, long *idx);
uint8_t parse_mem_addr(TokenVector *tokens, TokenVector *new, long *idx,
                       uint8_t float_type);
long builder(TokenVector *tokens, TokenVector *sym, TokenStack *stack,
             long idx);
void parse_vector(TokenVector *vec, TokenVector *sym);

void tokst_push(TokenStack *s, Token *el);
Token *tokst_peek(TokenStack *s);
Token *tokst_pop(TokenStack *s);
void tokst_init(TokenStack *s);
void tokst_free(TokenStack *s);

#ifdef DEBUG_MODE
void tokst_print(TokenStack *s);
#endif
