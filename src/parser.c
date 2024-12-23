#ifndef SICAS_PARSER
#define SICAS_PARSER
#include "../includes/parser.h"
#include <stdio.h>
#endif

uint8_t parse_regs(TokenVector *tokens, TokenVector *new, long *idx) {
  Token *tk = tokvec_get(tokens, *idx);
  *idx = *idx + 1;

  if (tk->type == REGISTER) {
    tokvec_add(new, tk);
    tk = tokvec_get(tokens, *idx);
    *idx = *idx + 1;

    if (tk->type == COMMA) {
      tokvec_add(new, tk);
      tk = tokvec_get(tokens, *idx);
      *idx = *idx + 1;
    } else {
      Token *s_tk = tokvec_get(new, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing second argument.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      return 1;
    }

    if (tk->type == REGISTER) {
      tokvec_add(new, tk);
    } else {
      Token *s_tk = tokvec_get(new, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Argument 2 is not a register.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      return 1;
    }

  } else {
    Token *s_tk = tokvec_get(new, 0);
    LOG_ERR("[%ld:%ld]|[%ld:%ld] Argument 1 is not a register.\n",
            s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
            tk->location.e_row);
    return 1;
  }

  return 0;
}

uint8_t parse_mem_addr(TokenVector *tokens, TokenVector *new, long *idx,
                       uint8_t float_type) {
  Token *tk = tokvec_get(tokens, *idx);
  *idx = *idx + 1;

  token_check_null(tk, "Instruction is missing parameters.\n");

  if (tk->type == LITERAL) {
    tokvec_add(new, tk);
    tk = tokvec_get(tokens, *idx);
    *idx = *idx + 1;

    if (tk->type == FNUM) {
      if (float_type == 0) {
        Token *s_tk = tokvec_get(new, 0);
        LOG_ERR("[%ld:%ld]|[%ld:%ld] Floats not allowed here.\n",
                s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
                tk->location.e_row);
        return 1;
      }

      tokvec_add(new, tk);
    } else if (tk->type == NUM || tk->type == HEX || tk->type == BIN) {
      tokvec_add(new, tk);
    } else {
      Token *s_tk = tokvec_get(new, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Literal missing numeral.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      return 1;
    }

    return 0;
  }

  if (tk->type == AT) {
    tokvec_add(new, tk);
    tk = tokvec_get(tokens, *idx);
    *idx = *idx + 1;

    // TODO:Is only ID allowed after?
    if (tk->type == ID) {
      tokvec_add(new, tk);
    } else {
      Token *s_tk = tokvec_get(new, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing identifier after @.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      return 1;
    }

    // TODO: Comma after this allowed?
    return 0;
  }

  if (tk->type == ID) {
    tokvec_add(new, tk);
    *idx = *idx + 1;
    tk = tokvec_get(tokens, *idx);

    if (tk->type == COMMA) {
      tokvec_add(new, tk);
      tk = tokvec_get(tokens, *idx);
      *idx = *idx + 1;

      if (tk->type == REGISTER && strcmp(tk->str, "X")) {
        tokvec_add(new, tk);
      } else {
        Token *s_tk = tokvec_get(new, 0);
        LOG_ERR("[%ld:%ld]|[%ld:%ld] Offset should be X.\n", s_tk->location.s_col,
                s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
        return 1;
      }
    } else {
      *idx = *idx - 1;
    }
  }

  return 0;
}

long builder(TokenVector *tokens, TokenVector *sym, TokenVector *stack,
             long idx) {
  Token *tk = tokvec_get(tokens, idx);
  uint8_t format = 0;

  token_check_null(tk, "Token in main token vector is null.\n");

  if (tk->type == PLUS) {
    format = 4;
  }

  // FIXME: Is this correct or do I need to initilase it with malloc?
  TokenVector *vc = {0};
  tokvec_init(vc);

  switch (tk->type) {
  case ADD:
  case AND:
  case COMP:
  case DIV:
  case J:
  case JEQ:
  case JGT:
  case JLT:
  case JSUB:
  case LDA:
  case LDB:
  case LDCH:
  case LDL:
  case LDS:
  case LDT:
  case LDX:
  case LPS:
  case MUL:
  case OR:
  case RD:
  case SSK:
  case STA:
  case STB:
  case STCH:
  case STL:
  case STS:
  case STSW:
  case STT:
  case STX:
  case SUB:
  case TD:
  case TIX:
  case WD:
    if (format != 4) {
      format = 3;
    }
    if (parse_mem_addr(tokens, vc, &idx, 0)) {
      exit(1);
    }
    break;

  case ADDF:
  case COMPF:
  case DIVF:
  case LDF:
  case MULF:
  case STF:
  case SUBF:
    if (format != 4) {
      format = 3;
    }
    if (parse_mem_addr(tokens, vc, &idx, 1)) {
      exit(1);
    }
    break;

  case ADDR:
  case COMPR:
  case DIVR:
  case MULR:
  case RMO:
  case SUBR:
    if (format == 4) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }
    format = 2;
    if (parse_regs(tokens, vc, &idx)) {
      exit(1);
    }
    break;

  case CLEAR:
    if (format == 4) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }
    format = 2;

    tk = tokvec_get(tokens, idx);
    if (tk->type == REGISTER) {
      tokvec_add(vc, tk);
    } else {
      Token *s_tk = tokvec_get(vc, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Argument should be a register.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      return 1;
    }
    break;

  case HIO:
  case SIO:
  case TIO:
    if (format == 4) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }
    format = 1;
    tokvec_add(vc, tk);
    tk = tokvec_get(tokens, idx);
    break;

  case FIX:
  case FLOAT:
  case NORM:
    if (format == 4) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }
    format = 1;
    tokvec_add(vc, tk);
    break;

  case RSUB:
    tokvec_add(vc, tk);

    break;

  case SHIFTL:
  case SHIFTR:
    tokvec_add(vc, tk);
    idx++;

    tk = tokvec_get(tokens, idx);
    idx++;
    if (tk->type == REGISTER) {
      tokvec_add(vc, tk);
    } else {
      Token *s_tk = tokvec_get(vc, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Argument 1 is not a register.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    tk = tokvec_get(tokens, idx);
    tokvec_add(vc, tk);

    break;

  case SVC:
    tokvec_add(vc, tk);
    idx++;

    tk = tokvec_get(tokens, idx);
    tokvec_add(vc, tk);
    break;

  case START:
  case END:
    // TODO
    break;

  case BYTE:
  case WORD:
  case RESB:
  case RESW:
    // GO until hitting something that is not a number
    break;

  default:
    LOG_ERR("This token should not be here alone %ld\n", idx);
    exit(1);
  }

  return idx;
}

void parse_vector(TokenVector *vec, TokenVector *sym) {
  long vec_size = vec->count;
  TokenVector stack = {0};

  for (long i = 0; i < vec_size; i++) {
    i = builder(vec, sym, &stack, i);
  }
}

Instruction *init_instr() {
  Instruction *instr = malloc(sizeof(*instr));
  if (!instr) {
    LOG_ERR("Failed to allocate memory for the instruction.");
  }

  return instr;
}

void instrvec_init(InstrVector *v){
  v->count = 0;
  v->capacity = INSTRVEC_INITIAL_CAPACITY;
  v->items = malloc(sizeof(*v->items) * v->capacity);

  if (!v->items) {
    LOG_ERR("Error during instruction vector init.\n");
    exit(1);
  }
}

void instrvec_free(InstrVector *v){
  if (!v) {
    LOG_ERR("Error while deallocating the instruction vector.\n");
    exit(1);
  }

  for(size_t i = 0; i < v->count; i++) {
    Instruction instr = v->items[i];
    tokvec_free(instr.vec);
  }

  free(v->items);
}

void instrvec_free_destructive(InstrVector *v){
  if (!v) {
    LOG_ERR("Error while deallocating the instruction vector.\n");
    exit(1);
  }

  for(size_t i = 0; i < v->count; i++) {
    Instruction instr = v->items[i];
    tokvec_free_destructive(instr.vec);
  }

  free(v->items);
}

void instrvec_add(InstrVector *v, Instruction *el){
  if (!v || !el) {
    LOG_ERR("Error while adding an element to the instruction vector.\n");
    exit(1);
  }

  if (v->count >= v->capacity) {
    v->capacity *= INSTRVEC_RESIZE_MULTIPLIER;
    v->items = realloc(v->items, sizeof(*v->items) * v->capacity);
    if (!v->items) {
      LOG_ERR("Error while expanding the instruction vector from %ld to %ld.\n", v->count,
              v->capacity);
      exit(1);
    }
  }

  v->items[v->count++] = *el;
}

void instrvec_add_at(InstrVector *v, Instruction *el, size_t idx){
  if (!v || !el || idx < 0) {
    LOG_ERR("Error while adding an element to the instruction vector.\n");
    exit(1);
  }

  if (idx >= v->capacity) {
    LOG_ERR("Error while adding instruction at index %ld capacity is %ld.\n", idx,
            v->capacity);
    exit(1);
  }

  if(idx >= v->count) {
    v->count++;
    v->items[idx] = *el;
  }else{
    Instruction old = v->items[idx];
    v->items[idx] = *el;
    for (size_t i = idx + 1; i < v->count; i++) {
      Instruction inter = v->items[i];
      v->items[i] = old;
      old = inter;
    }
    instrvec_add(v, &old);
  }
}

void instrvec_rm(InstrVector *v, size_t idx){
  if (!v|| idx < 0) {
    LOG_ERR("Error while removing an instruction to the vector.\n");
    exit(1);
  }

  if (idx >= v->capacity) {
    LOG_ERR("Error while removing instruction from index %ld capacity is %ld.\n", idx,
            v->capacity);
    exit(1);
  }

  int count = v->count;

  for (size_t i = idx + 1; i < count; i++) {
    v->items[i - 1] = v->items[i];
  }

  v->count -= 1;
}

Instruction *instrvec_get(InstrVector *v, size_t idx){
  if (!v || idx < 0) {
    LOG_ERR("Error while adding an element to the instruction vector.\n");
    exit(1);
  }

  if (idx >= v->capacity) {
    LOG_ERR("Error while getting instruction from index %ld capacity is %ld.\n", idx,
            v->capacity);
    exit(1);
  }

  return &(v->items[idx]);
}

size_t djb2_hash(char* str){
  size_t hash = 5381;
  char c;

  while ((c = *str++)){
      hash = ((hash << 5) + hash) + c;
  }

  return hash % SYMTABLE_INITIAL_CAPACITY;
}

void symtab_init(SymTable *table){
  table->count = 0;
  table->capacity = SYMTABLE_INITIAL_CAPACITY;
  table->map = malloc(sizeof(*(table->map)) * table->capacity);

  if(!table->map){
    LOG_ERR("Error during symtable init.\n");
    exit(1);
  }

  for(size_t i = 0; i < SYMTABLE_INITIAL_CAPACITY; i++){
    table->map[i].count = 0;
  }
}

void symtab_free(SymTable *table){
  if (!table) {
    LOG_ERR("Error while deallocating the SymTable.\n");
    exit(1);
  }

  free(table->map);
}

void symtab_free_destructive(SymTable *table){
  if (!table) {
    LOG_ERR("Error while deallocating the SymTable.\n");
    exit(1);
  }

  for(size_t i = 0; i < SYMTABLE_INITIAL_CAPACITY; i++){
    size_t count = table->map[i].count;
    for(size_t j = 0; j < count; j++){
      free(table->map[i].values[j].symbol);
    }
  }

  free(table->map);
}

void symtab_add_symbol(SymTable *table, char *symbol){
  size_t key = hash_key(symbol);
  size_t count = table->map[key].count;

  for(size_t i = 0; i < count; i++){
    if(strcmp(table->map[key].values[i].symbol, symbol) == 0){
      return;
    }
  }

  table->map[key].values[count].symbol = symbol;
  table->map[key].values[count].addr = 0;
  table->map[key].count++;
  table->count++;

  if(table->map[key].count >= SYMTABLE_SIZE){
    LOG_ERR("SYMTABLE has been completely filled.");
    exit(1);
  }
}

void symtab_add_addr(SymTable *table, char *symbol, uint64_t addr){
  size_t key = hash_key(symbol);
  size_t count = table->map[key].count;

  for(size_t i = 0; i < count; i++){
    if(strcmp(table->map[key].values[i].symbol, symbol) == 0){
      table->map[key].values[i].addr = addr;
      return;
    }
  }

  table->map[key].values[count].symbol = symbol;
  table->map[key].values[count].addr = addr;
  table->map[key].count++;
  table->count++;

  if(table->map[key].count >= SYMTABLE_SIZE){
    LOG_ERR("SYMTABLE has been completely filled.");
    exit(1);
  }
}

SymValue *symtab_get_symbol(SymTable *table, char *symbol){
  size_t key = hash_key(symbol);
  size_t count = table->map[key].count;

  for(size_t i = 0; i < count; i++){
    if(strcmp(table->map[key].values[i].symbol, symbol) == 0){
      return &(table->map[key].values[i]);
    }
  }

  return NULL;
}

#if (defined(PARSER_DEBUG_MODE) && defined(TOKENIZER_DEBUG_MODE)) || defined(DEBUG_MODE)
void instruction_print(Instruction *instr) {
  switch(instr->type) {
    case MINSTR:
      printf("itype: MINSTR, ");
      break;
    case DIRECTIVE:
      printf("itype: DIRECTIVE, ");
      break;
    case LABEL:
      printf("itype: LABEL, ");
      break;
    default:
      printf("No itype, ");
  }

  switch(instr->format) {
    case ZERO:
      printf("ftype: ZERO, ");
      break;
    case ONE:
      printf("ftype: ONE, ");
      break;
    case TWO:
      printf("ftype: TWO, ");
      break;
    case THREE:
      printf("ftype: THREE, ");
      break;
    case FOUR:
      printf("ftype: FOUR, ");
      break;
    default:
      printf("no ftype, ");
  }
  printf("opcode:%d, ", instr->opcode);
  printf("addr:%ld, ", instr->addr);
  printf("(");
  for (size_t i = 0; i < instr->vec->count; i++) {
    if(i > 0) {
      printf(", ");
    }
    Token t = instr->vec->items[i];
    token_print(t);
  }
  printf(")\n");
}

void symtab_print(SymTable *table) {
  printf("Symtable[%ld|%ld]:\n", table->count, table->capacity);
  for(size_t i = 0; i < SYMTABLE_INITIAL_CAPACITY; i++){
    size_t count = table->map[i].count;
    if(count == 0){
      continue;
    }
    printf("key: %ld, values: ", i);
    for(size_t j = 0; j < count; j++){
      if(j > 0){
        printf(", ");
      }
      printf("(%s, %ld)",table->map[i].values[j].symbol,  table->map[i].values[j].addr);
    }
    printf("\n");
  }
}
#endif
