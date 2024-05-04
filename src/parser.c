#ifndef PARSER
#include "../includes/parser.h"
#endif

long builder(TokenVector *tokens, TokenVector *sym, long index) {
  Instruction i = {};
  tokvec_init(i.vec);
  switch (tokvec_get(tokens, index)->type) {
  default:
    LOG_ERR("Error token type not valid at index %ld\n", index);
  }
  return index;
}

void parse_vector(TokenVector *vec, TokenVector *sym) {
  long vec_size = vec->count;
  for (long i = 0; i < vec_size; i++) {
    i = builder(vec, sym, i);
  }
}

void tokst_push(TokenStack *s, Token *el) { tokvec_add(s, el); }

Token *tokst_peek(TokenStack *s) {
  size_t index = tokvec_size(s);
  Token *t = tokvec_get(s, index);
  return t;
}

Token *tokst_pop(TokenStack *s) {
  size_t index = tokvec_size(s);
  Token *t = tokvec_get(s, index);
  tokvec_rm_at(s, index);
  return t;
}

void tokst_init(TokenStack *s) { tokvec_init(s); }

void tokst_free(TokenStack *s) { tokvec_free(s); }

#ifdef DEBUG_MODE
void tokst_print(TokenStack *s) { tokvec_print(s); }
#endif
