#include <stdint.h>
#ifndef TOKENIZER
#include "tokenizer.h"
#endif

void fill(FILE *f, Vector *vec) {
  int read_c;
  char *buffer;
  char *str;
  long row = 0;
  long col = 0;
  int index = 0;
  uint8_t num = 0;
  uint8_t identf = 0;
  uint8_t comment = 0;
  uint8_t special = 0;
  long s_row = -1;
  long s_col = -1;

  buffer = malloc(sizeof((*buffer)) * 256);
  if (buffer == NULL) {
    printf("Error during buffer malloc.");
    exit(1);
  }

  str = malloc(sizeof((*str)) * 128);
  if (str == NULL) {
    printf("Error during str malloc.");
    exit(1);
  }

  while ((read_c = fread(buffer, sizeof(char), 256, f)) > 1) {
    for (int i = 0; i < read_c; i++) {
      char c = buffer[i];
      col++;
      index++;

      if (c >= 'A' && c <= 'Z') {
        c = c + 32;
      }

      if (special == 0 && c == 'x') {
        special = 1;
      } else if (special == 0 && c == 'c') {
        special = 3;
      } else if (special && index == 1 && c == '\'') {
        special++;
        str[index - 1] = '\0';
        index = 0;
        identf = 0;
        continue;
      }

      if (special == 2 || special == 4) {
        if (c == '\'') {
          str[index - 1] = '\0';
          printf("HERE: %s [%ld, %ld]\n", str, row, col);
          Location loc = {
              .s_col = s_col, .s_row = s_row, .e_col = col, .e_row = row};
          Token el = {
              .str = str, .type = special == 4 ? STRING : HEX, .location = loc};
          add_el(vec, &el);

          if (el.str != NULL) {
            str = malloc(sizeof((*str)) * 128);
            if (str == NULL) {
              printf("Error allocating str after HEX or STRING.\n");
              exit(1);
            }
          }

          s_row = -1;
          index = 0;
          special = 0;
          continue;
        } else {
          if (special == 2 && !(c >= '0' && c <= '9') &&
              !(c >= 'a' && c <= 'f')) {
            printf("Not a valid hex. [%ld, %ld] [%ld, %ld] = %c\n", s_row,
                   s_col, row, col, buffer[i]);
            exit(1);
          }
          str[index - 1] = buffer[i];
          continue;
        }
      }

      if (c == '.') {
        comment = 1;
      } else if (comment && c != '\n') {
        continue;
      }

      if (!comment && !num && !identf && ((c >= 'a' && c <= 'z') || c == '_')) {
        s_row = row;
        s_col = col;
        index = 0;
        identf = 1;
      }

      if (identf) {
        if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'z') && c != '_') {
          str[index] = '\0';
          Location loc = {
              .s_col = s_col, .s_row = s_row, .e_col = col, .e_row = row};
          Token el = gen_token(str, loc);
          add_el(vec, &el);

          if (el.str != NULL) {
            str = malloc(sizeof((*str)) * 128);
            if (str == NULL) {
              printf("Error allocating str after num token");
              exit(1);
            }
          }

          s_row = -1;
          index = 0;
          identf = 0;
          special = 0;
        } else {
          str[index] = c;
          continue;
        }
      }

      if (!comment && !num && !identf && (c >= '0' && c <= '9')) {
        s_row = row;
        s_col = col;
        index = 0;
        num = 1;
      }

      if (num) {
        if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') &&
            !(str[0] == '0' && c == 'x' && index == 1)) {
          str[index] = '\0';
          Token el = {
              .type = NUM,
              .str = str,
              .location = {
                  .s_col = s_col, .s_row = s_row, .e_col = col, .e_row = row}};
          add_el(vec, &el);

          if (str != NULL) {
            str = malloc(sizeof((*str)) * 128);
            if (str == NULL) {
              printf("Error allocating str after num token");
              exit(1);
            }
          }

          s_row = -1;
          index = 0;
          num = 0;
        } else {
          str[index] = c;
          continue;
        }
      }

      switch (c) {
      case ' ':
        break;
      case '\t':
        break;
      case ',': {
        Token el = {
            .type = COMMA,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        add_el(vec, &el);
        break;
      }
      case '#': {
        Token el = {
            .type = LITERAL,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        add_el(vec, &el);
        break;
      }
      case '\n': {
        comment = 0;
        row += 1;
        col = 0;
        break;
      }
      case '+': {
        Token el = {
            .type = PLUS,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        add_el(vec, &el);
        break;
      }
      case '-': {
        Token el = {
            .type = MINUS,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        add_el(vec, &el);
        break;
      }
      case '@': {
        Token el = {
            .type = AT,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        add_el(vec, &el);
        break;
      }
      default:
        if (!comment) {
          printf("[%ld:%ld] Illegal symbol %c\n", row, col, c);
          exit(1);
        }
        break;
      }
    }
  }

  free(str);
  free(buffer);
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

Token gen_token(char *str, Location loc) {
  Token token = {};
  uint8_t not_keyword = 0;
  enum ttype typ;
  if (!strncmp(str, "a", 1)) {
    if (!strcmp(str, "add")) {
      typ = ADD;
    } else if (!strcmp(str, "addf")) {
      typ = ADDF;
    } else if (!strcmp(str, "addr")) {
      typ = ADDR;
    } else if (!strcmp(str, "and")) {
      typ = AND;
    } else {
      not_keyword = 1;
    }
  } else if (!strncmp(str, "c", 1)) {
    if (!strcmp(str, "clear")) {
      typ = CLEAR;
    } else if (!strcmp(str, "comp")) {
      typ = COMP;
    } else if (!strcmp(str, "compf")) {
      typ = COMPF;
    } else if (!strcmp(str, "compr")) {
      typ = COMPR;
    } else {
      not_keyword = 1;
    }
  } else if (!strncmp(str, "d", 1)) {
    if (!strcmp(str, "div")) {
      typ = DIV;
    } else if (!strcmp(str, "divf")) {
      typ = DIVF;
    } else if (!strcmp(str, "divr")) {
      typ = DIVR;
    } else {
      not_keyword = 1;
    }
  } else if (!strncmp(str, "f", 1)) {
    if (!strcmp(str, "fix")) {
      typ = FIX;
    } else if (!strcmp(str, "float")) {
      typ = FLOAT;
    } else {
      not_keyword = 1;
    }
  } else if (!strcmp(str, "hio")) {
    typ = HIO;
  } else if (!strncmp(str, "j", 1)) {
    if (!strcmp(str, "j")) {
      typ = J;
    } else if (!strcmp(str, "jeq")) {
      typ = JEQ;
    } else if (!strcmp(str, "jgt")) {
      typ = JGT;
    } else if (!strcmp(str, "jlt")) {
      typ = JLT;
    } else if (!strcmp(str, "jsub")) {
      typ = JSUB;
    } else {
      not_keyword = 1;
    }
  } else if (!strncmp(str, "l", 1)) {
    if (!strcmp(str, "lda")) {
      typ = LDA;
    } else if (!strcmp(str, "ldb")) {
      typ = LDB;
    } else if (!strcmp(str, "ldch")) {
      typ = LDCH;
    } else if (!strcmp(str, "ldf")) {
      typ = LDF;
    } else if (!strcmp(str, "ldl")) {
      typ = LDL;
    } else if (!strcmp(str, "lds")) {
      typ = LDS;
    } else if (!strcmp(str, "ldt")) {
      typ = LDT;
    } else if (!strcmp(str, "ldx")) {
      typ = LDX;
    } else if (!strcmp(str, "lps")) {
      typ = LPS;
    } else {
      not_keyword = 1;
    }
  } else if (!strncmp(str, "m", 1)) {
    if (!strcmp(str, "mul")) {
      typ = MUL;
    } else if (!strcmp(str, "mulf")) {
      typ = MULF;
    } else if (!strcmp(str, "mulr")) {
      typ = MULR;
    } else {
      not_keyword = 1;
    }
  } else if (!strcmp(str, "norm")) {
    typ = NORM;
  } else if (!strcmp(str, "or")) {
    typ = OR;
  } else if (!strncmp(str, "r", 1)) {
    if (!strcmp(str, "rd")) {
      typ = RD;
    } else if (!strcmp(str, "rmo")) {
      typ = RMO;
    } else if (!strcmp(str, "rsub")) {
      typ = RSUB;
    } else if (!strcmp(str, "resb")) {
      typ = RESB;
    } else if (!strcmp(str, "resw")) {
      typ = RESW;
    } else {
      not_keyword = 1;
    }
  } else if (!strncmp(str, "s", 1)) {
    if (!strcmp(str, "shiftl")) {
      typ = SHIFTL;
    } else if (!strcmp(str, "shiftr")) {
      typ = SHIFTR;
    } else if (!strcmp(str, "sio")) {
      typ = SIO;
    } else if (!strcmp(str, "ssk")) {
      typ = SSK;
    } else if (!strcmp(str, "sta")) {
      typ = STA;
    } else if (!strcmp(str, "stb")) {
      typ = STB;
    } else if (!strcmp(str, "stch")) {
      typ = STCH;
    } else if (!strcmp(str, "stf")) {
      typ = STF;
    } else if (!strcmp(str, "sti")) {
      typ = STI;
    } else if (!strcmp(str, "stl")) {
      typ = STL;
    } else if (!strcmp(str, "sts")) {
      typ = STS;
    } else if (!strcmp(str, "stsw")) {
      typ = STSW;
    } else if (!strcmp(str, "stt")) {
      typ = STT;
    } else if (!strcmp(str, "stx")) {
      typ = STX;
    } else if (!strcmp(str, "sub")) {
      typ = SUB;
    } else if (!strcmp(str, "subf")) {
      typ = SUBF;
    } else if (!strcmp(str, "subr")) {
      typ = SUBR;
    } else if (!strcmp(str, "svc")) {
      typ = SVC;
    } else if (!strcmp(str, "start")) {
      typ = START;
    } else {
      not_keyword = 1;
    }
  } else if (!strncmp(str, "t", 1)) {
    if (!strcmp(str, "td")) {
      typ = TD;
    } else if (!strcmp(str, "tio")) {
      typ = TIO;
    } else if (!strcmp(str, "tix")) {
      typ = TIX;
    } else if (!strcmp(str, "tixr")) {
      typ = TIXR;
    } else {
      not_keyword = 1;
    }
  } else if (!strcmp(str, "wd")) {
    typ = WD;
  } else if (!strcmp(str, "end")) {
    typ = END;
  } else if (!strcmp(str, "byte")) {
    typ = BYTE;
  } else if (!strcmp(str, "word")) {
    typ = WORD;
  } else {
    not_keyword = 1;
  }

  if (not_keyword) {
    token.str = str;
    typ = ID;
  }

  token.type = typ;
  token.location = loc;

  return token;
}

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
    case LITERAL:
      printf("LITERAL [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case PLUS:
      printf("PLUS [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case MINUS:
      printf("MINUS [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    case HEX:
      printf("HEX: %s [%d:%d] [%d:%d]\n", t.str, t.location.s_row,
             t.location.s_col, t.location.e_row, t.location.e_col);
      break;
    case STRING:
      printf("STRING: %s [%d:%d] [%d:%d]\n", t.str, t.location.s_row,
             t.location.s_col, t.location.e_row, t.location.e_col);
      break;
    case AT:
      printf("AT [%d:%d] [%d:%d]\n", t.location.s_row, t.location.s_col,
             t.location.e_row, t.location.e_col);
      break;
    default:
      break;
    }
  }
}
#endif
