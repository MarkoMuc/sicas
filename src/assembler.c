#ifndef SICAS_ASSEMBLER
#define SICAS_ASSEMBLER
#include "../includes/assembler.h"
#endif

void assemble_instructions(const InstrVector *instrs, const SymTable *sym, FILE *output) {
  assemble_header(instrs, sym, output);
  assemble_body(instrs, sym, output);
  assemble_end(instrs, output);
}

void assemble_header(const InstrVector *instrs, const SymTable *sym, FILE *output) {
  fprintf(output, "H");
  const Instruction *instr = instrvec_get(instrs, 0);
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

void assemble_body(const InstrVector *instrs, const SymTable *sym, FILE *output) {
  uint8_t *body = malloc(sizeof(*body) * (ASSEMBLER_BODY_LINE + 1));
  const size_t instr_count = instrs->count;
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
         const ResMemory *res = (ResMemory *)instr->instr;
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
      const Directive *d = (Directive *)instr->instr;

      if(d->directive == BASE) {
        const enum ttype type = d->tk->type;
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
      const MInstr *operation = (MInstr *)instr->instr;
      byte_rep[INSTR_B1] = mnemonic_get_opcode(operation->op);
      pc_reg += instr->format;

      if(instr->format == TWO) {
        const Regs *regs = (Regs *)operation->oper;
        byte_rep[INSTR_B2] = regs->reg1 << 4 | regs->reg2;
      } else if (instr->format == THREE || instr->format == FOUR) {
        const Mem *mem = (Mem *)operation->oper;
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

        byte_rep[INSTR_B1] = (byte_rep[INSTR_B1]) | bit_flags;

        // Set indexed bit and shift flag for bp flags
        bit_flags = (mem->indexed ? INDEX_BIT : BIT_OFF) << 2;

        int32_t diff = 0;
        bool format4 = false;

        if(mem->mem_type == IMM && disp_val <= 0xFFF) {
          //FIXME: isn't this equivalent? bit_flags = bit_flags << 2;
          // FIXME: This isnt actually completely correct, and is the same as just shifting. CHeck all this shit out again
          bit_flags = (((bit_flags | 0) << 1) | 0) << 1;
          diff = disp_val;
        } else if(((disp_val - pc_reg) < 0 && (disp_val - pc_reg) >= -2048) ||
                  ((disp_val - pc_reg) >= 0 && (disp_val - pc_reg) <= 2047)) {
          diff = disp_val - pc_reg;
          bit_flags = (bit_flags | PC_REL_BITS) << 1;
        } else if((disp_val - base_reg) >= 0 && (disp_val - base_reg) <= 4095){
          //CHKME: Is this correct displacement calc?
          diff = disp_val - base_reg;
          bit_flags = (bit_flags | BASE_REL_BITS) << 1;
        } else {
          format4 = 4;
        }

        if(format4) {
          if(instr->format != FOUR) {
            LOG_XLERR(instr->loc, instr->loc, "Instruction doesn't fit into a format 3 isntruciton try using format 4.\n");
          }

          if(disp_val > ((uint32_t)0xFFFFF)) {
            LOG_XLERR(instr->loc, instr->loc, "Displacement too large for format four.\n");
          }

          // set to IMM if not set yet
          if(mem->mem_type == SIM) {
            byte_rep[INSTR_B1] = byte_rep[INSTR_B1] | NI_SICXE_BITS;
          }

          // set b/p regs to 0
          bit_flags = (((bit_flags | 0) << 1) | 0) << 1;

          // BP flags are already zeroed out
          bit_flags = bit_flags << 1;

          // Set format field
          bit_flags = (bit_flags | FORMAT_BIT) << 4;

          // CHKME: get values the same way its done in the initiated memory section.
          bit_flags = bit_flags | ((disp_val >> 16) & ((uint8_t)0xFF));
          byte_rep[INSTR_B2] = bit_flags;

          byte_rep[INSTR_B3] = (disp_val >> 8) & ((uint8_t)0xFF);
          byte_rep[INSTR_B4] = disp_val & ((uint8_t)0xFF);
        } else {
          bit_flags = (bit_flags | BIT_OFF) << 4;

          // TODO: check if this correctly works with two's complement.
          bit_flags = bit_flags | ((diff >> 8) & ((uint8_t)0xF));
          byte_rep[INSTR_B2] = bit_flags;

          diff = ((uint8_t)0xFF) & diff;
          byte_rep[INSTR_B3] = diff;
        }
      }

      if(operation->op == RSUB) {
        byte_rep[INSTR_B2] = 0x0;
        byte_rep[INSTR_B3] = 0x0;
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
      const InitMemory *init = (InitMemory *)instr->instr;
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
        const char *str = init->tk->str;
        size_t idx = 0;
        bool special = false;

        while(str[idx] != '\0') {
          uint8_t val = str[idx++];

          if(b_idx >= ASSEMBLER_BODY_LINE) {
            output_text(output, body, &b_idx, &start_addr, pc_reg);
          }

          if(init->tk->type == STRING) {
            if(!special && val == '\\'){
              special = true;
              continue;
            }else if(special){
              val = escapeseq_to_char(val);
              special = false;
            }

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

        if(idx == 0) {
          if(b_idx >= ASSEMBLER_BODY_LINE) {
            output_text(output, body, &b_idx, &start_addr, pc_reg);
          }

          body[b_idx++] = nibble_to_hex(msn('\0'));

          if(b_idx >= ASSEMBLER_BODY_LINE) {
            output_text(output, body, &b_idx, &start_addr, pc_reg);
          }
          body[b_idx++] = nibble_to_hex(msn('\0'));

          pc_reg++;
        }
      } else{
        const uint64_t init_val = token_to_long(init->tk);
        uint64_t bytes = init->raw;

        while(bytes){

          uint8_t val = (uint8_t)(init_val & ((uint64_t)0xFF << (bytes - 1))) >> (bytes - 1);

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
        }
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

void assemble_end(const InstrVector *instrs, FILE *output) {
  fprintf(output, "E%06lx\n", instrs->first_addr);
}

void output_text(FILE *output, uint8_t *body, size_t *b_idx, uint64_t *start_addr, const uint32_t pc_reg) {
  body[*b_idx] = '\0';
  fprintf(output, "T%06lx%02lx%s\n", *start_addr, pc_reg - *start_addr, body);
  *start_addr = pc_reg;
  *b_idx = 0;
}

uint8_t nibble_to_hex(const uint8_t nibble) {
  return nibble + (nibble < 10 ? '0':('A' - 10));
}

uint8_t instr_to_text(uint8_t *body, const uint8_t *array, size_t *b_idx, uint8_t size, uint8_t start) {
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

uint8_t escapeseq_to_char(const uint8_t c){
  switch(c) {
    case '0':
      return '\0';
    case '\\':
      return '\\';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    default:
      LOG_PANIC("Invalid escape sequence, this character should not be here.\n");
  }
}
