#ifndef SICAS_LOGGER
#define SICAS_LOGGER
#include "logger.h"
#endif

#ifndef SICAS_PARSER
#define SICAS_PARSER
#include "parser.h"
#endif

void assemble_instructions(InstrVector *instrs, SymTable *sym, FILE *output);
void assemble_header(InstrVector *instrs, SymTable *sym, FILE *output);
