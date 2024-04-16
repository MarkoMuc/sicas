#ifndef PARSER
#include <parser.h>
#endif

long builder(TokenVector *tokens, TokenVector *instr, TokenVector *sym,
             long index) {

  return index;
}

void parse_vector(TokenVector *vec, TokenVector *instr, TokenVector *sym) {
  long vec_size = vec->count;
  for (long i = 0; i < vec_size; i++) {
    i = builder(vec, instr, sym, i);
  }
}

void push(TokenStack *s, Token *el) { add_el(s->vec, el); }

Token *peek(TokenStack *s) {
  size_t index = vec_size(s->vec);
  Token *t = get(s->vec, index);
  return t;
}

Token *pop(TokenStack *s) {
  size_t index = vec_size(s->vec);
  Token *t = get(s->vec, index);
  rm_at(s->vec, index);
  return t;
}

void stack_init(TokenStack *s) { init(s->vec); }

void free_stack(TokenStack *s) {
  free_vec(s->vec);
  free(s);
}

#ifdef DEBUG_MODE
void prints(TokenStack *s) { printv(s->vec); }
#endif
