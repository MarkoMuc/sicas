#ifndef SICAS_PARSER
#define SICAS_PARSER
#include "../includes/parser.h"
#endif

uint8_t parse_regs(TokenVector *tokens, TokenVector *new, long *idx){
  Token *tk = tokvec_get(tokens, *idx);
  *idx = *idx + 1;
  
  if(tk->type == REGISTER){
    tokvec_add(new, tk);
    tk = tokvec_get(tokens, *idx);
    *idx = *idx + 1;

    if(tk->type == COMMA){
      tokvec_add(new, tk);
      tk = tokvec_get(tokens, *idx);
      *idx = *idx + 1;
    }else{
      Token *s_tk = tokvec_get(new, 0);
      LOG_ERR("[%d:%d]|[%d:%d] Missing second argument.\n", s_tk->location.s_col,
                s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
      return 1;
    }

    if(tk->type == REGISTER){
      tokvec_add(new, tk);
    }else{
      Token *s_tk = tokvec_get(new, 0);
      LOG_ERR("[%d:%d]|[%d:%d] Argument 2 is not a register.\n", s_tk->location.s_col,
                s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
      return 1;
    }

  }else{
    Token *s_tk = tokvec_get(new, 0);
    LOG_ERR("[%d:%d]|[%d:%d] Argument 1 is not a register.\n", s_tk->location.s_col,
              s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
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
      LOG_ERR("[%d:%d]|[%d:%d] Floats not allowed here.\n", s_tk->location.s_col,
              s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
        return 1;
      }

      tokvec_add(new, tk);
    } else if (tk->type == NUM || tk->type == HEX || tk->type == BIN) {
      tokvec_add(new, tk);
    } else {
      Token *s_tk = tokvec_get(new, 0);
      LOG_ERR("[%d:%d]|[%d:%d] Literal missing numeral.\n", s_tk->location.s_col,
              s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
      return 1;
    }

    return 0;
  }

  if(tk->type == AT){
    tokvec_add(new, tk);
    tk = tokvec_get(tokens, *idx);
    *idx = *idx + 1;
  
    //TODO:Is only ID allowed after?
    if (tk->type == ID) {
      tokvec_add(new, tk);
    }else{
      Token *s_tk = tokvec_get(new, 0);
      LOG_ERR("[%d:%d]|[%d:%d] Missing identifier after @.\n", s_tk->location.s_col,
              s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
      return 1;
    }

    //TODO: Comma after this allowed?
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
        LOG_ERR("[%d:%d]|[%d:%d] Offset should be X.\n", s_tk->location.s_col,
                s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
        return 1;
      }
    }else{
      *idx = *idx - 1;
    }
  }

  return 0;
}

long builder(TokenVector *tokens, TokenVector *sym, TokenStack *stack,
             long idx) {
  Token *tk = tokvec_get(tokens, idx);
  uint8_t format = 0;

  token_check_null(tk, "Instruction is missing parameters.\n");

  if (tk->type == PLUS) {
    format = 4;
  }


  tokst_push(stack, tk);
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
      if(format != 4){ format = 3;}
      if (parse_mem_addr(tokens, vc, &idx, 0)) {exit(1);}
      break;

    case ADDF:
    case COMPF:
    case DIVF:
    case LDF:
    case MULF:
    case STF:
    case SUBF:
      if(format != 4){ format = 3;}
      if (parse_mem_addr(tokens, vc, &idx, 1)) {exit(1);}
      break;

    case ADDR:
    case COMPR:
    case DIVR:
    case MULR:
    case RMO:
    case SUBR:
      if(format == 4){
        LOG_ERR("[%d:%d]|[%d:%d] This instruction cannot be in format 4.\n", tk->location.s_col, tk->location.s_row, tk->location.e_col, tk->location.e_row);
        exit(1);
      }
      format = 2;
      if (parse_regs(tokens, vc, &idx)) {exit(1);}
      break;

    case CLEAR:
      if(format == 4){
        LOG_ERR("[%d:%d]|[%d:%d] This instruction cannot be in format 4.\n", tk->location.s_col, tk->location.s_row, tk->location.e_col, tk->location.e_row);
        exit(1);
      }
      format = 2;

      tk = tokvec_get(tokens, idx);
      if(tk->type == REGISTER){
        tokvec_add(vc, tk);
      }else{
        Token *s_tk = tokvec_get(vc, 0);
        LOG_ERR("[%d:%d]|[%d:%d] Argument should be a register.\n", s_tk->location.s_col,
                  s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
        return 1;
      }
      break;

    case HIO:
    case SIO:
    case TIO:
      if(format == 4){
        LOG_ERR("[%d:%d]|[%d:%d] This instruction cannot be in format 4.\n", tk->location.s_col, tk->location.s_row, tk->location.e_col, tk->location.e_row);
        exit(1);
      }
      format = 1;
      tokvec_add(vc, tk);
      tk = tokvec_get(tokens, idx);
      break;

    case FIX:
    case FLOAT:
    case NORM:
      if(format == 4){
        LOG_ERR("[%d:%d]|[%d:%d] This instruction cannot be in format 4.\n", tk->location.s_col, tk->location.s_row, tk->location.e_col, tk->location.e_row);
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
      if(tk->type == REGISTER){
        tokvec_add(vc, tk);
      }else{
        Token *s_tk = tokvec_get(vc, 0);
        LOG_ERR("[%d:%d]|[%d:%d] Argument 1 is not a register.\n", s_tk->location.s_col,
                  s_tk->location.s_row, tk->location.e_col, tk->location.e_row);
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
      //TODO
      break;
    
    case BYTE:
    case WORD:
    case RESB:
    case RESW:
      //GO until hitting something that is not a number
      break;

    default:
      LOG_ERR("This token should not be here alone %ld\n", idx);
      exit(1);
  }

  return idx;
}

void parse_vector(TokenVector *vec, TokenVector *sym) {
  long vec_size = vec->count;
  TokenStack *stack = malloc(sizeof(*stack));
  tokst_init(stack);

  for (long i = 0; i < vec_size; i++) {
    i = builder(vec, sym, stack, i);
  }
}

void tokst_push(TokenStack *s, Token *el) { tokvec_add(s, el); }

Token *tokst_peek(TokenStack *s) {
  size_t idx = tokvec_size(s);
  Token *t = tokvec_get(s, idx);
  return t;
}

Token *tokst_pop(TokenStack *s) {
  size_t idx = tokvec_size(s);
  Token *t = tokvec_get(s, idx);
  tokvec_rm_at(s, idx);
  return t;
}
void tokst_init(TokenStack *s) { tokvec_init(s); }

void tokst_free(TokenStack *s) { tokvec_free(s); }

#ifdef DEBUG_MODE
void tokst_print(TokenStack *s) { tokvec_print(s); }
#endif
