#include "tokenizer.h"

// TODO: ADD hexa(and maybe even octa) numbers
//   - ADD .equ and other possible commands
//   - ADD comments
//   - ADD ignore case
//   - ADD better location information for debug
//   - ADD error message for illegal symbols
//   - ADD how is address offset handled?

Token gen_token(char *str, Location loc) {
  Token token = {};
  enum ttype typ;
  if (strcmp(str, "add") == 0) {
    typ = ADD;
  } else if (strcmp(str, "mul") == 0) {
    typ = MUL;
  } else if (strcmp(str, "lda") == 0) {
    typ = LDA;
  } else if (strcmp(str, "sta") == 0) {
    typ = STA;
  } else {
    token.str = str;
    typ = ID;
  }

  token.type = typ;
  token.location = loc;

  return token;
}

void fill(FILE *f, Vector *vec) {
  int read_c;
  long row = 0;
  long col = 0;
  int index = 0;
  int num = 0;
  int identf = 0;
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
      col++;
      index++;

      if (buffer[i] >= 'a' && buffer[i] <= 'z') {
        if (s_row == -1) {
          s_row = row;
          s_col = col;
          index = 0;
        }
        str[index] = buffer[i];
        identf = 1;
      } else if (identf) {
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

      if (!identf && buffer[i] >= '0' && buffer[i] <= '9') {
        if (s_row == -1) {
          s_row = row;
          s_col = col;
          index = 0;
        }
        str[index] = buffer[i];
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
      if (buffer[i] == ' ') {
        continue;
      } else if (buffer[i] == ',') {
        Token el = {
            .type = COMMA,
            .str = NULL,
            .location = {
                .s_col = col, .s_row = row, .e_col = col, .e_row = row}};
        add_el(vec, &el);
      } else if (buffer[i] == '\n') {
        row += 1;
        col = 0;
      }
    }
  }
  free(str);
  free(buffer);
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
