#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "../includes/tokenizer.h"
#endif

void fill(FILE *f, TokenVector *vec) {
  size_t read_c;
  char *buffer;
  char *str;
  uint64_t row = 1;
  uint64_t col = 0;
  size_t idx = 0;
  uint8_t num = 0;
  uint8_t identf = 0;
  uint8_t comment = 0;
  enum special_token special = NON_T;
  int32_t num_delimiter = 0;
  int8_t fraction = -1;
  uint64_t s_row = 0;
  uint64_t s_col = 0;

  buffer = malloc(sizeof((*buffer)) * TOKENIZER_START_BUFFER_SIZE);
  if (!buffer) {
    LOG_PANIC("Error during inital buffer alloc for token.\n");
  }

  str = malloc(sizeof((*str)) * TOKENIZER_START_STRING_SIZE);
  if (!str) {
    LOG_PANIC("Error during inital string alloc for token.\n");
  }

  while ((read_c = fread(buffer, sizeof(char), TOKENIZER_START_BUFFER_SIZE, f)) > 1) {
    for (size_t i = 0; i < read_c; i++) {
      char c = buffer[i];
      col++;
      idx++;

      if (c >= 'A' && c <= 'Z') {
        c = c + 32;
      }

      if (special == NON_T && c == 'x' && !comment) {
        special = HEXP_T;
      } else if (special == NON_T && c == 'c' && !comment) {
        special = CHARP_T;
      } else if (special == NON_T && c == 'f' && !num && !comment) {
        special = FLOATP_T;
      } else if (special == NON_T && c == 'b' && !num && !comment) {
        special = BINP_T;
      } else if (special % 2 != 0 && idx == 1 && c == '\'') {
        special++;
        idx = -1;
        identf = 0;
        continue;
      }

      if (special % 2 == 0 && special) {
        if (c == '\'') {
          str[idx] = '\0';
          Location loc = { .s_row = s_row, .s_col = s_col, .e_row = row, .e_col = col - 1};
          enum ttype type = HEX;

          if (special > HEX_T) {
            type = special == CHAR_T ? STRING : FNUM;
            type = special == BIN_T ? BIN : type;
          }

          if(idx < 1 && special != CHAR_T) {
            LOG_XERR("[%ld,%ld]:[%ld,%ld] Only a char or string can be empty.\n", s_row, s_col, row, col);
          }

          if (type == FNUM && (fraction < 0 || str[idx-1] == '.')) {
            LOG_XERR("[%ld,%ld]:[%ld,%ld] '%s' float missing fractional part.\n", s_row, s_col, row, col, str);
          }

          Token el = {.str = str, .type = type, .location = loc};
          tokvec_add(vec, &el);

          if (el.str) {
            str = malloc(sizeof((*str)) * TOKENIZER_START_STRING_SIZE);
            if (!str) {
              LOG_PANIC("[%ld,%ld]:[%ld,%ld] Error allocating string for token.\n", s_row, s_col, row, col);
            }
          }

          idx = 0;
          special = NON_T;
          num_delimiter = 0;
          fraction = -1;
          continue;
        } else {
          if (special == HEX_T && !is_numeric(c) && !(c >= 'a' && c <= 'f')) {
            LOG_XERR("[%ld,%ld]:[%ld,%ld] '%c' is not a valid hex symbol.\n", s_row, s_col, row, col, buffer[i]);
          } else if (special == FLOAT_T && !is_numeric(c) && !(c == '.')) {
            LOG_XERR("[%ld,%ld]:[%ld,%ld] '%c' is not a valid float symbol.\n", s_row, s_col, row, col, buffer[i]);
          } else if (special == BIN_T && !(c == '0' || c == '1')) {
            LOG_XERR("[%ld,%ld]:[%ld,%ld] '%c' is not a valid binary symbol.\n", s_row, s_col, row, col, buffer[i]);
          }

          if (fraction > -1) {
            fraction++;
          }

          if (special == FLOAT_T) {
            if(c == '.') {
              if(fraction == -1 && idx < 1) {
                str[idx] = '\0';
                LOG_XERR("[%ld,%ld]:[%ld,%ld] Float is missing the decimal part.\n", s_row, s_col, row, col);
              }
              num_delimiter += 1;
              fraction += 1;
              if (num_delimiter > 1) {
                str[idx] = '\0';
                LOG_XERR("[%ld,%ld]:[%ld,%ld] '%s' float is not in correct format, one too many seperators.\n", s_row, s_col, row, col, str);
              }
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

      if (!comment && !num && !identf && (is_alphabetic(c) || c == '_')) {
        s_row = row;
        s_col = col;
        idx = 0;
        identf = 1;
      }

      if (identf) {
        if (!is_alphanumeric(c) && c != '_') {
          str[idx] = '\0';
          Location loc = { .s_row = s_row, .s_col = s_col, .e_row = row, .e_col = col - 1};
          Token el = gen_token(str, loc);
          tokvec_add(vec, &el);

          if (el.str) {
            str = malloc(sizeof((*str)) * TOKENIZER_START_STRING_SIZE);
            if (!str) {
              LOG_PANIC("Error during string alloc after identifer.\n");
            }
          }

          idx = 0;
          identf = 0;
          special = NON_T;
        } else {
          str[idx] = buffer[i];
          continue;
        }
      }

      if (!comment && !num && !identf && is_numeric(c)) {
        s_row = row;
        s_col = col;
        idx = 0;
        num = 1;
      }

      if (num) {
        if (!is_numeric(c) &&
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

          if (type == FNUM ) {
            if (c == '.' && num_delimiter >= 1) {
              LOG_XERR("[%ld,%ld]:[%ld,%ld] '%s' Float not in correct format, one too many seperators.\n", s_row, s_col, row, col, str);
            } else if(fraction <= 0){
              LOG_XERR("[%ld,%ld]:[%ld,%ld] '%s' Float missing fractional part.\n", s_row, s_col, row, col, str);
            }
          }
          if(idx < 1){
            LOG_XERR("[%ld,%ld]:[%ld,%ld] Only characters or strings can be empty.\n", s_row, s_col, row, col);
          }

          Token el = {
              .type = type,
              .str = str,
              .location = { .s_row = s_row, .s_col = s_col, .e_row = row, .e_col = col - 1}};
          tokvec_add(vec, &el);

          if (str) {
            str = malloc(sizeof((*str)) * TOKENIZER_START_STRING_SIZE);
            if (!str) {
              LOG_PANIC("Error during string alloc after number.\n");
            }
          }

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
            LOG_XERR("[%ld,%ld]:[%ld,%ld] '%c' is not a valid binary symbol.\n", s_row, s_col, row, col, buffer[i]);
          }

          if (fraction > -1) {
            fraction++;
          }

          if (num_delimiter > 0 && c == '.') {
            if(num_delimiter == 1 && idx < 1) {
                str[idx] = '\0';
                LOG_XERR("[%ld,%ld]:[%ld,%ld] Float is missing the decimal part.\n", s_row, s_col, row, col);
            }
            num_delimiter++;
            fraction += 1;
            comment = 0;
          }

          str[idx] = buffer[i];
          continue;
        }
      }

      switch (c) {
      case ' ':
        break;
      case '\t':
        col = col - 1;
        col += TOKENIZER_TAB_WIDTH - (col % TOKENIZER_TAB_WIDTH);
        break;
      case ',': {
        Token el = {
            .type = COMMA, .str = NULL,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        break;
      }
      case '=': {
        Token el = {
            .type = LITERAL, .str = NULL,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        break;
      }
      case '#': {
        Token el = {
            .type = HASH, .str = NULL,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
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
            .type = PLUS, .str = NULL,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        break;
      }
      case '-': {
        Token el = {
            .type = MINUS, .str = NULL,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        break;
      }
      case '@': {
        Token el = {
            .type = AT, .str = NULL,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        break;
      }
      default:
        if (!comment) {
          LOG_XERR("[%ld:%ld] '%c' is an illegal symbol.\n", row, col, buffer[i]);
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
    LOG_PANIC("Error while replacing an element in the token vector.\n");
  }

  if (idx >= v->count) {
    LOG_PANIC("Error while replacing token at index %ld current count is %ld.\n", idx,
            v->count);
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

  if (idx >= v->count) {
    LOG_PANIC("Error while getting token from index %ld count is %ld.\n", idx,
            v->count);
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
  if (!strncasecmp(str, "a", 1)) {
    if (strlen(str) == 1) {
      typ = REGISTER;
      token.str = str;
    } else if (!strcasecmp(str, "add")) {
      typ = ADD;
    } else if (!strcasecmp(str, "addf")) {
      typ = ADDF;
    } else if (!strcasecmp(str, "addr")) {
      typ = ADDR;
    } else if (!strcasecmp(str, "and")) {
      typ = AND;
    } else {
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "b", 1)) {
    if (!strcasecmp(str, "base")) {
      typ = BASE;
    } else if (!strcasecmp(str, "byte")) {
      typ = BYTE;
    } else if (!strcasecmp(str, "b")) {
      typ = REGISTER;
      token.str = str;
    }else{
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "c", 1)) {
    if (!strcasecmp(str, "clear")) {
      typ = CLEAR;
    } else if (!strcasecmp(str, "comp")) {
      typ = COMP;
    } else if (!strcasecmp(str, "compf")) {
      typ = COMPF;
    } else if (!strcasecmp(str, "compr")) {
      typ = COMPR;
    } else if (!strcasecmp(str, "cc")) {
      typ = REGISTER;
      token.str = str;
    } else {
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "d", 1)) {
    if (!strcasecmp(str, "div")) {
      typ = DIV;
    } else if (!strcasecmp(str, "divf")) {
      typ = DIVF;
    } else if (!strcasecmp(str, "divr")) {
      typ = DIVR;
    } else {
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "f", 1)) {
    if (strlen(str) == 1) {
      typ = REGISTER;
      token.str = str;
    } else if (!strcasecmp(str, "fix")) {
      typ = FIX;
    } else if (!strcasecmp(str, "float")) {
      typ = FLOAT;
    } else {
      not_keyword = 1;
    }
  } else if (!strcasecmp(str, "hio")) {
    typ = HIO;
  } else if (!strncasecmp(str, "j", 1)) {
    if (!strcasecmp(str, "j")) {
      typ = J;
    } else if (!strcasecmp(str, "jeq")) {
      typ = JEQ;
    } else if (!strcasecmp(str, "jgt")) {
      typ = JGT;
    } else if (!strcasecmp(str, "jlt")) {
      typ = JLT;
    } else if (!strcasecmp(str, "jsub")) {
      typ = JSUB;
    } else {
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "l", 1)) {
    if (!strcasecmp(str, "lda")) {
      typ = LDA;
    } else if (!strcasecmp(str, "ldb")) {
      typ = LDB;
    } else if (!strcasecmp(str, "ldch")) {
      typ = LDCH;
    } else if (!strcasecmp(str, "ldf")) {
      typ = LDF;
    } else if (!strcasecmp(str, "ldl")) {
      typ = LDL;
    } else if (!strcasecmp(str, "lds")) {
      typ = LDS;
    } else if (!strcasecmp(str, "ldt")) {
      typ = LDT;
    } else if (!strcasecmp(str, "ldx")) {
      typ = LDX;
    } else if (!strcasecmp(str, "lps")) {
      typ = LPS;
    } else {
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "m", 1)) {
    if (!strcasecmp(str, "mul")) {
      typ = MUL;
    } else if (!strcasecmp(str, "mulf")) {
      typ = MULF;
    } else if (!strcasecmp(str, "mulr")) {
      typ = MULR;
    } else {
      not_keyword = 1;
    }
  } else if (!strcasecmp(str, "norm")) {
    typ = NORM;
  } else if (!strcasecmp(str, "or")) {
    typ = OR;
  } else if (!strcasecmp(str, "pc")) {
    typ = REGISTER;
    token.str = str;
  } else if (!strncasecmp(str, "r", 1)) {
    if (!strcasecmp(str, "rd")) {
      typ = RD;
    } else if (!strcasecmp(str, "rmo")) {
      typ = RMO;
    } else if (!strcasecmp(str, "rsub")) {
      typ = RSUB;
    } else if (!strcasecmp(str, "resb")) {
      typ = RESB;
    } else if (!strcasecmp(str, "resw")) {
      typ = RESW;
    } else {
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "s", 1)) {
    if (strlen(str) == 1) {
      typ = REGISTER;
      token.str = str;
    } else if (!strncasecmp(str, "sw", 2)) {
      typ = REGISTER;
      token.str = str;
    } else if (!strcasecmp(str, "shiftl")) {
      typ = SHIFTL;
    } else if (!strcasecmp(str, "shiftr")) {
      typ = SHIFTR;
    } else if (!strcasecmp(str, "sio")) {
      typ = SIO;
    } else if (!strcasecmp(str, "ssk")) {
      typ = SSK;
    } else if (!strcasecmp(str, "sta")) {
      typ = STA;
    } else if (!strcasecmp(str, "stb")) {
      typ = STB;
    } else if (!strcasecmp(str, "stch")) {
      typ = STCH;
    } else if (!strcasecmp(str, "stf")) {
      typ = STF;
    } else if (!strcasecmp(str, "sti")) {
      typ = STI;
    } else if (!strcasecmp(str, "stl")) {
      typ = STL;
    } else if (!strcasecmp(str, "sts")) {
      typ = STS;
    } else if (!strcasecmp(str, "stsw")) {
      typ = STSW;
    } else if (!strcasecmp(str, "stt")) {
      typ = STT;
    } else if (!strcasecmp(str, "stx")) {
      typ = STX;
    } else if (!strcasecmp(str, "sub")) {
      typ = SUB;
    } else if (!strcasecmp(str, "subf")) {
      typ = SUBF;
    } else if (!strcasecmp(str, "subr")) {
      typ = SUBR;
    } else if (!strcasecmp(str, "svc")) {
      typ = SVC;
    } else if (!strcasecmp(str, "start")) {
      typ = START;
    } else {
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "t", 1)) {
    if (strlen(str) == 1) {
      typ = REGISTER;
      token.str = str;
    } else if (!strcasecmp(str, "td")) {
      typ = TD;
    } else if (!strcasecmp(str, "tio")) {
      typ = TIO;
    } else if (!strcasecmp(str, "tix")) {
      typ = TIX;
    } else if (!strcasecmp(str, "tixr")) {
      typ = TIXR;
    } else {
      not_keyword = 1;
    }
  } else if (!strncasecmp(str, "w", 1)) {
    if (!strcasecmp(str, "wd")) {
      typ = WD;
    } else if (!strcasecmp(str, "word")) {
      typ = WORD;
    } else {
      not_keyword = 1;
    }
  } else if (!strcasecmp(str, "x")) {
    typ = REGISTER;
    token.str = str;
  } else if (!strcasecmp(str, "end")) {
    typ = END;
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

#if defined(TOKENIZER_DEBUG_MODE) || defined(PARSER_DEBUG_MODE) || defined(DEBUG_MODE)
void tokvec_print(TokenVector *v) {
  for(size_t i = 0; i < v->count; i++) {
    Token t = v->items[i];
    token_print(t);
    printf("\n");
  }
}

void token_print(Token t) {
  token_type_print(t.type);
  if(t.type == NUM || t.type == FNUM || t.type == BIN || t.type == ID ||
     t.type == HEX || t.type == STRING ||t.type == REGISTER){
    printf(" '%s", t.str);
  }

  printf("' [%ld:%ld] [%ld:%ld]",t.location.s_row, t.location.s_col, t.location.e_row, t.location.e_col);
}

void token_type_print(enum ttype tk_type){
 switch (tk_type) {
  case NUM:
    printf("NUM");
    break;
  case FNUM:
    printf("FNUM");
    break;
  case BIN:
    printf("BIN");
    break;
  case ID:
    printf("ID");
    break;
  case ADD:
    printf("ADD");
    break;
  case ADDF:
    printf("ADDF");
    break;
  case ADDR:
    printf("ADDR");
    break;
  case AND:
    printf("AND");
    break;
  case CLEAR:
    printf("CLEAR");
    break;
  case COMP:
    printf("COMP");
    break;
  case COMPF:
    printf("COMPF");
    break;
  case COMPR:
    printf("COMPR");
    break;
  case DIV:
    printf("DIV");
    break;
  case DIVF:
    printf("DIVF");
    break;
  case DIVR:
    printf("DIVR");
    break;
  case FIX:
    printf("FIX");
    break;
  case FLOAT:
    printf("FLOAT");
    break;
  case HIO:
    printf("HIO");
    break;
  case J:
    printf("J");
    break;
  case JEQ:
    printf("JEQ");
    break;
  case JGT:
    printf("JGT");
    break;
  case JLT:
    printf("JLT");
    break;
  case JSUB:
    printf("JSUB");
    break;
  case LDA:
    printf("LDA");
    break;
  case LDB:
    printf("LDB");
    break;
  case LDCH:
    printf("LDCH");
    break;
  case LDF:
    printf("LDF");
    break;
  case LDL:
    printf("LDL");
    break;
  case LDS:
    printf("LDS");
    break;
  case LDT:
    printf("LDT");
    break;
  case LDX:
    printf("LDX");
    break;
  case LPS:
    printf("LPS");
    break;
  case MUL:
    printf("MUL");
    break;
  case MULF:
    printf("MULF");
    break;
  case MULR:
    printf("MULR");
    break;
  case NORM:
    printf("NORM");
    break;
  case OR:
    printf("OR");
    break;
  case RD:
    printf("RD");
    break;
  case RMO:
    printf("RMO");
    break;
  case RSUB:
    printf("RSUB");
    break;
  case SHIFTL:
    printf("SHIFTL");
    break;
  case SHIFTR:
    printf("SHIFTR");
    break;
  case SIO:
    printf("SIO");
    break;
  case SSK:
    printf("SSK");
    break;
  case STA:
    printf("STA");
    break;
  case STB:
    printf("STB");
    break;
  case STCH:
    printf("STCH");
    break;
  case STF:
    printf("STF");
    break;
  case STI:
    printf("STI");
    break;
  case STL:
    printf("STL");
    break;
  case STS:
    printf("STS");
    break;
  case STSW:
    printf("STSW");
    break;
  case STT:
    printf("STT");
    break;
  case STX:
    printf("STX");
    break;
  case SUB:
    printf("SUB");
    break;
  case SUBF:
    printf("SUBF");
    break;
  case SUBR:
    printf("SUBR");
    break;
  case SVC:
    printf("SVC");
    break;
  case TD:
    printf("TD");
    break;
  case TIO:
    printf("TIO");
    break;
  case TIX:
    printf("TIX");
    break;
  case TIXR:
    printf("TIXR");
    break;
  case WD:
    printf("WD");
    break;
  case START:
    printf("START");
    break;
  case END:
    printf("END");
    break;
  case BASE:
    printf("BASE");
    break;
  case BYTE:
    printf("BYTE");
    break;
  case WORD:
    printf("WORD");
    break;
  case RESB:
    printf("RESB");
    break;
  case RESW:
    printf("RESW");
    break;
  case COMMA:
    printf("COMMA");
    break;
  case LITERAL:
    printf("LITERAL");
    break;
  case PLUS:
    printf("PLUS");
    break;
  case MINUS:
    printf("MINUS");
    break;
  case HEX:
    printf("HEX");
    break;
  case STRING:
    printf("STRING");
    break;
  case REGISTER:
    printf("REGISTER");
    break;
  case AT:
    printf("AT");
    break;
  case HASH:
    printf("HASH");
    break;
  default:
      LOG_ERR("UNKNOWN");
    break;
  }
}

#endif
