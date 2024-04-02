#include "tokenizer.h"

void fill(FILE *f, Vector *vec) {
  int read_c;
  long row = 0;
  long col = 0;
  int index = 0;
  uint8_t num = 0;
  uint8_t identf = 0;
  uint8_t comment = 0;
  char *buffer;
  char *str;
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

      if (c == '.') {
        comment = 1;
      } else if (c != '\n' && comment) {
        continue;
      }

      if (!comment && !num && ((c >= 'a' && c <= 'z') || c == '_')) {
        if (s_row == -1) {
          s_row = row;
          s_col = col;
          index = 0;
        }

        str[index] = c;
        identf = 1;
      } else if (identf) {
        // if (!(c >= 'a' && c <= 'z') && c != '_') {
        //   printf("Illegal identifier at position %ld:%ld", row, col);
        //   exit(1);
        // }
        str[index] = '\0';
        Location loc = {
            .s_col = s_col, .s_row = s_row, .e_col = col, .e_row = row};
        Token el = gen_token(str, loc);
        add_el(vec, &el);

        if (el.str != NULL) {
          str = malloc(sizeof((*str)) * 128);
        }

        s_row = -1;
        identf = 0;
        index = 0;
      }

      if (!comment && !identf && c >= '0' && c <= '9') {
        if (s_row == -1) {
          s_row = row;
          s_col = col;
          index = 0;
        }
        str[index] = c;
        num = 1;
      } else if (num) {

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
      }

      if (c == ' ') {
        continue;
      } else if (c == ',') {
        Token el = {
            .type = COMMA,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        add_el(vec, &el);
      } else if (c == '\n') {
        comment = 0;
        row += 1;
        col = 0;
      } else if (!comment && !num && !identf) {
        printf("[%ld:%ld] Illegal symbol %c\n", row, col, c);
        exit(1);
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

int main() {
  FILE *f = fopen("./test/test.S", "r");
  if (f == NULL) {
    printf("File not found.");
    exit(1);
  }

  Vector vec = {};
  init(&vec);

  fill(f, &vec);
#ifdef DEBUG_MODE
  printv(&vec);
#endif
  free_vec(&vec);
  fclose(f);
  exit(0);
}

Token gen_token(char *str, Location loc) {
  Token token = {};
  enum ttype typ;
  if (!strcmp(str, "add")) {
    typ = ADD;
  } else if (!strcmp(str, "addf")) {
    typ = ADDF;
  } else if (!strcmp(str, "addr")) {
    typ = ADDR;
  } else if (!strcmp(str, "and")) {
    typ = AND;
  } else if (!strcmp(str, "clear")) {
    typ = CLEAR;
  } else if (!strcmp(str, "comp")) {
    typ = COMP;
  } else if (!strcmp(str, "compf")) {
    typ = COMPF;
  } else if (!strcmp(str, "compr")) {
    typ = COMPR;
  } else if (!strcmp(str, "div")) {
    typ = DIV;
  } else if (!strcmp(str, "divf")) {
    typ = DIVF;
  } else if (!strcmp(str, "divr")) {
    typ = DIVR;
  } else if (!strcmp(str, "fix")) {
    typ = FIX;
  } else if (!strcmp(str, "float")) {
    typ = FLOAT;
  } else if (!strcmp(str, "hio")) {
    typ = HIO;
  } else if (!strcmp(str, "j")) {
    typ = J;
  } else if (!strcmp(str, "jeq")) {
    typ = JEQ;
  } else if (!strcmp(str, "jgt")) {
    typ = JGT;
  } else if (!strcmp(str, "jlt")) {
    typ = JLT;
  } else if (!strcmp(str, "jsub")) {
    typ = JSUB;
  } else if (!strcmp(str, "lda")) {
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
  } else if (!strcmp(str, "mul")) {
    typ = MUL;
  } else if (!strcmp(str, "mulf")) {
    typ = MULF;
  } else if (!strcmp(str, "mulr")) {
    typ = MULR;
  } else if (!strcmp(str, "norm")) {
    typ = NORM;
  } else if (!strcmp(str, "or")) {
    typ = OR;
  } else if (!strcmp(str, "rd")) {
    typ = RD;
  } else if (!strcmp(str, "rmo")) {
    typ = RMO;
  } else if (!strcmp(str, "rsub")) {
    typ = RSUB;
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
  } else if (!strcmp(str, "td")) {
    typ = TD;
  } else if (!strcmp(str, "tio")) {
    typ = TIO;
  } else if (!strcmp(str, "tix")) {
    typ = TIX;
  } else if (!strcmp(str, "tixr")) {
    typ = TIXR;
  } else if (!strcmp(str, "wd")) {
    typ = WD;
  } else if (!strcmp(str, "start")) {
    typ = START;
  } else if (!strcmp(str, "end")) {
    typ = END;
  } else if (!strcmp(str, "byte")) {
    typ = BYTE;
  } else if (!strcmp(str, "word")) {
    typ = WORD;
  } else if (!strcmp(str, "resb")) {
    typ = RESB;
  } else if (!strcmp(str, "resw")) {
    typ = RESW;
  } else if (!strcmp(str, "comma")) {
    typ = COMMA;
  } else {
    token.str = str;
    typ = ID;
  }

  token.type = typ;
  token.location = loc;

  return token;
}
