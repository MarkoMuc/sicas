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

Token gen_token(char *str, Location loc);
void fill(FILE *f, Vector *vec);

void add_el(Vector *v, Token *el);
void init(Vector *v);
void free_vec(Vector *v);

#ifdef DEBUG_MODE
void printv(Vector *v) {
  for (int i = 0; i < v->count; i++) {
    Token t = v->items[i];
    switch (t.type) {
    case NUM:
      printf("NUM: %s [%d:%d] [%d:%d]\n", t.str, t.location.s_row,
             t.location.s_col, t.location.e_row, t.location.e_col);
      break;
    case ID:
      printf("ID: %s [%d:%d] [%d:%d]\n", t.str, t.location.s_row,
             t.location.s_col, t.location.e_row, t.location.e_col);
      break;
    case ADD:
      printf("ADD [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case ADDF:
      printf("ADDF [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case ADDR:
      printf("ADDR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case AND:
      printf("AND [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case CLEAR:
      printf("CLEAR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case COMP:
      printf("COMP [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case COMPF:
      printf("COMPF [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case COMPR:
      printf("COMPR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case DIV:
      printf("DIV [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case DIVF:
      printf("DIVF [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case DIVR:
      printf("DIVR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case FIX:
      printf("FIX [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case FLOAT:
      printf("FLOAT [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case HIO:
      printf("HIO [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case J:
      printf("J [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case JEQ:
      printf("JEQ [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case JGT:
      printf("JGT [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case JLT:
      printf("JLT [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case JSUB:
      printf("JSUB [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LDA:
      printf("LDA [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LDB:
      printf("LDB [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LDCH:
      printf("LDCH [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LDF:
      printf("LDF [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LDL:
      printf("LDL [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LDS:
      printf("LDS [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LDT:
      printf("LDT [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LDX:
      printf("LDX [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case LPS:
      printf("LPS [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case MUL:
      printf("MUL [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case MULF:
      printf("MULF [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case MULR:
      printf("MULR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case NORM:
      printf("NORM [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case OR:
      printf("OR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case RD:
      printf("RD [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case RMO:
      printf("RMO [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case RSUB:
      printf("RSUB [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case SHIFTL:
      printf("SHIFTL [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case SHIFTR:
      printf("SHIFTR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case SIO:
      printf("SIO [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case SSK:
      printf("SSK [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STA:
      printf("STA [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STB:
      printf("STB [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STCH:
      printf("STCH [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STF:
      printf("STF [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STI:
      printf("STI [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STL:
      printf("STL [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STS:
      printf("STS [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STSW:
      printf("STSW [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STT:
      printf("STT [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case STX:
      printf("STX [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case SUB:
      printf("SUB [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case SUBF:
      printf("SUBF [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case SUBR:
      printf("SUBR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case SVC:
      printf("SVC [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case TD:
      printf("TD [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case TIO:
      printf("TIO [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case TIX:
      printf("TIX [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case TIXR:
      printf("TIXR [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case WD:
      printf("WD [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case START:
      printf("START [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case END:
      printf("END [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case BYTE:
      printf("BYTE [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case WORD:
      printf("WORD [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case RESB:
      printf("RESB [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case RESW:
      printf("RESW [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case COMMA:
      printf("COMMA [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    default:
      break;
    }
  }
}
#endif
