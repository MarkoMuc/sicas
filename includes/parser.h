#ifndef STD_INT
#define STD_INT
#include <stdint.h>
#endif

#ifndef STD_STRING
#define STD_STRING
#include <string.h>
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "tokenizer.h"
#endif

enum itype { MINSTR, DIRECTIVE, LABEL };
enum ftype { ZERO, ONE, TWO, THREE, FOUR };

typedef struct {
  enum itype type;
  enum ftype format;
  int opcode;
  uint64_t addr;
  TokenVector *vec;
} Instruction;


uint8_t parse_regs(TokenVector *tokens, TokenVector *instr, long *idx);
uint8_t parse_mem_addr(TokenVector *tokens, TokenVector *instr, long *idx,
                       uint8_t float_type);
long builder(TokenVector *tokens, TokenVector *sym, TokenVector *stack,
             long idx);
void parse_vector(TokenVector *vec, TokenVector *sym);
Instruction* init_instr();

#if defined(PARSER_DEBUG_MODE) || defined(DEBUG_MODE)
void instruction_print(Instruction *instr);
#endif
