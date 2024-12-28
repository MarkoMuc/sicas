#ifndef SICAS_PARSER
#define SICAS_PARSER
#include "../includes/parser.h"
#endif

uint8_t parse_regs(TokenVector *tokens, Instruction *instr, size_t *idx) {
  size_t i = *idx;

  if(i >= tokens->count){
    Token *tk =  tokvec_get(tokens, i - 1);
    LOG_PANIC("[%ld:%ld]|[%ld:%ld] Missing first register for instruction of format 2.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col, tk->location.e_row);
  }

  Token *tk = tokvec_get(tokens, i++);

  if (tk->type == REGISTER) {
    tokvec_add(instr->vec, tk);

    if(i >= tokens->count){
      Token *tk =  tokvec_get(tokens, i - 1);
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] Missing separating comma for instruction of format 2.\n",
                tk->location.s_col, tk->location.s_row, tk->location.e_col, tk->location.e_row);
    }

    tk = tokvec_get(tokens, i++);

    if (tk->type == COMMA) {
      tokvec_add(instr->vec, tk);
    } else {
      Token *s_tk = tokvec_get(instr->vec, 0);
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] Instruction of type 2 is missing a separator between a registers.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
    }

    if(i >= tokens->count){
      Token *tk =  tokvec_get(tokens, i - 1);
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] Missing second register for instruction of type 2.\n",
                tk->location.s_col, tk->location.s_row, tk->location.e_col, tk->location.e_row);
    }

    tk = tokvec_get(tokens, i++);

    if (tk->type == REGISTER) {
      tokvec_add(instr->vec, tk);
    } else {
      Token *s_tk = tokvec_get(instr->vec, 0);
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] Instruction of type 2 is missing a register as second argument.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
    }

  } else {
    Token *s_tk = tokvec_get(instr->vec, 0);
    LOG_PANIC("[%ld:%ld]|[%ld:%ld] Instruction of type 2 is missing a register as first argument.\n",
            s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
            tk->location.e_row);
  }

  *idx = i;

  return 0;
}

