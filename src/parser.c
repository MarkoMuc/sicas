#ifndef SICAS_PARSER
#define SICAS_PARSER
#include "../includes/parser.h"
#endif

uint8_t parse_regs(TokenVector *tokens, Instruction *instr, size_t *idx) {
  size_t i = *idx;

  check_next_token(i, tokens, "Missing first register for instruction of format 2.\n");
  Token *tk = tokvec_get(tokens, i++);

  if (tk->type == REGISTER) {
    tokvec_add(instr->vec, tk);

    check_next_token(i, tokens, "Missing separating comma for instruction of format 2.\n");
    tk = tokvec_get(tokens, i++);

    if (tk->type == COMMA) {
      tokvec_add(instr->vec, tk);
    } else {
      Token *s_tk = tokvec_get(instr->vec, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Instruction of type 2 is missing a separator between a registers.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    check_next_token(i, tokens, "Missing second register for instruction of type 2.\n");
    tk = tokvec_get(tokens, i++);

    if (tk->type == REGISTER) {
      tokvec_add(instr->vec, tk);
    } else {
      Token *s_tk = tokvec_get(instr->vec, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Instruction of type 2 is missing a register as second argument.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

  } else {
    Token *s_tk = tokvec_get(instr->vec, 0);
    LOG_ERR("[%ld:%ld]|[%ld:%ld] Instruction of type 2 is missing a register as first argument.\n",
            s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
            tk->location.e_row);
    exit(1);
  }

  *idx = i;

  return 0;
}

uint8_t parse_mem_addr(TokenVector *tokens, Instruction *instr, SymTable *sym, size_t *idx, uint8_t float_instr) {
  size_t i = *idx;
  uint8_t indexing_illegal = 0;
  check_next_token(i, tokens, "Instruction has no address operand.\n");
  Token *tk = tokvec_get(tokens, i++);

  token_check_null(tk);

  switch(tk->type) {
    case LITERAL:
    literal_label:
      LOG_PANIC("LITERALS not implemented yet.\n");
      tokvec_add(instr->vec, tk);

      check_next_token(i, tokens, "No constant following a literal.\n");
      tk = tokvec_get(tokens, i++);
      token_check_null(tk);

      if (tk->type == FNUM) {
        if (float_instr == 0) {
          Token *s_tk = tokvec_get(instr->vec, 0);
          token_check_null(s_tk);
          LOG_ERR("[%ld:%ld]|[%ld:%ld] Floats not allowed here.\n",
                  s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
                  tk->location.e_row);
          exit(1);
        }

        tokvec_add(instr->vec, tk);
      } else if (tk->type == NUM || tk->type == HEX || tk->type == BIN || tk->type == STRING) {
        tokvec_add(instr->vec, tk);
      } else {
        Token *s_tk = tokvec_get(instr->vec, 0);
        token_check_null(s_tk);
        LOG_ERR("[%ld:%ld]|[%ld:%ld] Literal missing constant.\n",
                s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
                tk->location.e_row);
        exit(1);
      }
      break;

    case IMMEDIATE:
    case AT:
      tokvec_add(instr->vec, tk);

      check_next_token(i, tokens, "Missing operand for indirect or immediate addressing.\n");
      tk = tokvec_get(tokens, i++);
      token_check_null(tk);

      indexing_illegal = 1;
      if (tk->type == ID) {
        tokvec_add(instr->vec, tk);
        symtab_add_symbol(sym, tk->str);
      } else if(tk->type == NUM || tk->type == HEX || tk->type == BIN || tk->type == STRING){
        tokvec_add(instr->vec, tk);
      } else if(tk->type == LITERAL){
        goto literal_label;
      } else {
        Token *s_tk = tokvec_get(instr->vec, 0);
        token_check_null(s_tk);
        LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing identifier or constant after indirect or immediate addressing.\n",
                s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
                tk->location.e_row);
        exit(1);
      }
      break;

    case ID:
      tokvec_add(instr->vec, tk);
      symtab_add_symbol(sym, tk->str);
      break;

    case NUM:
    case HEX:
    case BIN:
    case STRING:
    case FNUM:
        if(tk->type == FNUM && float_instr){
          LOG_ERR("[%ld:%ld]|[%ld:%ld] Float constant not allowed here.\n",
                tk->location.s_col, tk->location.s_row, tk->location.e_col, tk->location.e_row);
          exit(1);
        }

        tokvec_add(instr->vec, tk);
    break;

    default:
      LOG_ERR("Instruction does not contain a valid address.\n");
      exit(1);
  }

  if(i < instr->vec->count){
    tk = tokvec_get(instr->vec, i++);
    token_check_null(tk);

    if (tk->type == COMMA) {
      if(indexing_illegal){
        Token *s_tk = tokvec_get(instr->vec, 0);
        token_check_null(s_tk);
        LOG_ERR("[%ld:%ld]|[%ld:%ld] Immediate and indirect addressing cannot be paired with indexed addressing.\n", s_tk->location.s_col,
                s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
        exit(1);
      }

      tokvec_add(instr->vec, tk);

      check_next_token(i, tokens, "Indexed addressing is missing X after the comma.\n");
      tk = tokvec_get(tokens, i++);

      if (tk->type == REGISTER && strcmp(tk->str, "X")) {
        tokvec_add(instr->vec, tk);
      } else {
        Token *s_tk = tokvec_get(instr->vec, 0);
        token_check_null(s_tk);
        LOG_ERR("[%ld:%ld]|[%ld:%ld] Offset should be register X.\n", s_tk->location.s_col,
                s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
        exit(1);
      }
    } else {
      i = i - 1;
    }
  }

  *idx = i;
  return 0;
}

size_t builder(TokenVector *tokens, InstrVector *instrs, SymTable *sym, size_t *idx, uint64_t *loc_ctr) {
  size_t i = *idx;
  Token *id = NULL;
  uint64_t offset = 0;
  Token *tk = tokvec_get(tokens, i++);
  enum ftype format = THREE;
  enum itype type = MINSTR;

  token_check_null(tk);

  if (tk->type == PLUS) {
    format = FOUR;
    check_next_token(i, tokens, "Missing token after +.");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);
  }

  //FIXME: Program name can overlap with a label
  if(tk->type == ID){
    id = tk;
    if(symtab_add_symbol(sym, tk->str)) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Symbol %s has multiple definitions.\n", tk->location.s_col,
              tk->location.s_row, tk->location.e_col, tk->location.e_row, tk->str);
      exit(1);
    }

    check_next_token(i, tokens, "Missing token after +.");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);
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
    parse_mem_addr(tokens, instr, sym, &i, 0);
    offset = format;
    break;

  case ADDF:
  case COMPF:
  case DIVF:
  case LDF:
  case MULF:
  case STF:
  case SUBF:
    tokvec_add(instr->vec, tk);
    parse_mem_addr(tokens, instr, sym, &i, 1);
    offset = format;
    break;

  case ADDR:
  case COMPR:
  case DIVR:
  case MULR:
  case RMO:
  case SUBR:
    if (format == FOUR) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    format = TWO;
    tokvec_add(instr->vec, tk);
    parse_regs(tokens, instr, &i);
    offset = format;
    break;

  case CLEAR:
  case TIXR:
    if (format == FOUR) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    format = TWO;
    tokvec_add(instr->vec, tk);

    check_next_token(i, tokens, "Instruction is missing a register operand.");
    tk = tokvec_get(tokens, i++);

    if (tk->type == REGISTER) {
      tokvec_add(instr->vec, tk);
    } else {
      Token *s_tk = tokvec_get(instr->vec, 0);
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Operand should be a register.\n",
              s_tk->location.s_col, s_tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }
    offset = format;
    break;

  case HIO:
  case SIO:
  case TIO:
  case FIX:
  case FLOAT:
  case NORM:
  case RSUB:
    if (format == FOUR) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] This instruction cannot be in format 4.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    format = ONE;
    tokvec_add(instr->vec, tk);
    offset = format;
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

    tk = tokvec_get(tokens, i++);
    tokvec_add(instr->vec, tk);
    offset = format;
    break;

  case SVC:
    LOG_PANIC("Instruction has not been implemented\n");
    break;

  case START:
    format = ZERO;

    if(*loc_ctr != 0) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Duplicate START directive or START is not the first instruction.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    if(id == NULL || id->type != ID) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing program name before START directive.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    tokvec_add(instr->vec, id);
    tokvec_add(instr->vec, tk);

    check_next_token(i, tokens, "Missing value after START directive.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);
    tokvec_add(instr->vec, tk);

    if(tk->type == NUM) {
        *loc_ctr = strtol(tk->str, NULL, 0);
    } else if(tk->type == HEX){
        *loc_ctr = strtol(tk->str, NULL, 16);
    } else if (tk->type == BIN){
        *loc_ctr = strtol(tk->str, NULL, 2);
    }else{
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing value after START directive or the value is not a constant.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    instrs->start_addr = *loc_ctr;
    offset = 0;
    //TODO: Save the program name
    break;

  case END:
    format = ZERO;
    tokvec_add(instr->vec, tk);

    check_next_token(i, tokens, "Missing value after END directive.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if (tk->type == NUM || tk->type == HEX || tk->type == BIN) {
      //TODO: resolve and save this value
    } else {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing value after END directive or the value is not a constant/symbol.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    offset = 0;
    break;

  case BYTE:
  case WORD:
  case RESB:
  case RESW:
    format = ZERO;

    if(id == NULL || id->type != ID) {
      LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing program label before RESB or RESW directive.\n",
              tk->location.s_col, tk->location.s_row, tk->location.e_col,
              tk->location.e_row);
      exit(1);
    }

    tokvec_add(instr->vec, id);
    tokvec_add(instr->vec, tk);

    check_next_token(i, tokens, "Missing value after RESW or RESB directive.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if(tk->type == RESB || tk->type == RESW) {
        if(tk->type == NUM) {
            offset = strtol(tk->str, NULL, 0);
        } else if(tk->type == HEX){
            offset = strtol(tk->str, NULL, 16);
        } else if (tk->type == BIN){
            offset = strtol(tk->str, NULL, 2);
        }else{
          LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing value after START directive or the value is not a constant.\n",
                  tk->location.s_col, tk->location.s_row, tk->location.e_col,
                  tk->location.e_row);
          exit(1);
        }

        if(tk->type == RESW){
            offset = SICAS_WORD_SIZE * offset;
        }
    }else{
        uint64_t num = 0;
        if(tk->type == NUM) {
            num = strtol(tk->str, NULL, 0);
        } else if(tk->type == HEX){
            num = strtol(tk->str, NULL, 16);
        } else if (tk->type == BIN){
            num = strtol(tk->str, NULL, 2);
        }else if (tk->type == STRING) {
          //FIXME: Take into account special characters.
            while(tk->str[num++] != '\0');
        } else{
          LOG_ERR("[%ld:%ld]|[%ld:%ld] Missing value after memory directive or the value is not a constant.\n",
                  tk->location.s_col, tk->location.s_row, tk->location.e_col,
                  tk->location.e_row);
          exit(1);
        }
        offset = long_log2(num)/(tk->type == WORD? SICAS_WORD_SIZE : SICAS_BYTE_SIZE);
        offset = offset == 0? 1 : offset;
    }

    break;

  default:
    token_print(*tk);
    printf("\n");
    LOG_PANIC("This token should not be here alone \n");
  }

  if(id != NULL){
    symtab_add_addr(sym, id->str, *loc_ctr);
  }

  instr->format = format;
  instr->type = type;
  instr->addr = *loc_ctr;
  *loc_ctr += offset;

  instrvec_add(instrs, instr);
  *idx = i;

  return i;
}

void parse_vector(TokenVector *vec, InstrVector *instrs, SymTable *sym) {
  long vec_size = vec->count;
  size_t i = 0;
  uint64_t loc_ctr = 0;

  while(i < vec_size){
    //FIXME: Maybe return error idk something?
    i = builder(vec, instrs, sym, &i, &loc_ctr);
  }
  instrs->end_addr = loc_ctr;
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

uint64_t long_log2(uint64_t num){
  if(num == 0){
    return 1;
  }

  uint64_t ctr = 0;
  while(num > 0){
    num = num >> 1;
    ctr++;
  }
  return ctr;
}

void instrvec_init(InstrVector *v){
  v->count = 0;
  v->start_addr = 0;
  v->end_addr = 0;
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

uint8_t symtab_add_symbol(SymTable *table, char *symbol){
  size_t key = hash_func(symbol);
  size_t count = table->map[key].count;

  for(size_t i = 0; i < count; i++){
    if(strcmp(table->map[key].values[i].symbol, symbol) == 0){
      return table->map[key].values[i].set;
    }
  }

  table->map[key].values[count].symbol = symbol;
  table->map[key].values[count].addr = 0;
  table->map[key].values[count].set = 0;
  table->map[key].count++;
  table->count++;

  if(table->map[key].count >= SYMTABLE_SIZE){
    LOG_PANIC("SYMTABLE has been completely filled.");
  }
  return 0;
}

void symtab_add_addr(SymTable *table, char *symbol, uint64_t addr){
  size_t key = hash_func(symbol);
  size_t count = table->map[key].count;

  for(size_t i = 0; i < count; i++){
    if(strcmp(table->map[key].values[i].symbol, symbol) == 0){
      table->map[key].values[i].addr = addr;
      return;
    }
  }

  table->map[key].values[count].symbol = symbol;
  table->map[key].values[count].addr = addr;
  table->map[key].values[count].set = 1;

  table->map[key].count++;
  table->count++;

  if(table->map[key].count >= SYMTABLE_SIZE){
    LOG_PANIC("SYMTABLE has been completely filled.");
  }
}

SymValue *symtab_get_symbol(SymTable *table, char *symbol){
  size_t key = hash_func(symbol);
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
  printf("addr:%08lx, ", instr->addr);
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
      printf("(%s,%08lx)",table->map[i].values[j].symbol,  table->map[i].values[j].addr);
    }
    printf("\n");
  }
}
#endif
