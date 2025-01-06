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
#define INSTR_B1 0
#define INSTR_B2 1
#define INSTR_B3 2
#define INSTR_B4 3
#define NI_SIC_BITS (uint8_t) 0x0
#define NI_IMMEDIATE_BITS (uint8_t) 0x1
#define NI_INDIRECT_BITS (uint8_t) 0x2
#define NI_SICXE_BITS (uint8_t) 0x3

#define INDEX_BIT (uint8_t) 0x1
#define BASE_REL_BITS (uint8_t) 0x2
#define PC_REL_BITS (uint8_t) 0x1
#define FORMAT_BIT (uint8_t) 0x1
#define BIT_OFF (uint8_t) 0x0
#define BIT_ON (uint8_t) 0x1

// mfunc

void assemble_instructions(InstrVector *instrs, SymTable *sym, FILE *output);
void assemble_header(InstrVector *instrs, SymTable *sym, FILE *output);
void assemble_body(InstrVector *instrs, SymTable *sym, FILE *output);
void assemble_end(InstrVector *instrs, SymTable *sym, FILE *output);
