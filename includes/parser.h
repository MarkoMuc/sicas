#ifndef TOKENIZER
#include "tokenizer.h"
#endif

enum itype { LOAD, STORE, ARIT, LOGI, JUMP, DEV, REG, MISC };
enum ftype { ONE, TWO, THREE, FOUR };

typedef struct {
  enum itype type;
  enum ftype format;
  long size;
  long opcode;
  TokenVector *vec;
} Instruction;

typedef TokenVector TokenStack;

long builder(TokenVector *tokens, TokenVector *sym, long index);
void parse_vector(TokenVector *vec, TokenVector *sym);

void tokst_push(TokenStack *s, Token *el);
Token *tokst_peek(TokenStack *s);
Token *tokst_pop(TokenStack *s);
void tokst_init(TokenStack *s);
void tokst_free(TokenStack *s);

#ifdef DEBUG_MODE
void tokst_print(TokenStack *s);
#endif
