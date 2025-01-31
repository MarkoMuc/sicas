#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "../includes/tokenizer.h"
#endif

bool fill(FILE *f, TokenVector *vec) {
  size_t read_c;
  char *buffer;
  Sicstr sicstr;
  uint64_t row = 1;
  uint64_t col = 0;
  bool num = false;
  bool identf = false;
  bool comment = false;
  bool special_char = false;
  bool num_init = false;
  enum special_token special = NON_T;
  int32_t num_delimiter = 0;
  int8_t fraction = -1;
  uint64_t s_row = 0;
  uint64_t s_col = 0;

  buffer = malloc(sizeof((*buffer)) * TOKENIZER_START_BUFFER_SIZE);
  if (!buffer) {
    LOG_PANIC("Error during inital buffer alloc for token.");
  }

  sicstr_init(&sicstr);

  while ((read_c = fread(buffer, sizeof(char), TOKENIZER_START_BUFFER_SIZE, f)) > 1) {
    for (size_t i = 0; i < read_c; i++) {
      char c = buffer[i];
      col++;

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
      } else if (special % 2 != 0 && sicstr.count == 1 && c == '\'') {
        special++;
        sicstr_reset(&sicstr);
        identf = false;
        continue;
      }

      if (special % 2 == 0 && special) {
        if (c == '\'') {
          sicstr_fin(&sicstr);
          enum ttype type = HEX;

          if (special > HEX_T) {
            type = special == CHAR_T ? STRING : FNUM;
            type = special == BIN_T ? BIN : type;
          }

          if(sicstr.count < 1 && special != CHAR_T) {
            LOG_XERR(LOCATION_LOG "Only a char or string can be empty.\n", s_row, s_col, row, col);
          }

          if(type == STRING && special_char){
            LOG_XERR(LOCATION_LOG "'%s' string or char has an invalid escape sequence.\n", s_row, s_col, row, col, sicstr_dump(&sicstr));
          }else if (type == FNUM && (fraction < 0 || sicstr_get(&sicstr, sicstr_lst(&sicstr)) == '.')) {
            LOG_XERR(LOCATION_LOG "'%s' float missing fractional part.\n", s_row, s_col, row, col, sicstr_dump(&sicstr));
          }

          Token el = {.str = sicstr, .type = type, .location = 
            {.s_row = s_row, .s_col = s_col, .e_row = row, .e_col = col - 1}};

          tokvec_add(vec, &el);

          sicstr_init(&sicstr);
          special = NON_T;
          num_delimiter = 0;
          fraction = -1;
          continue;
        } else {
          if(special != CHAR_T && sicstr.count >= 1 && c == '_') {
            continue;
          } else if (special == HEX_T && !is_numeric(c) && !(c >= 'a' && c <= 'f')) {
            LOG_XERR(LOCATION_LOG "'%c' is not a valid hex symbol.\n", s_row, s_col, row, col, buffer[i]);
          } else if (special == FLOAT_T && !is_numeric(c) && !(c == '.')) {
            LOG_XERR(LOCATION_LOG "'%c' is not a valid float symbol.\n", s_row, s_col, row, col, buffer[i]);
          } else if (special == BIN_T && !(c == '0' || c == '1')) {
            LOG_XERR(LOCATION_LOG "'%c' is not a valid binary symbol.\n", s_row, s_col, row, col, buffer[i]);
          } else if (special == CHAR_T && special_char && !is_specialchar(c)){
            LOG_XERR(LOCATION_LOG "'%c' is not a valid special character.\n", s_row, s_col, row, col, buffer[i]);
          }

          if (fraction > -1) {
            fraction++;
          }

          if(special_char){
            special_char = false;
          }else if(special == CHAR_T && c == '\\'){
            special_char = true;
          }

          if (special == FLOAT_T) {
            if(c == '.') {
              if(fraction == -1 && sicstr.count < 1) {
                sicstr_fin(&sicstr);
                LOG_XERR(LOCATION_LOG "Float is missing the decimal part.\n", s_row, s_col, row, col);
              }
              num_delimiter += 1;
              fraction += 1;
              if (num_delimiter > 1) {
                sicstr_fin(&sicstr);
                LOG_XERR(LOCATION_LOG "'%s' float is not in correct format, one too many seperators.\n", s_row, s_col, row, col, sicstr_dump(&sicstr));
              }
            }
          }

          sicstr_build(&sicstr, buffer[i]);
          continue;
        }
      }

      if (c == '.') {
        comment = true;
      } else if (comment && c != '\n') {
        continue;
      }

      if (!comment && !num && !identf && (is_alphabetic(c) || c == '_')) {
        s_row = row;
        s_col = col;
        sicstr_reset(&sicstr);
        identf = true;
      }

      if (identf) {
        if (!is_alphanumeric(c) && c != '_') {
          sicstr_fin(&sicstr);
          Location loc = { .s_row = s_row, .s_col = s_col, .e_row = row, .e_col = col - 1};
          Token el = gen_token(&sicstr, loc);
          tokvec_add(vec, &el);

          sicstr_init(&sicstr);

          identf = false;
          special = NON_T;
        } else {
          sicstr_build(&sicstr, buffer[i]);
          continue;
        }
      }

      if (!comment && !num && !identf && is_numeric(c)) {
        s_row = row;
        s_col = col;
        sicstr_reset(&sicstr);
        num = true;
      }

      if (num) {
        if (!is_numeric(c) && 
            !(c == '_' && sicstr.count >= 1 && num_init) &&
            !(!num_init && sicstr_fst(&sicstr) == '0' && c == 'x' && sicstr.count == 1) &&
            !(!num_init && sicstr_fst(&sicstr) == '0' && c == 'b' && sicstr.count == 1) &&
            !(!num_init && sicstr_fst(&sicstr) == '0' && c == 'f' && sicstr.count == 1) &&
            !(num_delimiter == -1 && c >= 'a' && c <= 'f') &&
            !(num_delimiter == 1 && c == '.')) {
          sicstr_fin(&sicstr);
          enum ttype type = NUM;
          if (num_delimiter != 0) {
            type = num_delimiter == -1 ? HEX : FNUM;
            type = num_delimiter == -2 ? BIN : type;
          }

          if(sicstr.count < 1){
            LOG_XERR(LOCATION_LOG "Only characters or strings can be empty.\n", s_row, s_col, row, col);
          }

          if (type == FNUM ) {
            if (c == '.' && num_delimiter >= 1) {
              LOG_XERR(LOCATION_LOG "'%s' Float not in correct format, one too many seperators.\n", s_row, s_col, row, col, sicstr_dump(&sicstr));
            } else if(fraction <= 0){
              LOG_XERR(LOCATION_LOG "'%s' Float missing fractional part.\n", s_row, s_col, row, col, sicstr_dump(&sicstr));
            }
          }

          Token el = { .type = type, .str = sicstr, .location = 
            { .s_row = s_row, .s_col = s_col, .e_row = row, .e_col = col - 1}};
          tokvec_add(vec, &el);

          sicstr_init(&sicstr);

          num = false;
          num_delimiter = 0;
          fraction = -1;
          if(type == FNUM) {
            comment = false;
          }
          num_init = false;
        } else {
          if (!num_init && sicstr.count == 1 && sicstr_fst(&sicstr) == '0' && c == 'x') {
            num_delimiter = -1;
            sicstr_reset(&sicstr);
            num_init = true;
            continue;
          } else if (!num_init && sicstr.count == 1 && sicstr_fst(&sicstr) == '0' && c == 'f') {
            num_delimiter++;
            sicstr_reset(&sicstr);
            num_init = true;
            continue;
          } else if (!num_init && sicstr.count == 1 && sicstr_fst(&sicstr) == '0' && c == 'b') {
            num_delimiter = -2;
            sicstr_reset(&sicstr);
            num_init = true;
            continue;
          }

          if(sicstr.count >= 1 && c == '_') {
            continue;
          }

          if (num_delimiter == -2 && (c != '0' && c != '1')) {
            LOG_XERR(LOCATION_LOG "'%c' is not a valid binary symbol.\n", s_row, s_col, row, col, buffer[i]);
          }

          if (fraction > -1) {
            fraction++;
          }

          if (num_delimiter > 0 && c == '.') {
            if(num_delimiter == 1 && sicstr.count < 1) {
                sicstr_fin(&sicstr);
                LOG_XERR(LOCATION_LOG "Float is missing the decimal part.\n", s_row, s_col, row, col);
            }
            num_delimiter++;
            fraction += 1;
            comment = false;
          }

          sicstr_build(&sicstr, buffer[i]);
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
        sicstr_build(&sicstr, buffer[i]);
        sicstr_fin(&sicstr);
        Token el = {
            .type = COMMA, .str = sicstr,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        sicstr_init(&sicstr);
        break;
      }
      case '=': {
        sicstr_build(&sicstr, buffer[i]);
        sicstr_fin(&sicstr);
        Token el = {
            .type = LITERAL, .str = sicstr,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        sicstr_init(&sicstr);
        break;
      }
      case '#': {
        sicstr_build(&sicstr, buffer[i]);
        sicstr_fin(&sicstr);
        Token el = {
            .type = HASH, .str = sicstr,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        sicstr_init(&sicstr);
        break;
      }
      case '\n': {
        comment = false;
        row += 1;
        col = 0;
        break;
      }
      case '+': {
        sicstr_build(&sicstr, buffer[i]);
        sicstr_fin(&sicstr);
        Token el = {
            .type = PLUS, .str = sicstr,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        sicstr_init(&sicstr);
        break;
      }
      case '-': {
        sicstr_build(&sicstr, buffer[i]);
        sicstr_fin(&sicstr);
        Token el = {
            .type = MINUS, .str = sicstr,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        sicstr_init(&sicstr);
        break;
      }
      case '@': {
        sicstr_build(&sicstr, buffer[i]);
        sicstr_fin(&sicstr);
        Token el = {
            .type = AT, .str = sicstr,
            .location = { .s_row = row, .s_col = col, .e_row = row, .e_col = col }};
        tokvec_add(vec, &el);
        sicstr_init(&sicstr);
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

  if(sicstr.count == 0) {
    sicstr_free_destructive_stack(&sicstr);
  }

  free(buffer);

  return vec->count == 0;
}

void tokvec_init(TokenVector *v) {
  v->count = 0;
  v->capacity = TOKVEC_INITIAL_CAPACITY;
  v->items = malloc(sizeof(*v->items) * v->capacity);

  if (!v->items) {
    LOG_PANIC("Error during token vector init.");
  }
}

void tokvec_free(TokenVector *v) {
  if (!v) {
    LOG_PANIC("Error while deallocating the token vector.");
  }

  v->count = 0;
  v->capacity = 0;
  free(v->items);
}

void tokvec_free_destructive(TokenVector *v) {
  if (!v) {
    LOG_PANIC("Error while deallocating the token vector.");
  }

  const size_t count = v->count;
  for (size_t i = 0; i < count; i++) {
    Token token = v->items[i];
    sicstr_free_destructive_stack(&token.str);
  }

  v->count = 0;
  v->capacity = 0;
  free(v->items);
}

void tokvec_add(TokenVector *v, const Token *el) {
  if (!v || !el) {
    LOG_PANIC("Error while adding an element to the token vector.");
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

void tokvec_add_at(TokenVector *v, const Token *el, const size_t idx) {
  if (!v || !el || idx < 0) {
    LOG_PANIC("Error while replacing an element in the token vector.");
  }

  if (idx >= v->count) {
    LOG_PANIC("Error while replacing token at index %ld current count is %ld.\n", idx,
            v->count);
  }

  const size_t count = v->count;
  Token old = v->items[idx];
  v->items[idx] = *el;
  for(size_t i = idx + 1; i < count; i++) {
    Token inter = v->items[i];
    v->items[i] = old;
    old = inter;
  }

  tokvec_add(v, &old);
}

void tokvec_rm(TokenVector *v, const size_t idx) {
  if (!v || idx < 0) {
    LOG_PANIC("Error while removing an element from the token vector.");
  }

  if (idx >= v->capacity) {
    LOG_PANIC("Error while removing token from index %ld capacity is %ld.\n", idx,
            v->capacity);
  }

  const int count = v->count;

  for (size_t i = idx + 1; i < count; i++) {
    v->items[i - 1] = v->items[i];
  }

  v->count -= 1;
}

Token *tokvec_get(const TokenVector *v, const size_t idx) {
  if (!v || idx < 0) {
    LOG_PANIC("Error while adding an element to the token vector.");
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

Token gen_token(Sicstr *sicstr, const Location loc) {
  Token token = {0};
  uint8_t not_keyword = 0;
  enum ttype typ;
  char* str = sicstr->str;
  if (!strncasecmp(str, "a", 1)) {
    if (strlen(str) == 1) {
      typ = REGISTER;
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
    } else if (!strncasecmp(str, "sw", 2)) {
      typ = REGISTER;
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
  } else if (!strcasecmp(str, "end")) {
    typ = END;
  } else {
    not_keyword = 1;
  }

  token.str = *sicstr;
  if (not_keyword) {
    typ = ID;
  }

  token.type = typ;
  token.location = loc;

  return token;
}

void token_print(Token t) {
  if(t.type == NUM || t.type == FNUM || t.type == BIN || t.type == ID ||
     t.type == HEX || t.type == STRING ||t.type == REGISTER){
    token_type_print(t.type);
    printf(" '%s' ", sicstr_dump(&t.str));
  } else {
    printf("%s", sicstr_dump(&t.str));
  }
}

bool token_type_print(enum ttype tk_type){
  switch (tk_type) {
    case NUM:
      printf("NUM");
      return true;
    case FNUM:
      printf("FNUM");
      return true;
    case BIN:
      printf("BIN");
      return true;
    case ID:
      printf("ID");
      return true;
    case HEX:
      printf("HEX");
      return true;
    case STRING:
      printf("STRING");
      return true;
    case REGISTER:
      printf("REGISTER");
      return true;
    default:
      return false;
  }
}

#if defined(TOKENIZER_DEBUG_MODE) || defined(PARSER_DEBUG_MODE) || defined(DEBUG_MODE)
void token_loc_print(Token t) {
  printf(LOCATION_LOG ,t.location.s_row, t.location.s_col, t.location.e_row, t.location.e_col);
  token_print(t);
}

void tokvec_print(TokenVector *v) {
  for(size_t i = 0; i < v->count; i++) {
    Token t = v->items[i];
    token_loc_print(t);
    printf("\n");
  }
}
#endif
