#ifndef SICAS_ASSEMBLER
#define SICAS_ASSEMBLER
#include "../includes/assembler.h"
#endif

void assemble_instructions(InstrVector *instrs, SymTable *sym, FILE *output){
  assemble_header(instrs, sym, output);
  assemble_body(instrs, sym, output);
  assemble_end(instrs, sym, output);
}

void assemble_header(InstrVector *instrs, SymTable *sym, FILE *output){
  fprintf(output, "H");
  Instruction *instr = instrvec_get(instrs, 0);
  if(instr->type == DIRECTIVE && tokvec_get(instr->vec, 1)->type == START){
      Token *tk = tokvec_get(instr->vec, 0);
      fprintf(output,"%-6.6s", tk->str);
  }else{
    fprintf(output, "sicas");
  }

  fprintf(output, "%06lx%06lx\n", instrs->start_addr, instrs->end_addr - instrs->start_addr);
}

void assemble_body(InstrVector *instrs, SymTable *sym, FILE *output){
  char *body = malloc(sizeof(*body) * (ASSEMBLER_BODY_LINE + 1));
  size_t instr_count = instrs->count;
  size_t b_idx = 0;
  uint64_t start_addr = 0;
  uint64_t new_addr = 0;
  bool reserved = false;

  if(body) {
    LOG_PANIC("Failed to allocate memory for a string.");
  }

  for(size_t i = 0; i < instr_count; i++){
    Instruction *instr = instrvec_get(instrs, i);

    if(instr->type == DIRECTIVE){
      continue;
    }else if (instr->type == RMEM) {
      body[b_idx] = '\0';
      new_addr += calc_offset();
      reserved = true;
      continue;
    }else if(instr->type == IMEM){

    }else if(instr->type == INSTR){
      Token *tk = tokvec_get(instr->vec, 0);
      uint8_t opcode = mnemonic_get_opcode(tk->type);

      if(instr->format == TWO) {
        sprintf(body + b_idx, "%02xl", opcode);
        b_idx = b_idx + 2;

        tk = tokvec_get(instr->vec, 1);
        uint8_t reg1 = mnemonic_get_reg(tk->str);

        tk = tokvec_get(instr->vec, 2);
        uint8_t reg2 = mnemonic_get_reg(tk->str);

        sprintf(body + b_idx, "%01x%01x", reg1, reg2);
        b_idx = b_idx + 2;
      }else if(instr->format == THREE){
        opcode = opcode | 0b11;
        sprintf(body + b_idx, "%02xl", opcode);
        b_idx = b_idx + 2;

      }else if(instr->format == FOUR){

      }

      new_addr = new_addr + instr->format;
    }

    if(reserved || b_idx == ASSEMBLER_BODY_LINE) {
      body[b_idx] = '\0';
      fprintf(output, "T%06lx%02lx%s\n", start_addr, new_addr - start_addr, body);
      b_idx = 0;
      start_addr = new_addr;
      reserved = false;
    }

  }

  free(body);
}

void assemble_end(InstrVector *instrs, SymTable *sym, FILE *output){
  fprintf(output, "E%06lx", instrs->end_addr);
}
