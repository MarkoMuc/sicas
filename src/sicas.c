#include <sicas.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("No file\n");
    exit(1);
  }

  for (int arg = 1; arg < argc; arg++) {
    FILE *f = fopen(argv[arg], "r");
    if (f == NULL) {
      printf("File not found.");
      exit(1);
    }

    TokenVector vec = {};
    init(&vec);

    fill(f, &vec);

#ifdef DEBUG_MODE
    printv(&vec);
#endif

    free_vec(&vec);
    fclose(f);
  }

  exit(0);
}
