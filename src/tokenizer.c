#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "../includes/tokenizer.h"
#endif

void fill(FILE *f, TokenVector *vec) {
  size_t read_c;
  char *buffer;
  char *str;
  uint64_t row = 0;
  uint64_t col = 0;
  size_t idx = 0;
  uint8_t num = 0;
  uint8_t identf = 0;
  uint8_t comment = 0;
  uint8_t special = 0;
  int32_t num_delimiter = 0;
  int8_t fraction = -1;
  uint64_t s_row = -1;
  uint64_t s_col = -1;

  buffer = malloc(sizeof((*buffer)) * START_BUFFER_SIZE);
  if (!buffer) {
    LOG_PANIC("Error during inital buffer alloc for token.\n");
  }

  str = malloc(sizeof((*str)) * START_STRING_SIZE);
  if (!str) {
    LOG_PANIC("Error during inital string alloc for token.\n");
  }

  while ((read_c = fread(buffer, sizeof(char), START_BUFFER_SIZE, f)) > 1) {
    for (size_t i = 0; i < read_c; i++) {
      char c = buffer[i];
      col++;
      idx++;

      if (c >= 'A' && c <= 'Z') {
        c = c + 32;
      }

      if (special == 0 && c == 'x') {
        special = 1;
      } else if (special == 0 && c == 'c') {
        special = 3;
      } else if (special == 0 && c == 'f' && !num) {
        special = 5;
      } else if (special == 0 && c == 'b' && !num) {
        special = 7;
      } else if (special && idx == 1 && c == '\'') {
        special++;
        idx = -1;
        identf = 0;
        continue;
      }

      if (special % 2 == 0 && special) {
        if (c == '\'') {
          str[idx] = '\0';
          Location loc = {
              .s_col = s_col, .s_row = s_row, .e_col = col, .e_row = row};
          enum ttype type = HEX;

          if (special > 2) {
            type = special == 4 ? STRING : FNUM;
            type = special == 8 ? BIN : type;
          }

          if (fraction == 0) {
            LOG_PANIC("[%ld, %ld]:[%ld, %ld] %s float missing fractional part.\n",
                    s_row, s_col, row, col, str);
          }

          Token el = {.str = str, .type = type, .location = loc};
          tokvec_add(vec, &el);

          if (el.str) {
            str = malloc(sizeof((*str)) * START_STRING_SIZE);
            if (!str) {
              LOG_PANIC(
                  "[%ld, %ld]:[%ld, %ld] Error allocating string for token.\n",
                  s_row, s_col, row, col);
            }
          }

          s_row = -1;
          idx = 0;
          special = 0;
          num_delimiter = 0;
          fraction = -1;
          continue;
        } else {
          if (special == 2 && !(c >= '0' && c <= '9') &&
              !(c >= 'a' && c <= 'f')) {
            LOG_PANIC("[%ld, %ld]:[%ld, %ld] %c is not a valid hex symbol.\n",
                    s_row, s_col, row, col, buffer[i]);
          } else if (special == 6 && !(c >= '0' && c <= '9') && !(c == '.')) {
            LOG_PANIC("[%ld, %ld]:[%ld, %ld] %c is not a valid float symbol.\n",
                    s_row, s_col, row, col, buffer[i]);
          } else if (special == 8 && !(c == '0' || c == '1')) {
            LOG_PANIC("[%ld, %ld]:[%ld, %ld] %c is not a valid binary symbol.\n",
                    s_row, s_col, row, col, buffer[i]);
          }

          if (fraction > -1) {
            fraction++;
          }

          if (special == 6 && c == '.') {
            num_delimiter += 1;
            fraction += 1;
            if (num_delimiter > 1) {
              str[idx] = '\0';
              LOG_PANIC(
                  "[%ld, %ld]:[%ld, %ld] %s float not in correct format, one "
                  "too many seperators.\n",
                  s_row, s_col, row, col, str);
            }
          }

          str[idx] = buffer[i];
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
        idx = 0;
        identf = 1;
      }

      if (identf) {
        if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'z') && c != '_') {
          str[idx] = '\0';
          Location loc = {
              .s_col = s_col, .s_row = s_row, .e_col = col, .e_row = row};
          Token el = gen_token(str, loc);
          tokvec_add(vec, &el);

          if (el.str) {
            str = malloc(sizeof((*str)) * START_STRING_SIZE);
            if (!str) {
              LOG_PANIC("Error during string alloc after identifer.\n");
            }
          }

          s_row = -1;
          idx = 0;
          identf = 0;
          special = 0;
        } else {
          str[idx] = c;
          continue;
        }
      }

      if (!comment && !num && !identf && (c >= '0' && c <= '9')) {
        s_row = row;
        s_col = col;
        idx = 0;
        num = 1;
      }

      if (num) {
        if (!(c >= '0' && c <= '9') &&
            !(str[0] == '0' && c == 'x' && idx == 1) &&
            !(str[0] == '0' && c == 'b' && idx == 1) &&
            !(num_delimiter == -1 && c >= 'a' && c <= 'f') &&
            !(str[0] == '0' && c == 'f' && idx == 1) &&
            !(num_delimiter == 1 && c == '.')) {
          str[idx] = '\0';
          enum ttype type = NUM;
          if (num_delimiter != 0) {
            type = num_delimiter == -1 ? HEX : FNUM;
            type = num_delimiter == -2 ? BIN : type;
          }

          if (fraction == 0) {
            LOG_PANIC("[%ld, %ld]:[%ld, %ld] %s float missing fractional part.\n",
                    s_row, s_col, row, col, str);
          } else if (c == '.' && num_delimiter >= 1) {
            LOG_PANIC("[%ld, %ld]:[%ld, %ld] %s float not in correct format, one "
                    "too many seperators.\n",
                    s_row, s_col, row, col, str);
          }

          Token el = {
              .type = type,
              .str = str,
              .location = {
                  .s_col = s_col, .s_row = s_row, .e_col = col, .e_row = row}};
          tokvec_add(vec, &el);

          if (str) {
            str = malloc(sizeof((*str)) * START_STRING_SIZE);
            if (!str) {
              LOG_PANIC("Error during string alloc after number.\n");
            }
          }

          s_row = -1;
          idx = 0;
          num = 0;
          num_delimiter = 0;
          fraction = -1;
          if (type == FNUM) {
            comment = 0;
          }
        } else {
          if (idx == 1 && str[0] == '0' && c == 'x') {
            num_delimiter = -1;
            idx = -1;
            continue;
          } else if (idx == 1 && str[0] == '0' && c == 'f') {
            num_delimiter++;
            idx = -1;
            continue;
          } else if (idx == 1 && str[0] == '0' && c == 'b') {
            num_delimiter = -2;
            idx = -1;
            continue;
          }

          if (num_delimiter == -2 && (c != '0' && c != '1')) {
            LOG_PANIC("[%ld, %ld]:[%ld, %ld] %c is not a valid binary symbol.\n",
                    s_row, s_col, row, col, buffer[i]);
          }

          if (fraction > -1) {
            fraction++;
          }

          if (num_delimiter > 0 && c == '.') {
            num_delimiter++;
            fraction += 1;
            comment = 0;
          }

          str[idx] = c;
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
        tokvec_add(vec, &el);
        break;
      }
      case '=': {
        Token el = {
            .type = LITERAL,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        tokvec_add(vec, &el);
        break;
      }
      case '#': {
        Token el = {
            .type = IMMEDIATE,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        tokvec_add(vec, &el);
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
        tokvec_add(vec, &el);
        break;
      }
      case '-': {
        Token el = {
            .type = MINUS,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        tokvec_add(vec, &el);
        break;
      }
      case '@': {
        Token el = {
            .type = AT,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        tokvec_add(vec, &el);
        break;
      }
      default:
        if (!comment) {
          LOG_PANIC("[%ld:%ld] %c is an illegal symbol.\n", row, col, buffer[i]);
        }
        break;
      }
    }
  }

  free(str);
  free(buffer);
}

void tokvec_init(TokenVector *v) {
  v->count = 0;
  v->capacity = TOKVEC_INITIAL_CAPACITY;
  v->items = malloc(sizeof(*v->items) * v->capacity);

  if (!v->items) {
    LOG_PANIC("Error during token vector init.\n");
  }
}

void tokvec_free(TokenVector *v) {
  if (!v) {
    LOG_PANIC("Error while deallocating the token vector.\n");
  }

  v->count = 0;
  v->capacity = 0;
  free(v->items);
}

void tokvec_free_destructive(TokenVector *v) {
  if (!v) {
    LOG_PANIC("Error while deallocating the token vector.\n");
  }

  for (size_t i = 0; i < v->count; i++) {
    Token token = v->items[i];
    if (token.str != NULL) {
      free(token.str);
    }
  }

  v->count = 0;
  v->capacity = 0;
  free(v->items);
}

void tokvec_add(TokenVector *v, Token *el) {
  if (!v || !el) {
    LOG_PANIC("Error while adding an element to the token vector.\n");
  }

  if (v->count >= v->capacity) {
    v->capacity *= TOKVEC_RESIZE_MULTIPLIER;
    v->items = realloc(v->items, sizeof(*v->items) * v->capacity);
    
    if (!v->items) {
      LOG_PANIC("Error while expanding the token vector from %ld to %ld.\n", v->count,
              v->capacity);
    }
  }

  v->items[v->count++] = *el;
}

void tokvec_replace(TokenVector *v, Token *el, size_t idx) {
  if (!v || !el || idx < 0) {
    LOG_PANIC("Error while replace an element in the token vector.\n");
  }

  if (idx >= v->count ) {
    LOG_PANIC("Error while replacing token at index %ld capacity is %ld.\n", idx,
            v->capacity);
  }

  Token old = v->items[idx];
  v->items[idx] = *el;

  for(size_t i = idx + 1; i < v->count; i++) {
    Token inter = v->items[i];
    v->items[i] = old;
    old = inter;
  }

  tokvec_add(v, &old);
}

void tokvec_rm(TokenVector *v, size_t idx) {
  if (!v || idx < 0) {
    LOG_PANIC("Error while removing an element from the token vector.\n");
  }

  if (idx >= v->capacity) {
    LOG_PANIC("Error while removing token from index %ld capacity is %ld.\n", idx,
            v->capacity);
  }

  int count = v->count;

  for (size_t i = idx + 1; i < count; i++) {
    v->items[i - 1] = v->items[i];
  }

  v->count -= 1;
}

Token *tokvec_get(TokenVector *v, size_t idx) {
  if (!v || idx < 0) {
    LOG_PANIC("Error while adding an element to the token vector.\n");
  }

  if (idx >= v->capacity) {
    LOG_PANIC("Error while getting token from index %ld capacity is %ld.\n", idx,
            v->capacity);
  }

  return &(v->items[idx]);
}

//FIXME: This can literally just be a comparison, why am I using a function?
Token gen_token(char *str, Location loc) {
  Token token = {0};
  uint8_t not_keyword = 0;
  enum ttype typ;
  if (!strncmp(str, "a", 1)) {
    if (strlen(str) == 1) {
      typ = REGISTER;
      token.str = str;
    } else if (!strcmp(str, "add")) {
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
  } else if (!strcmp(str, "b")) {
    typ = REGISTER;
    token.str = str;
  } else if (!strncmp(str, "c", 1)) {
    if (!strcmp(str, "clear")) {
      typ = CLEAR;
    } else if (!strcmp(str, "comp")) {
      typ = COMP;
    } else if (!strcmp(str, "compf")) {
      typ = COMPF;
    } else if (!strcmp(str, "compr")) {
      typ = COMPR;
    } else if (!strcmp(str, "cc")) {
      typ = REGISTER;
      token.str = str;
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
    if (strlen(str) == 1) {
      typ = REGISTER;
      token.str = str;
    } else if (!strcmp(str, "fix")) {
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
  } else if (!strcmp(str, "pc")) {
    typ = REGISTER;
    token.str = str;
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
    if (strlen(str) == 1) {
      typ = REGISTER;
      token.str = str;
    } else if (!strncmp(str, "sw", 2)) {
      typ = REGISTER;
      token.str = str;
    } else if (!strcmp(str, "shiftl")) {
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
    if (strlen(str) == 1) {
      typ = REGISTER;
      token.str = str;
    } else if (!strcmp(str, "td")) {
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
  } else if (!strcmp(str, "x")) {
    typ = REGISTER;
    token.str = str;
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

#if defined(TOKENIZER_DEBUG_MODE) || defined(DEBUG_MODE)
void tokvec_print(TokenVector *v) {
  for(size_t i = 0; i < v->count; i++) {
    Token t = v->items[i];
    token_print(t);
    printf("\n");
  }
}

void token_print(Token t) {
  switch (t.type) {
  case NUM:
    printf("NUM: %s [%ld:%ld] [%ld:%ld]", t.str, t.location.s_row,
           t.location.s_col, t.location.e_row, t.location.e_col);
    break;
  case FNUM:
    printf("FNUM: %s [%ld:%ld] [%ld:%ld]", t.str, t.location.s_row,
           t.location.s_col, t.location.e_row, t.location.e_col);
    break;
  case BIN:
    printf("BIN: %s [%ld:%ld] [%ld:%ld]", t.str, t.location.s_row,
           t.location.s_col, t.location.e_row, t.location.e_col);
    break;
  case ID:
    printf("ID: %s [%ld:%ld] [%ld:%ld]", t.str, t.location.s_row,
           t.location.s_col, t.location.e_row, t.location.e_col);
    break;
  case ADD:
    printf("ADD [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case ADDF:
    printf("ADDF [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case ADDR:
    printf("ADDR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case AND:
    printf("AND [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case CLEAR:
    printf("CLEAR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case COMP:
    printf("COMP [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case COMPF:
    printf("COMPF [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case COMPR:
    printf("COMPR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case DIV:
    printf("DIV [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case DIVF:
    printf("DIVF [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case DIVR:
    printf("DIVR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case FIX:
    printf("FIX [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case FLOAT:
    printf("FLOAT [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case HIO:
    printf("HIO [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case J:
    printf("J [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case JEQ:
    printf("JEQ [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case JGT:
    printf("JGT [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case JLT:
    printf("JLT [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case JSUB:
    printf("JSUB [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LDA:
    printf("LDA [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LDB:
    printf("LDB [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LDCH:
    printf("LDCH [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LDF:
    printf("LDF [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LDL:
    printf("LDL [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LDS:
    printf("LDS [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LDT:
    printf("LDT [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LDX:
    printf("LDX [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LPS:
    printf("LPS [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case MUL:
    printf("MUL [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case MULF:
    printf("MULF [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case MULR:
    printf("MULR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case NORM:
    printf("NORM [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case OR:
    printf("OR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case RD:
    printf("RD [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case RMO:
    printf("RMO [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case RSUB:
    printf("RSUB [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case SHIFTL:
    printf("SHIFTL [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case SHIFTR:
    printf("SHIFTR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case SIO:
    printf("SIO [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case SSK:
    printf("SSK [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STA:
    printf("STA [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STB:
    printf("STB [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STCH:
    printf("STCH [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STF:
    printf("STF [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STI:
    printf("STI [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STL:
    printf("STL [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STS:
    printf("STS [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STSW:
    printf("STSW [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STT:
    printf("STT [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case STX:
    printf("STX [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case SUB:
    printf("SUB [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case SUBF:
    printf("SUBF [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case SUBR:
    printf("SUBR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case SVC:
    printf("SVC [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case TD:
    printf("TD [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case TIO:
    printf("TIO [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case TIX:
    printf("TIX [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case TIXR:
    printf("TIXR [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case WD:
    printf("WD [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case START:
    printf("START [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case END:
    printf("END [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case BYTE:
    printf("BYTE [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case WORD:
    printf("WORD [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case RESB:
    printf("RESB [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case RESW:
    printf("RESW [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case COMMA:
    printf("COMMA [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case LITERAL:
    printf("LITERAL [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case PLUS:
    printf("PLUS [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case MINUS:
    printf("MINUS [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case HEX:
    printf("HEX: %s [%ld:%ld] [%ld:%ld]", t.str, t.location.s_row,
           t.location.s_col, t.location.e_row, t.location.e_col);
    break;
  case STRING:
    printf("STRING: %s [%ld:%ld] [%ld:%ld]", t.str, t.location.s_row,
           t.location.s_col, t.location.e_row, t.location.e_col);
    break;
  case REGISTER:
    printf("REGISTER: %s [%ld:%ld] [%ld:%ld]", t.str, t.location.s_row,
           t.location.s_col, t.location.e_row, t.location.e_col);
    break;
  case AT:
    printf("AT [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  case IMMEDIATE:
    printf("IMMEDIATE [%ld:%ld] [%ld:%ld]", t.location.s_row, t.location.s_col,
           t.location.e_row, t.location.e_col);
    break;
  default:
    break;
  }
}
#endif
