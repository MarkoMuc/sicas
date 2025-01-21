#ifndef SICAS_PARSER
#define SICAS_PARSER
#include "../includes/parser.h"
#endif

Regs* parse_regs(const TokenVector *tokens, Instruction *instr, size_t *idx) {
  size_t i = *idx;
  Regs *regs = malloc(sizeof(*regs));

  if(!regs){
    LOG_PANIC("Could not allocate memory for regs struct.\n");
  }

  check_next_token(i, tokens, instr->loc, "Missing first register for instruction of format 2.\n");
  Token *tk = tokvec_get(tokens, i++);
  token_check_null(tk);

  if (tk->type == REGISTER) {
    regs->reg1 = mnemonic_get_reg(tk->str);
    check_next_token(i, tokens, tk->location, "Missing separating comma for instruction of format 2.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if (tk->type != COMMA) {
      LOG_XLERR(instr->loc, tk->location, "Instruction of type 2 is missing a separator between a registers.\n");
    }

    check_next_token(i, tokens, tk->location,"Missing second register for instruction of type 2.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if (tk->type == REGISTER) {
      regs->reg2 = mnemonic_get_reg(tk->str);
    } else {
      LOG_XLERR(instr->loc, tk->location, "Instruction of type 2 is missing a register as second argument.\n");
    }

  } else {
    LOG_XLERR(instr->loc, tk->location,"Instruction of type 2 is missing a register as first argument.\n");
  }

  instr->loc.e_row = tk->location.e_row;
  instr->loc.e_col = tk->location.e_col;

  *idx = i;

  return regs;
}

Mem *parse_mem_addr(const TokenVector *tokens, Instruction *instr, SymTable *sym, size_t *idx, const uint8_t float_instr) {
  Mem *mem = malloc(sizeof(*mem));
  size_t i = *idx;
  bool indexing_illegal = 0;

  if(!mem){
    LOG_PANIC("Could not allocate memory for mem struct.\n");
  }

  mem->indexed = false;
  mem->mem_type = SIM;

  check_next_token(i, tokens, instr->loc, "Instruction has no address operand.\n");
  Token *tk = tokvec_get(tokens, i++);
  token_check_null(tk);

  switch(tk->type) {
    case HASH:
    case AT:
      mem->mem_type = IND;

      if(tk->type == HASH){
        mem->mem_type = IMM;
      }

      check_next_token(i, tokens, instr->loc, "Missing operand for indirect or immediate addressing.\n");
      tk = tokvec_get(tokens, i++);
      token_check_null(tk);

      indexing_illegal = true;
      if(tk->type == ID || tk->type == NUM || tk->type == HEX || tk->type == BIN || tk->type == STRING){
        mem->tk = tk;
        if(tk->type == ID){
          symtab_add_symbol(sym, tk->str);
        }

        break;
      } else if(tk->type != LITERAL){
        LOG_XLERR(instr->loc, tk->location, "Missing identifier or constant after indirect or immediate addressing.\n");
      }
      // If there is a literal, go forward.
    case LITERAL:
      LOG_PANIC("LITERALS not implemented yet.\n");

      check_next_token(i, tokens, instr->loc, "No constant following a literal.\n");
      tk = tokvec_get(tokens, i++);
      token_check_null(tk);

      if (tk->type == FNUM) {
        if (float_instr == 0) {
          LOG_XLERR(instr->loc, tk->location, "Floats not allowed here.\n");
        }

        //tokvec_add(instr->vec, tk);
      } else if (tk->type == NUM || tk->type == HEX || tk->type == BIN || tk->type == STRING) {
        //tokvec_add(instr->vec, tk);
      } else {
        LOG_XLERR(instr->loc, tk->location, "Literal missing constant.\n");
      }

      break;

    case ID:
      symtab_add_symbol(sym, tk->str);
    case NUM:
    case HEX:
    case BIN:
    case STRING:
    case FNUM:
      //FIXME: does float_instr even matter here?
      if(tk->type == FNUM && float_instr){
        LOG_XLERR(instr->loc, tk->location, "Float constant not allowed here.\n");
      }
      mem->tk = tk;
      break;

    default:
      LOG_XLERR(instr->loc, tk->location, "Instruction does not contain a valid address.\n");
  }

  instr->loc.e_row = tk->location.e_row;
  instr->loc.e_col = tk->location.e_col;

  if(i < tokens->count){
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if (tk->type == COMMA) {
      if(indexing_illegal){
        LOG_XLERR(instr->loc, tk->location, "Immediate and indirect addressing cannot be paired with indexed addressing.\n");
      }

      check_next_token(i, tokens, instr->loc, "Indexed addressing is missing X after the comma.\n");
      tk = tokvec_get(tokens, i++);

      if (tk->type != REGISTER && strcmp(tk->str, "X")) {
        LOG_XLERR(instr->loc, tk->location, "Offset should be register X.\n");
      }
      mem->indexed = true;

      instr->loc.e_row = tk->location.e_row;
      instr->loc.e_col = tk->location.e_col;
    } else {
      i = i - 1;
    }
  }

  *idx = i;
  return mem;
}

