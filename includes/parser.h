#ifndef STD_INT
#define STD_INT
#include <stdint.h>
#endif

#ifndef STD_STRING
#define STD_STRING
#include <string.h>
#endif

#ifndef STD_BOOL
#define STD_BOOL
#include "stdbool.h"
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "tokenizer.h"
#endif

#ifndef SICAS_OPCODE
#define SICAS_OPCODE
#include "opcode.h"
#endif


// macros

#define SYMTABLE_SIZE (size_t) 256
#define SYMTABLE_INITIAL_CAPACITY (size_t) 256
#define INSTRVEC_INITIAL_CAPACITY TOKVEC_INITIAL_CAPACITY
#define INSTRVEC_RESIZE_MULTIPLIER TOKVEC_RESIZE_MULTIPLIER
#define hash_func djb2_hash
#define SICAS_WORD_SIZE 3
#define SICAS_BYTE_SIZE 1
// enums

enum itype { INSTR, DIRECTIVE, RMEM, IMEM, CEXPR };
enum ftype { ZERO = 0 , ONE = 1, TWO = 2 , THREE = 3, FOUR = 4};
enum mtype { SIM, IMM, IND};

// structs

typedef struct {
  uint8_t reg1;
  uint8_t reg2;
} Regs;

typedef struct {
  enum mtype mem_type;
  bool indexed;
  Token *tk;
} Mem;

typedef struct {
  enum ttype directive;
  Token *tk;
} Directive;

typedef struct {
  enum ttype op;
  void *oper;
} MInstr;

typedef struct {
  enum itype type;
  enum ftype format;
  uint64_t addr;
  void *instr;
} Instruction;

typedef struct {
  size_t count;
  size_t capacity;
  uint64_t first_addr;
  uint64_t start_addr;
  uint64_t end_addr;
  Instruction **items;
} InstrVector;

typedef struct {
  char* symbol;
  uint64_t addr;
  uint8_t set;
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

// mfunc

Regs* parse_regs(TokenVector *tokens, Instruction *instr, size_t *idx);
Mem* parse_mem_addr(TokenVector *tokens, Instruction *instr, SymTable *sym, size_t *idx, uint8_t float_instr);
size_t builder(TokenVector *tokens, InstrVector *instrs, SymTable *sym, size_t *idx, uint64_t *loc_ctr);
void parse_vector(TokenVector *vec, InstrVector *instrs, SymTable *sym);
Instruction* instr_create();

size_t djb2_hash(char* key);
uint64_t long_log2(uint64_t num);
uint64_t token_to_long(Token *tk);

// ufunc

void instrvec_init(InstrVector *v);
void instrvec_free(InstrVector *v);
void instrvec_add(InstrVector *v, Instruction *el);
void instrvec_replace(InstrVector *v, Instruction *el, size_t idx);
void instrvec_rm(InstrVector *v, size_t idx);
Instruction *instrvec_get(InstrVector *v, size_t idx);

void symtab_init(SymTable *table);
void symtab_free(SymTable *table);
void symtab_free_destructive(SymTable *table);
uint8_t symtab_add_symbol(SymTable *table, char *symbol);
void symtab_add_addr(SymTable *table, char *symbol, uint64_t addr);
SymValue *symtab_get_symbol(SymTable *table, char *symbol);

// debug

#if (defined(PARSER_DEBUG_MODE) && defined(TOKENIZER_DEBUG_MODE)) || defined(DEBUG_MODE)
void instruction_print(Instruction *instr);
void symtab_print(SymTable *table);
#endif
