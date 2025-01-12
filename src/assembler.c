#ifndef SICAS_ASSEMBLER
#define SICAS_ASSEMBLER
#include "../includes/assembler.h"
#endif

void assemble_instructions(InstrVector *instrs, SymTable *sym, FILE *output) {
  assemble_header(instrs, sym, output);
  assemble_body(instrs, sym, output);
  assemble_end(instrs, sym, output);
}

void assemble_header(InstrVector *instrs, SymTable *sym, FILE *output) {
  fprintf(output, "H");
  Instruction *instr = instrvec_get(instrs, 0);
  if(instr->type == DIRECTIVE) {
    Directive *d = (Directive *)instr->instr;
    if(d->directive == START) {
      fprintf(output, "%-6.6s", d->tk->str);
    }
  } else {
    fprintf(output, "sicas");
  }

  fprintf(output, "%06lx%06lx\n", instrs->start_addr, instrs->end_addr - instrs->start_addr);
}

void assemble_body(InstrVector *instrs, SymTable *sym, FILE *output) {
  uint8_t *body = malloc(sizeof(*body) * (ASSEMBLER_BODY_LINE + 1));
  size_t instr_count = instrs->count;
  size_t b_idx = 0;
  uint64_t start_addr = 0;
  uint32_t pc_reg = 0;
  uint32_t base_reg = 0;
  bool reserved = false;

  if(!body) {
    LOG_PANIC("Failed to allocate memory for a string.\n");
  }

  if(instr_count < 1) {
    LOG_XERR("Input contains no instruction.\n");
  }

  size_t i = 0;
  Instruction *instr = instrvec_get(instrs, i++);
  uint8_t byte_rep[ASSEMBLER_INSTR_MAX_SIZE] = {0, 0, 0, 0};
  uint64_t carry_bits = 0;

  if(!instr){
    LOG_PANIC("Input contains no instructions but it has not been caught yet.\n");
  }

  start_addr = instr->addr;
  pc_reg = start_addr;

  while(true) {

     if(reserved || instr->type == RMEM) {
       if(b_idx != 0) {
         output_text(output, body, &b_idx, &start_addr, pc_reg);
       }

       reserved = true;
       if(instr->type != RMEM) {
         start_addr = instr->addr;
         pc_reg = instr->addr;
         reserved = false;
       }else{
         ResMemory *res = (ResMemory *)instr->instr;
         pc_reg += res->reserved;

         if(i >= instr_count) {
            break;
         }

         instr = instrvec_get(instrs, i++);
         continue;
       }
     }

    if (b_idx >= ASSEMBLER_BODY_LINE) {
      output_text(output, body, &b_idx, &start_addr, pc_reg);
    }

    if(instr->type == DIRECTIVE) {
      Directive *d = (Directive *)instr->instr;

      if(d->directive == BASE) {
        enum ttype type = d->tk->type;
        if(type == ID) {
          base_reg = symtab_check_get_addr(sym, d->tk->str, instr);
        } else {
          base_reg = token_to_long(d->tk);
        }
      }
 
      instr = instrvec_get(instrs, i++);
      continue;
    }

    if(instr->type == INSTR) {
      MInstr *operation = (MInstr *)instr->instr;
      byte_rep[INSTR_B1] = mnemonic_get_opcode(operation->op);
      pc_reg += instr->format;

      if(instr->format == TWO) {
        Regs *regs = (Regs *)operation->oper;
        byte_rep[INSTR_B2] = regs->reg1 << 4 | regs->reg2;
      } else if (instr->format == THREE || instr->format == FOUR) {
        Mem *mem = (Mem *)operation->oper;
        uint8_t bit_flags = NI_SICXE_BITS;
        uint32_t disp_val = 0;

        if(mem->tk->type == ID) {
          disp_val = symtab_check_get_addr(sym, mem->tk->str, instr);
        } else {
          disp_val = token_to_long(mem->tk);
        }

        if(mem->mem_type == IMM) {
          bit_flags = NI_IMMEDIATE_BITS;
        } else if (mem->mem_type == IND) {
          bit_flags = NI_INDIRECT_BITS;
        }

        // TODO: Do i need to switch by 2?
        byte_rep[INSTR_B1] = (byte_rep[INSTR_B1] << 2) | bit_flags;

        // Set indexed bit and shift flag for bp flags
        bit_flags = (mem->indexed ? INDEX_BIT : BIT_OFF) << 2;

        if(instr->format == THREE) {
          int32_t diff = pc_reg - disp_val;

          if((diff < 0 && diff >= -2048) || (diff >= 0 && diff <= 2047)) {
            bit_flags = (bit_flags | PC_REL_BITS) << 1;
          } else {
            if(diff > 0 && diff <= 4095) {
              diff = base_reg - disp_val;
              bit_flags = (bit_flags | BASE_REL_BITS) << 1;
            } else {
              LOG_XLERR(instr->loc, instr->loc, "Instruction cannot be used with PC or BASE register addressing.\n");
            }
          }

          bit_flags = (bit_flags | BIT_OFF) << 4;

          // Use the upper 4 bytes and make sure to zero out all other values.
          // TODO: check if this correctly works with two's complement.
          bit_flags = bit_flags | ((diff >> 8) & ((uint8_t)0xF));
          byte_rep[INSTR_B2] = bit_flags;

          // Only use the lower 8 bytes
          diff = ((uint8_t)0xFF) & diff;
          byte_rep[INSTR_B3] = diff;
        } else {
          // TODO: Make sure how format four works.
          if(disp_val > ((uint32_t)0xFFFFF)) {
            LOG_XLERR(instr->loc, instr->loc, "Displacement too large for format four.\n");
          }

          // BP flags are already zeroed out
          bit_flags = bit_flags << 1;

          // Set format field
          bit_flags = (bit_flags | FORMAT_BIT) << 4;

          bit_flags = bit_flags | ((disp_val >> 16) & ((uint8_t)0xFF));
          byte_rep[INSTR_B2] = bit_flags;

          byte_rep[INSTR_B3] = (disp_val >> 8) & ((uint8_t)0xFF);
          byte_rep[INSTR_B4] = disp_val & ((uint8_t)0xFF);
        }
      }

      carry_bits = instr_to_text(body, byte_rep, &b_idx, instr->format, 0);

      if(carry_bits) {
        output_text(output, body, &b_idx, &start_addr, pc_reg - carry_bits);
        instr_to_text(body, byte_rep, &b_idx, carry_bits,
                      instr->format - carry_bits);
      }
      if(i >= instr_count) {
        break;
      }
      instr = instrvec_get(instrs, i++);
      continue;
    }

    if(instr->type == IMEM) {
      InitMemory *init = (InitMemory *)instr->instr;
      uint64_t append = init->reserved - init->raw;

      while(append--) {
        if (b_idx >= ASSEMBLER_BODY_LINE) {
          output_text(output, body, &b_idx, &start_addr, pc_reg);
        }
        body[b_idx++] = '0';

        if (b_idx >= ASSEMBLER_BODY_LINE) {
          output_text(output, body, &b_idx, &start_addr, pc_reg);
        }
        body[b_idx++] = '0';

        pc_reg++;
      }

      if(init->tk->type == HEX || init->tk->type == STRING) {
        char *str = init->tk->str;
        size_t idx = 0;

        while(str[idx] != '\0') {
          uint8_t val = str[idx++];

          if(b_idx >= ASSEMBLER_BODY_LINE) {
            output_text(output, body, &b_idx, &start_addr, pc_reg);
          }

          if(init->tk->type == STRING) {
            body[b_idx++] = nibble_to_hex(msn(val));

            if(b_idx >= ASSEMBLER_BODY_LINE) {
              output_text(output, body, &b_idx, &start_addr, pc_reg);
            }

            body[b_idx++] = nibble_to_hex(lsn(val));
          } else {
            body[b_idx++] = '0';

            if(b_idx >= ASSEMBLER_BODY_LINE) {
              output_text(output, body, &b_idx, &start_addr, pc_reg);
            }

            body[b_idx++] = val;
          }

          pc_reg++;
        }
      } else{
        uint64_t init_val = token_to_long(init->tk);
        uint64_t bytes = init->raw / 8;

        do{
          uint8_t val = (uint8_t)(init_val & ((uint64_t)0xFF << bytes)) >> bytes;

          if(b_idx >= ASSEMBLER_BODY_LINE) {
            output_text(output, body, &b_idx, &start_addr, pc_reg);
          }

          body[b_idx++] = nibble_to_hex(msn(val));

          if(b_idx >= ASSEMBLER_BODY_LINE) {
            output_text(output, body, &b_idx, &start_addr, pc_reg);
          }

          body[b_idx++] = nibble_to_hex(lsn(val));
          pc_reg++;

          bytes--;
        }while(bytes);
      }

      if(i >= instr_count) {
        break;
      }

      instr = instrvec_get(instrs, i++);
      continue;
    }
  }

  if(b_idx > 0) {
    output_text(output, body, &b_idx, &start_addr, pc_reg);
  }

  free(body);
}

void assemble_end(InstrVector *instrs, SymTable *sym, FILE *output) {
  fprintf(output, "E%06lx\n", instrs->first_addr);
}

void output_text(FILE *output, uint8_t *body, size_t *b_idx, uint64_t *start_addr, uint32_t pc_reg) {
  body[*b_idx] = '\0';
  fprintf(output, "T%06lx%02lx%s\n", *start_addr, pc_reg - *start_addr, body);
  *start_addr = pc_reg;
  *b_idx = 0;
}

uint8_t nibble_to_hex(uint8_t nibble) {
  return nibble + (nibble < 10 ? '0':('A' - 10));
}

uint8_t instr_to_text(uint8_t *body, uint8_t *array, size_t *b_idx, uint8_t size, uint8_t start) {
  size_t i = *b_idx;

  while(size && (i) + 1 < ASSEMBLER_BODY_LINE) {
    body[i++] = nibble_to_hex(msn(array[start]));
    body[i++] = nibble_to_hex(lsn(array[start]));
    start++;
    size--;
  }

  *b_idx = i;
  return size;
}
