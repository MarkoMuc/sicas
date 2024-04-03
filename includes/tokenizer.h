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

enum ttype {
  // -- CPU Instructions
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
  // -- ASSEMBLER DIRECTIVES
  START,
  END,
  BYTE,
  WORD,
  RESB,
  RESW,
  // -- ADDITIONAL
  AT,
  STRING,
  HEX,
  COMMA,
  LITERAL,
  PLUS,
  MINUS,
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

Token gen_token(char *str, Location loc);
void fill(FILE *f, Vector *vec);

void add_el(Vector *v, Token *el);
void init(Vector *v);
void free_vec(Vector *v);

#ifdef DEBUG_MODE
void printv(Vector *v);
#endif
