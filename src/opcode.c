#ifndef SICAS_OPCODE
#define SICAS_OPCODE
#include "../includes/opcode.h"
#endif

uint8_t get_opcode(enum ttype operation){
  switch (operation) {
  // -- CPU Instructions
    case ADD: return 0x18;
    case ADDF: return 0x58;
    case ADDR: return 0x90;
    case AND: return 0x40;
    case CLEAR: return 0xB4;
    case COMP: return 0x28;
    case COMPF: return 0x88;
    case COMPR: return 0xA0;
    case DIV: return 0x24;
    case DIVF: return 0x64;
    case DIVR: return 0x9C;
    case FIX: return 0xC4;
    case FLOAT: return 0xC0;
    case HIO: return 0xF4;
    case J: return 0x3C;
    case JEQ: return 0x30;
    case JGT: return 0x34;
    case JLT: return 0x38;
    case JSUB: return 0x48;
    case LDA: return 0x00;
    case LDB: return 0x68;
    case LDCH: return 0x50;
    case LDF: return 0x70;
    case LDL: return 0x08;
    case LDS: return 0x6C;
    case LDT: return 0x74;
    case LDX: return 0x04;
    case LPS: return 0xD0;
    case MUL: return 0x20;
    case MULF: return 0x60;
    case MULR: return 0x98;
    case NORM: return 0xC8;
    case OR: return 0x44;
    case RD: return 0xD8;
    case RMO: return 0xAC;
    case RSUB: return 0x4C;
    case SHIFTL: return 0xA4;
    case SHIFTR: return 0xA8;
    case SIO: return 0xF0;
    case SSK: return 0xEC;
    case STA: return 0x0C;
    case STB: return 0x78;
    case STCH: return 0x54;
    case STF: return 0x80;
    case STI: return 0xD4;
    case STL: return 0x14;
    case STS: return 0x7C;
    case STSW: return 0xE8;
    case STT: return 0x84;
    case STX: return 0x10;
    case SUB: return 0x1C;
    case SUBF: return 0x5C;
    case SUBR: return 0x94;
    case SVC: return 0xB0;
    case TD: return 0xE0;
    case TIO: return 0xF8;
    case TIX: return 0x2C;
    case TIXR: return 0xB8;
    case WD: return 0xDC;
  default:
    LOG_PANIC("Invalid token\n");
  }
}
