#ifndef STD_BOOL
#define STD_BOOL
#include "stdbool.h"
#endif

#ifndef SICAS_SICSTR
#define SICAS_SICSTR
#include "sicstr.h"
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

#define msn(byte) (uint8_t)((byte) >> 4)
#define lsn(byte) (uint8_t)((byte) & ((uint8_t) 0x0F))

// mfunc

bool assemble_instructions(const InstrVector *instrs, const SymTable *sym, FILE *output);
bool assemble_header(const InstrVector *instrs, const SymTable *sym, FILE *output);
bool assemble_body(const InstrVector *instrs, const SymTable *sym, FILE *output);
bool assemble_end(const InstrVector *instrs, FILE *output);
uint8_t instr_to_text(uint8_t *body, const uint8_t *array, uint64_t *b_idx, uint8_t size, uint8_t start);
uint8_t nibble_to_hex(const uint8_t nibble);
bool output_text(FILE* output, uint8_t *body, size_t *b_idx, uint64_t *start_addr, const uint32_t pc_reg);
uint8_t escapeseq_to_char(const uint8_t c);
