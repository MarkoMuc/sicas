#ifndef TOKENIZER
#include <tokenizer.h>
#endif

typedef struct {
  size_t count;
  size_t capacity;
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
