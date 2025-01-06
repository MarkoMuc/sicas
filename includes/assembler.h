#ifndef STD_BOOL
#define STD_BOOL
#include "stdbool.h"
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

#ifndef SICAS_PARSER
#define SICAS_PARSER
#include "parser.h"
#endif

#ifndef SICAS_MNEMONIC
#define SICAS_MNEMONIC
#include "mnemonic.h"
#endif

// macros

#define ASSEMBLER_BODY_LINE 60
#define ASSEMBLER_INSTR_MAX_SIZE 4
#define INSTR_B1 1
#define INSTR_B2 2
#define INSTR_B3 3
#define INSTR_B4 4
#define NI_SIC_BITS 0x0
#define NI_IMMEDIATE_BITS 0x1
#define NI_INDIRECT_BITS 0x2
#define NI_SICXE_BITS 0x3

#define INDEX_BIT 0x1
#define BASE_REL_BITS 0x2
#define PC_REL_BITS 0x1
#define FORMAT_BIT 0x1
#define BIT_OFF 0x0

// mfunc

void assemble_instructions(InstrVector *instrs, SymTable *sym, FILE *output);
void assemble_header(InstrVector *instrs, SymTable *sym, FILE *output);
void assemble_body(InstrVector *instrs, SymTable *sym, FILE *output);
void assemble_end(InstrVector *instrs, SymTable *sym, FILE *output);
