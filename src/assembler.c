#ifndef SICAS_ASSEMBLER
#define SICAS_ASSEMBLER
#include "../includes/assembler.h"
#endif

void assemble_header(InstrVector *instrs, SymTable *sym, FILE *output){
  fprintf(output, "H");
  Instruction *instr = instrvec_get(instrs, 0);
  if(instr->type == MINSTR){
    Token *tk = tokvec_get(instr->vec, 1);
    if(tk->type == START){
      tk = tokvec_get(instr->vec, 0);
      fprintf(output,"%-5.5s", tk->str);
    }
  }else{
    fprintf(output, "sicas");
  }

  fprintf(output, "%05lx%05lx\n", instrs->start_addr, instrs->end_addr - instrs->start_addr);
}

void assemble_instructions(InstrVector *instrs, SymTable *sym, FILE *output){
  assemble_header(instrs, sym, output);
}
