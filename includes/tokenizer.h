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

#ifndef STD_STRINGS
#define STD_STRINGS
#include <strings.h>
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
  BASE,
  BYTE,
  WORD,
  RESB,
  RESW,
  ID,
  // -- Registers
  REGISTER,
  // -- Addressing
  AT,
  HASH,
  // -- Constants
  STRING,
  COMMA,
  LITERAL,
  HEX,
  BIN,
  NUM,
  FNUM,
  // -- Additional
  PLUS,
  MINUS
};

typedef struct {
  uint64_t s_row;
  uint64_t s_col;
  uint64_t e_row;
  uint64_t e_col;
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

#define TOKENIZER_START_BUFFER_SIZE (size_t) 256
#define TOKENIZER_START_STRING_SIZE (size_t) 128
#define TOKVEC_INITIAL_CAPACITY (size_t) 256
#define TOKVEC_RESIZE_MULTIPLIER 2
#define TOKENIZER_TAB_WIDTH (uint8_t) 4

#define token_check_null(tk) \
        do{                       \
          if ((tk) == NULL) {     \
            LOG_PANIC(("Token is NULL.\n"));}    \
        }while(0)

#define check_next_token(idx, token_vec, loc, error_msg) \
        do {\
            if((idx) >= (token_vec)->count){ \
              Token *tk =  tokvec_get((token_vec), (idx) - 1); \
              if (tk == NULL) { LOG_PANIC(("Token is NULL.\n"));}    \
              LOG_XLERR((loc), tk->location, error_msg);}\
        }while(0)

Token gen_token(char *str, Location loc);
void fill(FILE *f, TokenVector *vec);

void tokvec_init(TokenVector *v);
void tokvec_free(TokenVector *v);
void tokvec_free_destructive(TokenVector *v);
void tokvec_add(TokenVector *v, Token *el);
void tokvec_replace(TokenVector *v, Token *el, size_t idx);
void tokvec_rm(TokenVector *v, size_t idx);
Token *tokvec_get(TokenVector *v, size_t idx);

#if defined(TOKENIZER_DEBUG_MODE) || defined (DEBUG_MODE)
void tokvec_print(TokenVector *v);
void token_print(Token t);
void token_type_print(enum ttype tk_type);
#endif
