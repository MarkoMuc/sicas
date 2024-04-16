#ifndef TOKENIZER
#include <tokenizer.h>
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

typedef struct {
  TokenVector *vec;
} TokenStack;

long builder(TokenVector *tokens, TokenVector *instr, TokenVector *sym,
             long index);
void parse_vector(TokenVector *vec, TokenVector *instr, TokenVector *sym);

void push(TokenStack *s, Token *el);
Token *peek(TokenStack *s);
Token *pop(TokenStack *s);
void stack_init(TokenStack *s);
void free_stack(TokenStack *s);

#ifdef DEBUG_MODE
void prints(TokenStack *s);
#endif
