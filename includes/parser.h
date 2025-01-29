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

#ifndef SICAS_SICSTR
#define SICAS_SICSTR
#include "sicstr.h"
#endif

#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

#ifndef SICAS_TOKENIZER
#define SICAS_TOKENIZER
#include "tokenizer.h"
#endif

#ifndef SICAS_MNEMONIC
#define SICAS_MNEMONIC
#include "mnemonic.h"
#endif

// macros

#define SYMTABLE_SIZE (size_t) 256
#define SYMTABLE_INITIAL_CAPACITY (size_t) 256
#define INSTRVEC_INITIAL_CAPACITY TOKVEC_INITIAL_CAPACITY
#define INSTRVEC_RESIZE_MULTIPLIER TOKVEC_RESIZE_MULTIPLIER
#define hash_func djb2_hash
#define SICAS_BYTE_SIZE 1
#define SICAS_WORD_SIZE SICAS_BYTE_SIZE * 3
#define SICXE_FLOAT_PRECISION 36

#define DIRECT_INSTR(instr) ((MInstr*)((instr)->instr))
#define DIRECT_DIR(instr) ((Directive*)((instr)->instr))
#define DIRECT_IMEM(instr) ((InitMemory*)((instr)->instr))
#define DIRECT_RMEM(instr) ((ResMemory*)((instr)->instr))
#define DIRECT_MEM(instr) ((Mem*)DIRECT_INSTR(instr)->oper)
#define DIRECT_REGS(instr) ((Regs*)DIRECT_INSTR(instr)->oper)

#define IS_DECIMAL(type) ((type) == NUM || (type) == BIN || (type) == HEX)
#define IS_NUMBER(type) (IS_DECIMAL(type) || (type) == FNUM)
#define IS_CONSTANT(type) (IS_NUMBER(type) || (type) == STRING)
#define IS_DCONST(type) (IS_DECIMAL(type) || (type) == STRING)

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
  //FIXME: This field might not be needed
  enum ttype type;
  uint64_t start_addr;
  uint64_t reserved;
  uint64_t raw;
  Token *tk;
} InitMemory;

typedef struct {
  //FIXME: This field might not be needed
  enum ttype type;
  uint64_t start_addr;
  uint64_t reserved;
} ResMemory;

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
  Location loc;
  uint64_t addr;
  void *instr;
  Sicstr str;
} Instruction;

typedef struct {
  size_t count;
  size_t capacity;
  uint64_t first_addr;
  uint64_t start_addr;
  uint64_t end_addr;
  Sicstr prog_name;
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

Regs* parse_regs(const TokenVector *tokens, Instruction *instr, size_t *idx);
Mem* parse_mem_addr(const TokenVector *tokens, Instruction *instr, SymTable *sym, size_t *idx, const uint8_t float_instr);
size_t builder(const TokenVector *tokens, InstrVector *instrs, SymTable *sym, size_t *idx, uint64_t *loc_ctr);
bool parse_vector(const TokenVector *vec, InstrVector *instrs, SymTable *sym);
Instruction* instr_create();

size_t djb2_hash(const char* key);
uint32_t dec_to_float48(const Token *tk, uint64_t *value);
uint64_t long_log2(uint64_t num);
uint64_t long_log10(uint64_t num);
uint64_t pow_of(uint64_t a, uint64_t b);
uint64_t token_to_long(const Token *tk);
uint64_t long_ceil(uint64_t num1, uint64_t div);

// ufunc

void instrvec_init(InstrVector *v);
void instrvec_free(InstrVector *v);
void instrvec_add(InstrVector *v, Instruction *el);
void instrvec_add_at(InstrVector *v, Instruction *el, size_t idx);
void instrvec_rm(InstrVector *v, const size_t idx);
Instruction *instrvec_get(const InstrVector *v, const size_t idx);

void symtab_init(SymTable *table);
void symtab_free(SymTable *table);
void symtab_free_destructive(SymTable *table);
uint8_t symtab_add_symbol(SymTable *table, char *symbol);
void symtab_add_addr(SymTable *table, char *symbol, const uint64_t addr);
SymValue *symtab_get_symbol(const SymTable *table, const char *symbol);
void symtab_rm_symbol(SymTable *table, const char *symbol);
uint64_t symtab_check_get_addr(const SymTable *table, const char *symbol, const Instruction *instr);

// debug

#if defined(PARSER_DEBUG_MODE) || defined(DEBUG_MODE)
void instruction_print(Instruction *instr);
void instrvec_print(InstrVector *instrs);
void symtab_print(SymTable *table);
#endif
