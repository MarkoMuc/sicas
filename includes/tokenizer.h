#ifndef STD_IO
#define STD_IO
#include <stdio.h>
#endif

#ifndef STD_INT
#define STD_INT
#include <stdint.h>
#endif

#ifndef STD_LIB
#define STD_LIB
#include <stdlib.h>
#endif

#ifndef STD_STRING
#define STD_STRING
#include <string.h>
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
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
  AT,//TODO
  STRING,//TODO
  COMMA,
  LITERAL,
  PLUS,
  MINUS,
  ID,
  HEX,
  BIN,
  NUM,
  FNUM
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

#define token_check_null(tk, err) \
        do{\
          if ((tk) == NULL) {\
            LOG_ERR((err));\
            exit(1);}\
        }while(0)

Token gen_token(char *str, Location loc);
void fill(FILE *f, TokenVector *vec);

void tokvec_add(TokenVector *v, Token *el);
void tokvec_add_at(TokenVector *v, Token *el, size_t idx);
Token *tokvec_get(TokenVector *v, size_t idx);
void tokvec_rm_at(TokenVector *v, size_t idx);
size_t tokvec_size(TokenVector *v);
void tokvec_init(TokenVector *v);
void tokvec_free(TokenVector *v);

#ifdef DEBUG_MODE
void tokvec_print(TokenVector *v);
void token_print(Token t);
#endif
