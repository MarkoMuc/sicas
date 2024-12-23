#include "../includes/sicas.h"

char help[] = "sicas " SICAS_VERSION " the SIX/XE assembler.\n"
              "Usage:\n"
              "\t sicas [file]\n";

int main(int argc, char **argv) {
  if (argc == 1) {
    printf(help);
    exit(1);
  }

  for (int arg = 1; arg < argc; arg++) {
    FILE *f = fopen(argv[arg], "r");
    if (f == NULL) {
      printf("File not found.");
      exit(1);
    }

    TokenVector vec = {0};
    tokvec_init(&vec);

    fill(f, &vec);
    fclose(f);
#if defined(TOKENIZER_DEBUG_MODE) || defined(DEBUG_MODE)
    tokvec_print(&vec);
#endif
    TokenVector sym = {0};
    tokvec_init(&sym);
#if (defined(PARSER_DEBUG_MODE) && defined(TOKENIZER_DEBUG_MODE))|| defined(DEBUG_MODE)
    tokvec_print(&vec);
#endif
    //parse_vector(&vec, &sym);

    tokvec_free(&vec);
    tokvec_free(&sym);
  }

  exit(0);
}
