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

#define SYMTABLE_SIZE (size_t) 256
#define SYMTABLE_INITIAL_CAPACITY (size_t) 256
#define INSTRVEC_INITIAL_CAPACITY TOKVEC_INITIAL_CAPACITY
#define INSTRVEC_RESIZE_MULTIPLIER TOKVEC_RESIZE_MULTIPLIER
#define hash_func djb2_hash

enum itype { MINSTR, DIRECTIVE, LABEL, CEXPR };
enum ftype { ZERO, ONE, TWO, THREE, FOUR };

typedef struct {
  enum itype type;
  enum ftype format;
  int opcode;
  uint64_t addr;
  TokenVector *vec;
} Instruction;

typedef struct {
  size_t count;
  size_t capacity;
  Instruction **items;
} InstrVector;

typedef struct {
  char* symbol;
  uint64_t addr;
} SymValue;

typedef struct {
  size_t count;
  SymValue values[SYMTABLE_SIZE];
} SymMap;

typedef struct {
  size_t count;
  size_t capacity;
  SymMap *map;
} SymTable;

uint8_t parse_regs(TokenVector *tokens, Instruction *instr, size_t *idx);
uint8_t parse_mem_addr(TokenVector *tokens, Instruction *instr, SymTable *sym, size_t *idx, uint8_t float_instr);
size_t builder(TokenVector *tokens, InstrVector *instrs, SymTable *sym, size_t *idx);
void parse_vector(TokenVector *vec, InstrVector *instrs, SymTable *sym);
Instruction* instr_create();

size_t djb2_hash(char* key);

void instrvec_init(InstrVector *v);
void instrvec_free(InstrVector *v);
void instrvec_add(InstrVector *v, Instruction *el);
void instrvec_replace(InstrVector *v, Instruction *el, size_t idx);
void instrvec_rm(InstrVector *v, size_t idx);
Instruction *instrvec_get(InstrVector *v, size_t idx);

void symtab_init(SymTable *table);
void symtab_free(SymTable *table);
void symtab_free_destructive(SymTable *table);
void symtab_add_symbol(SymTable *table, char *symbol);
void symtab_add_addr(SymTable *table, char *symbol, uint64_t addr);
SymValue *symtab_get_symbol(SymTable *table, char *symbol);

#if (defined(PARSER_DEBUG_MODE) && defined(TOKENIZER_DEBUG_MODE)) || defined(DEBUG_MODE)
void instruction_print(Instruction *instr);
void symtab_print(SymTable *table);
#endif
