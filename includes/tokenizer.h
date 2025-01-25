#ifndef STD_IO
#define STD_IO
#include <stdio.h>
#endif

#ifndef STD_INT
#define STD_INT
#include <stdint.h>
#endif

#ifndef STD_BOOL
#define STD_BOOL
#include <stdbool.h>
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

// macros

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

#define is_alphabetic(c) ((c) >= 'a' && (c) <= 'z')
#define is_numeric(c) ((c) >= '0' && (c) <= '9')
#define is_alphanumeric(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= '0' && (c) <= '9'))
#define is_specialchar(c) ((c) == '0' || (c) == '\\' || (c) == 'n' || (c) == 'r' || (c) == 't' || (c) == 'v')

// enums

enum special_token {
  NON_T = 0,
  HEXP_T = 1,
  HEX_T = 2,
  CHARP_T = 3,
  CHAR_T = 4,
  FLOATP_T = 5,
  FLOAT_T = 6,
  BINP_T = 7,
  BIN_T = 8
};

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

//structs

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

// mfunc

Token gen_token(char *str, const Location loc);
bool fill(FILE *f, TokenVector *vec);

// ufunc

void tokvec_init(TokenVector *v);
void tokvec_free(TokenVector *v);
void tokvec_free_destructive(TokenVector *v);
void tokvec_add(TokenVector *v, const Token *el);
void tokvec_add_at(TokenVector *v, const Token *el, const size_t idx);
void tokvec_rm(TokenVector *v, const size_t idx);
Token *tokvec_get(const TokenVector *v, const size_t idx);

// debug

void token_print(Token t);
void token_type_print(enum ttype tk_type);
#if defined(TOKENIZER_DEBUG_MODE) || defined(PARSER_DEBUG_MODE) || defined (DEBUG_MODE)
void tokvec_print(TokenVector *v);
#endif