size_t builder(const TokenVector *tokens, InstrVector *instrs, SymTable *sym, size_t *idx, uint64_t *loc_ctr) {
  size_t i = *idx;
  Token *id = NULL;
  uint64_t offset = 0;
  Token *tk = tokvec_get(tokens, i++);
  enum ftype format = THREE;
  enum itype type = INSTR;

  token_check_null(tk);
  Instruction *instr = instr_create();

  if (tk->type == PLUS) {
    format = FOUR;
    instr->loc.s_row = tk->location.s_row;
    instr->loc.s_col = tk->location.s_col;

    check_next_token(i, tokens, tk->location, "Missing token after +.");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);
  }

  //FIXME: Program name can overlap with a label
  if(tk->type == ID){
    id = tk;
    if(symtab_add_symbol(sym, tk->str)) {
      LOG_XLERR(tk->location, tk->location, "Symbol %s has multiple definitions.\n", tk->str);
    }

    check_next_token(i, tokens, tk->location, "Missing token after +.");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);
  }

  if(id && format != FOUR) {
    instr->loc.s_row = id->location.s_row;
    instr->loc.s_col = id->location.s_col;
  }else if(format != FOUR){
    instr->loc.s_row = tk->location.s_row;
    instr->loc.s_col = tk->location.s_col;
  }

  instr->loc.e_row = tk->location.e_row;
  instr->loc.e_col = tk->location.e_col;

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
    instr->instr = malloc(sizeof(MInstr));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc minstr.\n");
    }
    DIRECT_INSTR(instr)->op = tk->type;
    DIRECT_INSTR(instr)->oper = parse_mem_addr(tokens, instr, sym, &i, 0);

    offset = format;
    break;

  case ADDF:
  case COMPF:
  case DIVF:
  case LDF:
  case MULF:
  case STF:
  case SUBF:
    instr->instr = malloc(sizeof(MInstr));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc minstr.\n");
    }

    DIRECT_INSTR(instr)->op = tk->type;
    DIRECT_INSTR(instr)->oper = parse_mem_addr(tokens, instr, sym, &i, 1);

    offset = format;
    break;

  case ADDR:
  case COMPR:
  case DIVR:
  case MULR:
  case RMO:
  case SUBR:
    if (format == FOUR) {
      LOG_XLERR(instr->loc, instr->loc, "This instruction cannot be in format 4.\n");
    }

    format = TWO;
    instr->instr = malloc(sizeof(MInstr));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc minstr.\n");
    }

    DIRECT_INSTR(instr)->op = tk->type;
    DIRECT_INSTR(instr)->oper = parse_regs(tokens, instr, &i);

    offset = format;
    break;

  case CLEAR:
  case TIXR:
    if (format == FOUR) {
      LOG_XLERR(instr->loc, instr->loc, "This instruction cannot be in format 4.\n");
    }

    format = TWO;
    instr->instr = malloc(sizeof(MInstr));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc minstr.\n");
    }

    DIRECT_INSTR(instr)->op = tk->type;

    check_next_token(i, tokens, instr->loc, "Missing first register for instruction of format 2.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if (tk->type != REGISTER) {
      LOG_XLERR(instr->loc, tk->location, "Operand should be a register.\n");
    }


    if(tk->type != HEX && tk->type != NUM && tk->type != BIN) {
      LOG_XLERR(instr->loc, tk->location, "Operand one should be a register.\n");
    }

    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    DIRECT_INSTR(instr)->oper = malloc(sizeof(Regs));

    if(!DIRECT_INSTR(instr)->oper){
      LOG_PANIC("Failed to malloc regs struct.\n");
    }

    DIRECT_REGS(instr)->reg1 = mnemonic_get_reg(tk->str);
    DIRECT_REGS(instr)->reg2 = 0x0;

    offset = format;
    break;

  case FIX:
  case FLOAT:
  case HIO:
  case NORM:
  case SIO:
  case TIO:
    if(format == FOUR) {
      LOG_XLERR(instr->loc, instr->loc, "This instruction cannot be in format 4.\n");
    }

    format = ONE;

    instr->instr = malloc(sizeof(MInstr));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc minstr.\n");
    }

    DIRECT_INSTR(instr)->op = tk->type;
    DIRECT_INSTR(instr)->oper = NULL;

    offset = format;
    break;

  case RSUB:
    if(format == FOUR) {
      LOG_XLERR(instr->loc, instr->loc, "This instruction cannot be in format 4.\n");
    }

    instr->instr = malloc(sizeof(MInstr));

    if(!instr->instr) {
      LOG_PANIC("Failed to malloc minstr.\n");
    }

    DIRECT_INSTR(instr)->op = tk->type;

    DIRECT_INSTR(instr)->oper = malloc(sizeof(Mem));
    if(!DIRECT_INSTR(instr)->oper) {
      LOG_PANIC("Could not allocate memory for mem struct.\n");
    }

    DIRECT_MEM(instr)->indexed = false;
    DIRECT_MEM(instr)->mem_type = IMM;
    DIRECT_MEM(instr)->tk = malloc(sizeof(Token));

    if(!DIRECT_MEM(instr)->tk) {
      LOG_PANIC("Could not allocate memory for token struct.\n");
    }

    DIRECT_MEM(instr)->tk->type = NUM;
    DIRECT_MEM(instr)->tk->location.s_col = 0;
    DIRECT_MEM(instr)->tk->location.s_row = 0;
    DIRECT_MEM(instr)->tk->location.e_col = 0;
    DIRECT_MEM(instr)->tk->location.e_row = 0;
    DIRECT_MEM(instr)->tk->str = "0";

    offset = format;
    break;

  case SHIFTL:
  case SHIFTR:
    if (format == FOUR) {
      LOG_XLERR(instr->loc, instr->loc, "This instruction cannot be in format 4.\n");
    }

    format = TWO;
    instr->instr = malloc(sizeof(MInstr));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc minstr.\n");
    }

    DIRECT_INSTR(instr)->op = tk->type;
    DIRECT_INSTR(instr)->oper = malloc(sizeof(Regs));

    if(!DIRECT_INSTR(instr)->oper){
      LOG_PANIC("Failed to malloc regs struct.\n");
    }

    check_next_token(i, tokens, instr->loc, "Missing first register for instruction of format 2.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if(tk->type != REGISTER) {
      LOG_XLERR(instr->loc, tk->location, "Operand one should be a register.\n");
    }

    DIRECT_REGS(instr)->reg1 = mnemonic_get_reg(tk->str);

    check_next_token(i, tokens, instr->loc, "Missing integer for  format 2.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if(tk->type != HEX && tk->type != NUM && tk->type != BIN) {
      LOG_XLERR(instr->loc, tk->location, "Operand one should be a register.\n");
    }

    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    uint64_t shift = token_to_long(tk);
    if(shift < 1 || shift > 16) {
      LOG_XLERR(instr->loc, instr->loc, "Shift can only be between 1 and 16\n");
    }

    DIRECT_REGS(instr)->reg2 = (uint8_t) (shift - 1);

    offset = format;
    break;

  case SVC:
    if (format == FOUR) {
      LOG_XLERR(instr->loc, instr->loc, "This instruction cannot be in format 4.\n");
    }

    format = TWO;
    instr->instr = malloc(sizeof(MInstr));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc minstr.\n");
    }

    DIRECT_INSTR(instr)->oper = malloc(sizeof(Regs));

    if(!DIRECT_INSTR(instr)->oper){
      LOG_PANIC("Failed to malloc regs struct.\n");
    }

    DIRECT_INSTR(instr)->op = tk->type;

    check_next_token(i, tokens, instr->loc, "Missing integer.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    if(tk->type != HEX && tk->type != NUM && tk->type != BIN) {
      LOG_XLERR(instr->loc, tk->location, "Operand one should be a register.\n");
    }

    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    uint64_t interrupt = token_to_long(tk);
    if(interrupt < 1 || interrupt > 16) {
      LOG_XLERR(instr->loc, instr->loc, "Interrupt can only be between 1 and 16\n");
    }

    DIRECT_REGS(instr)->reg1 = (uint8_t) interrupt;
    DIRECT_REGS(instr)->reg2 = (uint8_t) 0x0;

    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    DIRECT_REGS(instr)->reg1 = mnemonic_get_reg(tk->str);
    DIRECT_REGS(instr)->reg2 = 0x0;

    offset = format;
    break;

  case START:
    format = ZERO;
    type = DIRECTIVE;

    instr->instr = malloc(sizeof(Directive));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc directive.\n");
    }

    if(*loc_ctr != 0) {
      LOG_XLERR(instr->loc, instr->loc, "Duplicate START directive or START is not the first instruction.\n");
    }

    if(!id || id->type != ID) {
      LOG_XLERR(instr->loc, instr->loc, "Missing program name before START directive.\n");
    }

    DIRECT_DIR(instr)->tk = id;
    DIRECT_DIR(instr)->directive = tk->type;

    check_next_token(i, tokens, instr->loc, "Missing value after START directive.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    if(tk->type == NUM || tk->type == HEX || tk->type == BIN) {
        *loc_ctr = token_to_long(tk);
    }else{
      LOG_XLERR(instr->loc, instr->loc, "Missing value after START directive or the value is not a constant.\n");
    }

    instrs->start_addr = *loc_ctr;
    offset = 0;
    break;

  case END:
    format = ZERO;
    type = DIRECTIVE;

    instr->instr = malloc(sizeof(Directive));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc directive.\n");
    }

    if(instrs->first_addr != 0) {
      LOG_XLERR(instr->loc, instr->loc, "Duplicate START directive or START is not the first instruction.\n");
    }

    DIRECT_DIR(instr)->directive = tk->type;

    check_next_token(i, tokens, instr->loc, "Missing value after END directive.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    DIRECT_DIR(instr)->tk = tk;
    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    if (tk->type == NUM || tk->type == HEX || tk->type == BIN ){
        instrs->first_addr = token_to_long(tk);
    } else if (tk->type != ID){
      LOG_XLERR(instr->loc, instr->loc, "Missing value after END directive or the value is not a constant/symbol.\n");
    }

    offset = 0;
    break;

  case BASE:
    format = ZERO;
    type = DIRECTIVE;

    instr->instr = malloc(sizeof(Directive));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc directive.\n");
    }

    DIRECT_DIR(instr)->directive = tk->type;

    check_next_token(i, tokens, instr->loc, "Missing value after BASE directive.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    if(tk->type == NUM || tk->type == HEX || tk->type == BIN || tk->type == ID) {
      if(tk->type == ID){
        symtab_add_symbol(sym, tk->str);
      }
    }else{
      LOG_XLERR(instr->loc, instr->loc, "Missing value after BASE directive or the value is not a constant or symbol.\n");
    }

    DIRECT_DIR(instr)->tk = tk;

    offset = 0;
    break;

  case BYTE:
  case WORD:
    format = ZERO;
    type = IMEM;

    instr->instr = malloc(sizeof(InitMemory));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc initialized memory.\n");
    }

    DIRECT_IMEM(instr)->type = tk->type;

    if(!id || id->type != ID) {
      LOG_XLERR(instr->loc, instr->loc, "Missing program label before BYTE or WORD directive.\n");
    }

    check_next_token(i, tokens, instr->loc, "Missing value after BYTE or WORD directive.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    uint64_t res_bytes = 0;
    if (tk->type == NUM || tk->type == HEX || tk->type == BIN ){
        res_bytes = long_ceil(long_log2(token_to_long(tk)), 8);
    } else if (tk->type == STRING) {
        bool escape = false;
        size_t counter = 0;
        while(tk->str[counter] != '\0'){
          if(!escape && tk->str[counter] == '\\'){
            escape = true;
            counter++;
            continue;
          }
          res_bytes++;
          counter++;
          escape = false;
        }
    } else{
      LOG_XLERR(instr->loc, instr->loc, "Missing value after WORD/BYTE or the value is not a constant.\n");
    }

    const uint8_t format_size = DIRECT_IMEM(instr)->type == WORD? SICAS_WORD_SIZE : SICAS_BYTE_SIZE;
    offset = format_size*(long_ceil(res_bytes, format_size));

    DIRECT_IMEM(instr)->start_addr = *loc_ctr;
    DIRECT_IMEM(instr)->reserved = offset;
    DIRECT_IMEM(instr)->raw = res_bytes == 0? 1 : res_bytes;
    DIRECT_IMEM(instr)->tk = tk;

    break;

  case RESB:
  case RESW:
    format = ZERO;
    type = RMEM;

    instr->instr = malloc(sizeof(ResMemory));

    if(!instr->instr){
      LOG_PANIC("Failed to malloc reserved memory.\n");
    }

    DIRECT_RMEM(instr)->type = tk->type;

    if(!id || id->type != ID) {
      LOG_XLERR(instr->loc, instr->loc, "Missing program label before RESB or RESW directive.\n");
    }

    check_next_token(i, tokens, instr->loc, "Missing value after RESW or RESB directive.\n");
    tk = tokvec_get(tokens, i++);
    token_check_null(tk);

    instr->loc.e_row = tk->location.e_row;
    instr->loc.e_col = tk->location.e_col;

    if (tk->type == NUM || tk->type == HEX || tk->type == BIN ){
        offset = token_to_long(tk);
    } else{
      LOG_XLERR(tk->location, tk->location, "Missing value after RESW / RESB or the value is not a constant.\n");
    }

    if(tk->type == RESW){
        offset = SICAS_WORD_SIZE * offset;
    }

    DIRECT_RMEM(instr)->start_addr = *loc_ctr;
    DIRECT_RMEM(instr)->reserved = offset;

    break;

  default:
    LOG_ERR("This token should not be here alone.\n");
    if(id){
      token_print(*id);
      printf("\n");
      token_print(*tk);
    }else{
      token_print(*tk);
    }
    printf("\n");
    exit(1);
  }

  if(id){
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

void parse_vector(const TokenVector *vec, InstrVector *instrs, SymTable *sym) {
  const long vec_size = vec->count;
  size_t i = 0;
  uint64_t loc_ctr = 0;

  while(i < vec_size){
    //FIXME: Maybe return error idk something?
    i = builder(vec, instrs, sym, &i, &loc_ctr);
  }
  instrs->end_addr = loc_ctr;
  instrs->first_addr = instrs->first_addr == 0? instrs->start_addr : instrs->first_addr;
}

Instruction *instr_create() {
  Instruction *instr = malloc(sizeof(*instr));
  if (!instr) {
    LOG_PANIC("Failed to allocate memory for the instruction.\n");
  }

  instr->type = ONE;
  instr->addr = 0;
  instr->format = 0;
  instr->instr = NULL;

  return instr;
}

uint64_t token_to_long(const Token *tk){
  uint64_t res = 0;
  if (tk->type == NUM){
      res = strtol(tk->str, NULL, 0);
  } else if(tk->type == HEX){
      res = strtol(tk->str, NULL, 16);
  } else if (tk->type == BIN){
      res = strtol(tk->str, NULL, 2);
  } else {
    LOG_PANIC("Passing non integer type token.\n");
  }

  return res;
}

uint64_t long_ceil(uint64_t num1, uint64_t div){
  if(num1 == 0) {
    return 1;
  }

  if(num1 % div != 0){
     return (num1/div) + 1;
  }

  return num1/div;
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
  const size_t count = v->count;
  for(size_t i = 0; i < count; i++) {
    Instruction *instr = v->items[i];
    if(instr->instr){
      if(instr->type == INSTR) {
        free(DIRECT_INSTR(instr)->oper);
      }
      free(instr->instr);
    }
  }

  free(v->items);
}

void instrvec_free_destructive(InstrVector *v){
  if (!v) {
    LOG_PANIC("Error while deallocating the instruction vector.\n");
  }

  const size_t count = v->count;
  for(size_t i = 0; i < count; i++) {
    Instruction *instr = v->items[i];

    if(instr->instr){
      if(instr->type == DIRECTIVE){
        free(DIRECT_DIR(instr)->tk);
      }else if(instr->type == INSTR) {
        if(instr->format == TWO) {
          free(DIRECT_INSTR(instr)->oper);
        }else if(instr->format == THREE || instr->format == FOUR){
          free(DIRECT_MEM(instr)->tk);
        } else{
          LOG_PANIC("Instructions of this format should not exist.\n");
        }
      }else{
        //FIXME: implement others
        LOG_PANIC("Not implemented this type of instruction yet.\n");
      }

      free(instr->instr);
    }
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

void instrvec_replace(InstrVector *v, Instruction *el, const size_t idx){
  if (!v || !el || idx < 0) {
    LOG_PANIC("Error while replacing element in the instruction vector.\n");
  }

  if (idx >= v->count) {
    LOG_PANIC("Error while replacing instruction at index %ld current count is %ld.\n", idx,
            v->count);
  }

  const size_t count = v->count;
  Instruction *old = v->items[idx];
  v->items[idx] = el;
  for (size_t i = idx + 1; i < count; i++) {
    Instruction *inter = v->items[i];
    v->items[i] = old;
    old = inter;
  }

  instrvec_add(v, old);
}

void instrvec_rm(InstrVector *v, const size_t idx){
  if (!v|| idx < 0) {
    LOG_PANIC("Error while removing an instruction from the vector.\n");
  }

  if (idx >= v->capacity) {
    LOG_PANIC("Error while removing instruction from instruction vector at index %ld capacity is %ld.\n", idx,
            v->capacity);
  }

  const int count = v->count;
  for (size_t i = idx + 1; i < count; i++) {
    v->items[i - 1] = v->items[i];
  }

  v->count -= 1;
}

Instruction *instrvec_get(const InstrVector *v, const size_t idx){
  if (!v || idx < 0) {
    LOG_PANIC("Error while adding an element to the instruction vector.\n");
  }

  if (idx >= v->count) {
    LOG_PANIC("Error while getting instruction from index %ld count is %ld.\n", idx,
            v->count);
  }

  if (idx >= v->capacity) {
    LOG_PANIC("Error while getting instruction from index %ld capacity is %ld.\n", idx,
            v->capacity);
  }

  return v->items[idx];
}

size_t djb2_hash(const char* str){
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
    const size_t count = table->map[i].count;
    for(size_t j = 0; j < count; j++){
      free(table->map[i].values[j].symbol);
    }
  }

  free(table->map);
}

uint8_t symtab_add_symbol(SymTable *table, char *symbol){
  const size_t key = hash_func(symbol);
  const size_t count = table->map[key].count;

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

void symtab_add_addr(SymTable *table, char *symbol, const uint64_t addr){
  const size_t key = hash_func(symbol);
  const size_t count = table->map[key].count;

  for(size_t i = 0; i < count; i++){
    if(strcmp(table->map[key].values[i].symbol, symbol) == 0){
      table->map[key].values[i].addr = addr;
      table->map[key].values[i].set = 1;
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

SymValue *symtab_get_symbol(const SymTable *table, const char *symbol){
  const size_t key = hash_func(symbol);
  const size_t count = table->map[key].count;

  for(size_t i = 0; i < count; i++){
    if(strcmp(table->map[key].values[i].symbol, symbol) == 0){
      return &(table->map[key].values[i]);
    }
  }

  return NULL;
}

uint64_t symtab_check_get_addr(const SymTable *table, const char *symbol, const Instruction *instr){
  const SymValue *val = symtab_get_symbol(table, symbol);

  if(!val) {
    LOG_PANIC("Symbol exist but is not present in the symbol table.\n");
  }

  if(val->set == false) {
    LOG_XLERR(instr->loc, instr->loc,"Symbol %s has not been defined yet.\n", symbol);
  }

  return val->addr;
}

#if defined(PARSER_DEBUG_MODE) || defined(DEBUG_MODE)
void instruction_print(Instruction *instr) {
  printf("[%ld, %ld]:[%ld, %ld] ", instr->loc.s_row, instr->loc.s_col, instr->loc.e_row, instr->loc.e_col);\

  switch(instr->type) {
    case INSTR:
      printf("itype: INSTR, ");
      break;
    case IMEM:
      printf("itype: IMEM, ");
      break;
    case RMEM:
      printf("itype: RMEM, ");
      break;
    case DIRECTIVE:
      printf("itype: DIRECTIVE, ");
      break;
    default:
      printf("ERROR, ");
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
      printf("ERROR, ");
  }
  printf("addr:%08lx, ", instr->addr);

  switch(instr->type) {
    case DIRECTIVE:{
      Directive *d = instr->instr;
      token_type_print(d->directive);
      printf(" ");
      if(d->tk){
        token_print(*d->tk);
      }
      break;
    }
    case INSTR:{
      MInstr *m = instr->instr;
      token_type_print(m->op);
      printf(" ");
      if(instr->format == TWO){
        Regs *r = m->oper;
        printf("%d, %d", r->reg1, r->reg2);
      }else if(instr->format == THREE || instr->format == FOUR){
        Mem *mem = m->oper;
        printf("%s", mem->tk->str);
        if(mem->indexed){
          printf(",X");
        }
      }
      break;
    }
    case IMEM:{
      InitMemory *m = instr->instr;
      token_type_print(m->type);
      printf(" ");
      printf("%08lx + %08lx (%08lx) ", m->start_addr, m->reserved, m->raw);
      token_print(*m->tk);
      break;
    }
    case RMEM:{
      ResMemory *m = instr->instr;
      token_type_print(m->type);
      printf(" ");
      printf("[%08lx->%08lx] %08lx", m->start_addr, m->start_addr + m->reserved, m->reserved);
      break;
    }
    default:
      printf("ERROR, ");
  }

  printf("\n");
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