uint8_t parse_mem_addr(TokenVector *tokens, Instruction *instr, SymTable *sym, size_t *idx, uint8_t float_instr, enum ftype format) {
  size_t i = *idx;
  Token *tk = tokvec_get(tokens, i++);

  token_check_null(tk, "Instruction is missing parameters.\n");

  switch(tk->type) {
    case LITERAL:
      tokvec_add(instr->vec, tk);
      tk = tokvec_get(tokens, i++);

      if (tk->type == FNUM) {
        if (float_instr == 0) {
          Token *s_tk = tokvec_get(instr->vec, 0);
          LOG_PANIC("[%ld:%ld]|[%ld:%ld] Floats not allowed here.\n",
                  s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
                  tk->location.e_row);
        }

        tokvec_add(instr->vec, tk);
      } else if (tk->type == NUM || tk->type == HEX || tk->type == BIN || tk->type == STRING) {
        tokvec_add(instr->vec, tk);
      } else {
        Token *s_tk = tokvec_get(instr->vec, 0);
        LOG_PANIC("[%ld:%ld]|[%ld:%ld] Literal missing numeral.\n",
                s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
                tk->location.e_row);
      }
      break;

    case AT:
        tokvec_add(instr->vec, tk);
        tk = tokvec_get(tokens, i++);

      // TODO:Is only ID allowed after?
      if (tk->type == ID) {
        tokvec_add(instr->vec, tk);
        symtab_add_symbol(sym, tk->str);
      } else {
        Token *s_tk = tokvec_get(instr->vec, 0);
        LOG_PANIC("[%ld:%ld]|[%ld:%ld] Missing identifier after @.\n",
                s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
                tk->location.e_row);
      }
      break;

    case ID:
      tokvec_add(instr->vec, tk);
      symtab_add_symbol(sym, tk->str);
      break;
    default:
      LOG_PANIC("Instruction does not contain a valid address.\n");
  }

  if(i < instr->vec->count){
    tk = tokvec_get(instr->vec, i++);

    if (tk->type == COMMA) {
      tokvec_add(instr->vec, tk);
      tk = tokvec_get(tokens, i++);

      if (tk->type == REGISTER && strcmp(tk->str, "X")) {
        tokvec_add(instr->vec, tk);
      } else {
        Token *s_tk = tokvec_get(instr->vec, 0);
        LOG_PANIC("[%ld:%ld]|[%ld:%ld] Offset should be register X.\n", s_tk->location.s_col,
                s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
      }
    } else {
      i = i - 1;
    }
  }

  *idx = i;
  return 0;
}

size_t builder(TokenVector *tokens, InstrVector *instrs, SymTable *sym, size_t *idx) {
  size_t i = *idx;
  Token *tk = tokvec_get(tokens, i++);
  enum ftype format = THREE;
  enum itype type = MINSTR;

  token_check_null(tk, "Token in main token vector is null.\n");

  if (tk->type == PLUS) {
    format = FOUR;
    tk = tokvec_get(tokens, i++);
  }

  Instruction *instr = instr_create();

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
    tokvec_add(instr->vec, tk);
    //TODO: How do we handle format?
    parse_mem_addr(tokens, instr, sym, &i, 0, format);
    break;

  case ADDF:
  case COMPF:
  case DIVF:
  case LDF:
  case MULF:
  case STF:
  case SUBF:
    tokvec_add(instr->vec, tk);
    parse_mem_addr(tokens, instr, sym, &i, 1, format);
    break;

  case ADDR:
  case COMPR:
  case DIVR:
  case MULR:
  case RMO:
  case SUBR:
    if (format == FOUR) {
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
    }

    format = TWO;
    tokvec_add(instr->vec, tk);
    parse_regs(tokens, instr, &i);
    break;

  case CLEAR:
    LOG_PANIC("Instruction has not been implemented\n");
    if (format == FOUR) {
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
    }
    format = TWO;
    tokvec_add(instr->vec, tk);
    //FIXME: This is a mnemonic so its wrong
    tk = tokvec_get(tokens, i++);
    if (tk->type == REGISTER) {
      tokvec_add(instr->vec, tk);
    } else {
      Token *s_tk = tokvec_get(instr->vec, 0);
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] Argument should be a register.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
    }
    break;

  case HIO:
  case SIO:
  case TIO:
    LOG_PANIC("Instruction has not been implemented\n");
    if (format == FOUR) {
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
    }
    format = ONE;
    tokvec_add(instr->vec, tk);
    break;

  case FIX:
  case FLOAT:
  case NORM:
    LOG_PANIC("Instruction has not been implemented\n");
    if (format == 4) {
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
    }
    format = ONE;
    tokvec_add(instr->vec, tk);
    break;

  case RSUB:
    LOG_PANIC("Instruction has not been implemented\n");
    tokvec_add(instr->vec, tk);
    break;

  case SHIFTL:
  case SHIFTR:
    LOG_PANIC("Instruction has not been implemented\n");
    tokvec_add(instr->vec, tk);
    tk = tokvec_get(tokens, i++);

    if (tk->type == REGISTER) {
    tokvec_add(instr->vec, tk);
    } else {
      Token *s_tk = tokvec_get(instr->vec, 0);
      LOG_PANIC("[%ld:%ld]|[%ld:%ld] Argument 1 is not a register.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
    }

    //FIXME: This should check the following is a numeral
    tk = tokvec_get(tokens, i++);
    tokvec_add(instr->vec, tk);
    break;

  case SVC:
    LOG_PANIC("Instruction has not been implemented\n");
    break;

  case START:
  case END:
    format = ZERO;
    LOG_PANIC("Instruction has not been implemented\n");
    break;

  case BYTE:
  case WORD:
  case RESB:
  case RESW:
    format = ZERO;
    LOG_PANIC("Instruction has not been implemented\n");
    break;

  default:
    token_print(*tk);
    printf("\n");
    LOG_PANIC("This token should not be here alone \n");
  }

  instr->format = format;
  instr->type = type;
  //FIXME: Calc addr
  instr->addr = 0;

  instrvec_add(instrs, instr);
  *idx = i;
  //FIXME: Maybe return instruction
  return i;
}

void parse_vector(TokenVector *vec, InstrVector *instrs, SymTable *sym) {
  long vec_size = vec->count;
  size_t i = 0;
  while(i < vec_size){
    //FIXME: Maybe return error idk something?
    i = builder(vec, instrs, sym, &i);
  }
}

