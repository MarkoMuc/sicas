#ifndef STD_IO
#include <stdio.h>
#endif

#ifndef STD_INT
#include <stdint.h>
#endif

#ifndef STD_LIB
#include <stdlib.h>
#endif

#ifndef STD_STRING
#include <string.h>
#endif

// TODO: add additional assembler keywards like .equ
enum ttype {
  ADD,
  ADDF,
  ADDR,
  AND,
  CLEAR,
  COMP,
  COMPF,
  COMPR,
  DIV,
  DIVF,
  DIVR,
  FIX,
  FLOAT,
  HIO,
  J,
  JEQ,
  JGT,
  JLT,
  JSUB,
  LDA,
  LDB,
  LDCH,
  LDF,
  LDL,
  LDS,
  LDT,
  LDX,
  LPS,
  MUL,
  MULF,
  MULR,
  NORM,
  OR,
  RD,
  RMO,
  RSUB,
  SHIFTL,
  SHIFTR,
  SIO,
  SSK,
  STA,
  STB,
  STCH,
  STF,
  STI,
  STL,
  STS,
  STSW,
  STT,
  STX,
  SUB,
  SUBF,
  SUBR,
  SVC,
  TD,
  TIO,
  TIX,
  TIXR,
  WD,
  COMMA,
  ID,
  NUM
};

typedef struct {
  uint32_t s_row;
  uint32_t s_col;
  uint32_t e_row;
  uint32_t e_col;
} Location;

typedef struct {
  enum ttype type;
  char *str;
  Location location;
} Token;

typedef struct {
  size_t count;
  size_t capacity;
  Token *items;
} Vector;

void add_el(Vector *v, Token *el) {
  if (v == NULL || el == NULL) {
    printf("Error adding element.");
    exit(1);
  }

  if (v->count == v->capacity) {
    v->capacity *= 2;
    v->items = realloc(v->items, sizeof(*v->items) * v->capacity);
    if (v->items == NULL) {
      printf("Error expanding vector.");
      exit(1);
    }
  }
  v->items[v->count] = *el;
  v->count++;
}

void init(Vector *v) {
  v->count = 0;
  v->capacity = 256;
  v->items = malloc(sizeof(*v->items) * v->capacity);

  if (v->items == NULL) {
    printf("Error during vector init.");
    exit(1);
  }
}

void free_vec(Vector *v) {
  if (v == NULL) {
    printf("Error during vector free.");
    exit(1);
  }
  for (int i = 0; i < v->count; i++) {
    Token token = v->items[i];
    if (token.str != NULL) {
      free(token.str);
    }
  }
  free(v->items);
}

#ifdef DEBUG_MODE
void printv(Vector *v) {
  for (int i = 0; i < v->count; i++) {
    Token t = v->items[i];
    switch (t.type) {
    case NUM:
      printf("NUM: %s [%d:%d]\n", t.str, t.location.s_row, t.location.s_col);
      break;
    case ID:
      printf("ID: %s [%d:%d]\n", t.str, t.location.s_row, t.location.s_col);
      break;
    case ADD:
      printf("ADD [%d:%d]\n", t.location.s_row, t.location.s_col);
      break;
    case MUL:
      printf("MUL [%d:%d]\n", t.location.s_row, t.location.s_col);
      break;
    case LDA:
      printf("LDA [%d:%d]\n", t.location.s_row, t.location.s_col);
      break;
    case STA:
      printf("STA [%d:%d]\n", t.location.s_row, t.location.s_col);
      break;
    case COMMA:
      printf("COMMA [%d:%d]\n", t.location.s_row, t.location.s_col);
      break;
    default:
      break;
    }
  }
}
#endif
