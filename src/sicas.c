#include "../includes/sicas.h"

char help[] = "sicas " SICAS_VERSION " the SIX/XE assembler.\n"
              "Usage:\n"
              "\t sicas [file]\n"
              "\t -h or --help for this message.\n";

int main(int argc, char **argv) {
  if (argc == 1) {
    printf(help);
    exit(1);
  }

  for (int arg = 1; arg < argc; arg++) {
    if(argv[arg][0] == '-'){
      if(!strcmp(argv[arg], "-h") || !strcmp(argv[arg], "--help")){
        printf(help);
        exit(1);
      }else{
        LOG_ERR("Command %s does not exist.\n",argv[arg]);
        exit(1);
      }
    }


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

    SymTable symbols = {0};
    symtab_init(&symbols);

    InstrVector instrs = {0};
    instrvec_init(&instrs);

#if (defined(PARSER_DEBUG_MODE) && defined(TOKENIZER_DEBUG_MODE))|| defined(DEBUG_MODE)
    tokvec_print(&vec);
#endif
    parse_vector(&vec, &instrs, &symbols);

    tokvec_free(&vec);
    symtab_free(&symbols);
    instrvec_free(&instrs);
  }

  exit(0);
}
