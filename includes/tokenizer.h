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

#ifndef LOGGER
#include "logger.h"
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
  // -- Registers
  REGISTER,
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
} TokenVector;

Token gen_token(char *str, Location loc);
void fill(FILE *f, TokenVector *vec);

void add_el(TokenVector *v, Token *el);
void add_at(TokenVector *v, Token *el, size_t index);
Token *get(TokenVector *v, size_t index);
void rm_at(TokenVector *v, size_t index);
size_t vec_size(TokenVector *v);
void init(TokenVector *v);
void free_vec(TokenVector *v);

#ifdef DEBUG_MODE
void printv(TokenVector *v);
void printt(Token t);
#endif
