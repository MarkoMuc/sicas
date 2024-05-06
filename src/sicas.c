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

    TokenVector vec = {};
    tokvec_init(&vec);

    fill(f, &vec);

#ifdef DEBUG_MODE
    tokvec_print(&vec);
#endif

    tokvec_free(&vec);
    fclose(f);
  }

  exit(0);
}
