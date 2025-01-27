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

  for (size_t arg = 1; arg < argc; arg++) {
    if(argv[arg][0] == '-'){
      if(!strcmp(argv[arg], "-h") || !strcmp(argv[arg], "--help")){
        printf(help);
        exit(1);
      }else{
        LOG_ERR("Command %s does not exist.\n",argv[arg]);
        exit(1);
      }
    }


    struct stat stats;
    if(stat(argv[arg], &stats) == 0) {
      if(stats.st_size == 0){
        break;
      }
    } else{
      LOG_XERR("Failed to open file %s\n", argv[arg]);
    }

    FILE *f = fopen(argv[arg], "r");
    if (!f) {
      LOG_XERR("Failed to open file %s\b", argv[arg]);
    }

    TokenVector vec = {0};
    tokvec_init(&vec);

    bool err_val = fill(f, &vec);
    fclose(f);

    if(err_val) {
      tokvec_free(&vec);
      break;
    }

#if defined(TOKENIZER_DEBUG_MODE) || defined(DEBUG_MODE)
    tokvec_print(&vec);
#endif

    SymTable symbols = {0};
    symtab_init(&symbols);

    InstrVector instrs = {0};
    instrvec_init(&instrs);

    err_val = parse_vector(&vec, &instrs, &symbols);
    if(err_val) {
      tokvec_free(&vec);
      symtab_free(&symbols);
      instrvec_free(&instrs);
      break;
    }

#if defined(PARSER_DEBUG_MODE) || defined(DEBUG_MODE)
     printf("Instructions [%08lx:%08lx][%08lx]:\n", instrs.start_addr, instrs.end_addr, instrs.first_addr);
     for(size_t i = 0; i < instrs.count; i++){
       Instruction *instr = instrvec_get(&instrs, i);
       instruction_print(instr);
     }

     printf("Symbols:\n");
    symtab_print(&symbols);
#endif
    FILE *out;
    char *prog_name;
    if(instrs.prog_name.count){
      size_t name_size = instrs.prog_name.count;
      name_size += 5;

      prog_name = malloc(sizeof(*prog_name) * (name_size));
      if(!prog_name){
        LOG_PANIC("Could not allocate program name.");
      }

      sprintf(prog_name, "%s.obj", sicstr_dump(&instrs.prog_name));
      prog_name[name_size - 1] = '\0';
      out = fopen(prog_name,"w");
    }else{
      LOG_PANIC("Output file has no name.");
    }

    out = fopen(prog_name,"w");
    err_val = assemble_instructions(&instrs, &symbols, out);
    fclose(out);

    if(err_val) {
      if(out != stdout && out != stderr) {
        remove(prog_name);
      }
      LOG_XERR("Error writting to file '%s'.\n", prog_name);
    }

    free(prog_name);
    tokvec_free(&vec);
    symtab_free(&symbols);
    instrvec_free(&instrs);
  }

  exit(0);
}
