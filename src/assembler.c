#ifndef SICAS_ASSEMBLER
#define SICAS_ASSEMBLER
#include "../includes/assembler.h"
#endif

void assemble_instructions(InstrVector *instrs, SymTable *sym, FILE *output){
  assemble_header(instrs, sym, output);
 // assemble_body(instrs, sym, output);
  assemble_end(instrs, sym, output);
}

void assemble_header(InstrVector *instrs, SymTable *sym, FILE *output){
  fprintf(output, "H");
  Instruction *instr = instrvec_get(instrs, 0);
  if(instr->type == DIRECTIVE ){
        Directive *d = (Directive *) instr->instr;
      if(d->directive == START){
        fprintf(output,"%-6.6s", d->tk->str);
      }
  }else{
    fprintf(output, "sicas");
  }

  fprintf(output, "%06lx%06lx\n", instrs->start_addr, instrs->end_addr - instrs->start_addr);
}

void assemble_body(InstrVector *instrs, SymTable *sym, FILE *output){
  uint8_t *body = malloc(sizeof(*body) * (ASSEMBLER_BODY_LINE + 1));
  size_t instr_count = instrs->count;
 // size_t b_idx = 0;
  uint64_t start_addr = 0;
  uint32_t pc_reg = 0;
  uint32_t base_reg = 0;
  //bool reserved = false;
  //bool endline = false;

  if(body) {
    LOG_PANIC("Failed to allocate memory for a string.");
  }

  if(instr_count < 1) {
    LOG_XERR("No instructions found.\n");
  }

  size_t i = 0;
  Instruction *instr = instrvec_get(instrs, i++);
  uint8_t byte_rep[ASSEMBLER_INSTR_MAX_SIZE] = {0, 0, 0, 0}; 
  //uint8_t carry_bits = 0;
  start_addr = instr->addr;
  pc_reg = start_addr;

  while(i < instr_count) {
    if(instr->type == DIRECTIVE) {
      //TODO: set base register
      continue;
    }

    if(instr->type == INSTR) {
      MInstr *operation = (MInstr *)instr->instr;
      byte_rep[INSTR_B1] = mnemonic_get_opcode(operation->op);
      pc_reg += instr->format;

      if(instr->format == TWO) {
        Regs *regs = (Regs *)operation->oper;
        byte_rep[INSTR_B2] = regs->reg1 << 4 | regs->reg2;
      }else if(instr->format == THREE){
        Mem *mem = (Mem *)operation->oper;
        uint8_t bit_flags = NI_SICXE_BITS;
        uint32_t sym_addr = 0;

        if(mem->tk->type == ID) {
          SymValue *val = symtab_get_symbol(sym, mem->tk->str);

          if(!val){
            LOG_XERR("Symbol %s has not been defined yet.", mem->tk->str);
          }

          if(val->set == false) {
            LOG_XERR("Symbol %s has not been defined yet.", mem->tk->str);
          }

          sym_addr = val->addr;
        }else {
          sym_addr = token_to_long(mem->tk);
        }

        if(mem->mem_type == IMM) {
          bit_flags = NI_IMMEDIATE_BITS;
        }else if(mem->mem_type == IND) {
          bit_flags = NI_INDIRECT_BITS;
        }

        //TODO: Do i need to switch by 2?
        byte_rep[INSTR_B1] = (byte_rep[INSTR_B1] << 2) | bit_flags;

        bit_flags = (mem->indexed ? INDEX_BIT : BIT_OFF) << 2;
        int32_t diff = pc_reg - sym_addr;

        if((diff < 0 && diff >= -2048) || (diff >= 0 && diff <= 2047)){
          bit_flags = (bit_flags | PC_REL_BITS) << 1; 
        }else {
          if (diff > 0 && diff <= 4095){
            diff = base_reg - sym_addr;
            bit_flags = (bit_flags | BASE_REL_BITS) << 1; 
          } else{
            LOG_XERR("Instruction cannot be used with PC or BASE register addressing.\n");
          }
        }
        bit_flags = (bit_flags | BIT_OFF) << 4;

        // Use the upper 4 bytes and make sure to zero out all other values.
        // FIXME: check if this correctly works with two's complement
        bit_flags = bit_flags | ((diff >> 8) & (0x1 << 4));
        byte_rep[INSTR_B2] = bit_flags;

        // Only use the lower 8 bytes
        diff = (0x1 << 8) & diff;
        byte_rep[INSTR_B3] = diff;

      }else if(instr->format == FOUR){
        //bp == 0
      }

    }

  }

 free(body);
}

void assemble_end(InstrVector *instrs, SymTable *sym, FILE *output){
  fprintf(output, "E%06lx\n", instrs->end_addr);
}