Instruction *instr_create() {
  Instruction *instr = malloc(sizeof(*instr));
  if (!instr) {
    LOG_PANIC("Failed to allocate memory for the instruction.\n");
  }

  instr->vec = malloc(sizeof(*(instr->vec)));
  if(!instr->vec){
    LOG_PANIC("Failed to allocate memory for the token vector during instruction creation.\n");
  }

  tokvec_init(instr->vec);
  instr->type = 0;
  instr->addr = 0;
  instr->format = 0;

  return instr;
}

void instrvec_init(InstrVector *v){
  v->count = 0;
  v->capacity = INSTRVEC_INITIAL_CAPACITY;
  v->items = malloc(sizeof(*(v->items)) * v->capacity);

  if (!v->items) {
    LOG_PANIC("Error during instruction vector init.\n");
  }
}

void instrvec_free(InstrVector *v){
  if (!v) {
    LOG_PANIC("Error while deallocating the instruction vector.\n");
  }

  for(size_t i = 0; i < v->count; i++) {
    Instruction *instr = v->items[i];
    tokvec_free(instr->vec);
  }

  free(v->items);
}

void instrvec_free_destructive(InstrVector *v){
  if (!v) {
    LOG_PANIC("Error while deallocating the instruction vector.\n");
  }

  for(size_t i = 0; i < v->count; i++) {
    Instruction *instr = v->items[i];
    tokvec_free_destructive(instr->vec);
  }

  free(v->items);
}

void instrvec_add(InstrVector *v, Instruction *el){
  if (!v || !el) {
    LOG_PANIC("Error while adding an element to the instruction vector.\n");
  }

  if (v->count >= v->capacity) {
    v->capacity *= INSTRVEC_RESIZE_MULTIPLIER;
    v->items = realloc(v->items, sizeof(*(v->items)) * v->capacity);
    if (!v->items) {
      LOG_PANIC("Error while expanding the instruction vector from %ld to %ld.\n", v->count,
              v->capacity);
    }
  }

  v->items[v->count++] = el;
}

void instrvec_replace(InstrVector *v, Instruction *el, size_t idx){
  if (!v || !el || idx < 0) {
    LOG_PANIC("Error while replacing element in the instruction vector.\n");
  }

  if (idx >= v->count) {
    LOG_PANIC("Error while replacing instruction at index %ld current count is %ld.\n", idx,
            v->count);
  }

  Instruction *old = v->items[idx];
  v->items[idx] = el;
  for (size_t i = idx + 1; i < v->count; i++) {
    Instruction *inter = v->items[i];
    v->items[i] = old;
    old = inter;
  }

  instrvec_add(v, old);
}

void instrvec_rm(InstrVector *v, size_t idx){
  if (!v|| idx < 0) {
    LOG_PANIC("Error while removing an instruction from the vector.\n");
  }

  if (idx >= v->capacity) {
    LOG_PANIC("Error while removing instruction from instruction vector at index %ld capacity is %ld.\n", idx,
            v->capacity);
  }

  int count = v->count;

  for (size_t i = idx + 1; i < count; i++) {
    v->items[i - 1] = v->items[i];
  }

  v->count -= 1;
}

Instruction *instrvec_get(InstrVector *v, size_t idx){
  if (!v || idx < 0) {
    LOG_PANIC("Error while adding an element to the instruction vector.\n");
  }

  if (idx >= v->capacity) {
    LOG_PANIC("Error while getting instruction from index %ld capacity is %ld.\n", idx,
            v->capacity);
  }

  return v->items[idx];
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
    LOG_PANIC("Error during symtable init.\n");
  }

  for(size_t i = 0; i < SYMTABLE_INITIAL_CAPACITY; i++){
    table->map[i].count = 0;
  }
}

void symtab_free(SymTable *table){
  if (!table) {
    LOG_PANIC("Error while deallocating the SymTable.\n");
  }

  free(table->map);
}

void symtab_free_destructive(SymTable *table){
  if (!table) {
    LOG_PANIC("Error while deallocating the SymTable.\n");
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
    LOG_PANIC("SYMTABLE has been completely filled.");
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
    LOG_PANIC("SYMTABLE has been completely filled.");
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
