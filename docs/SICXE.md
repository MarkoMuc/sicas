# The Simplified Instruction Computer or SIC

There are two versions of SIC, one is the basic SIC the other one is known as SIC/XE (XE as in extra equipment or extra expensive). The two versions are **upward compatible**.

## Machine architecture of SIC

### Memory

- Memory consists of 8-bit bytes.
- Three (3) consecutive bytes form a **word** (24bits).
- All addresses on SIC are byte addresses.
- Words are addressed by the location of their lowest numbered byte.
- The maximum available memory is 2^15 bytes.

### Registers

- Basic SIC contains five registers, all of them are special use.
- Each register is 24 bits in length.
- SIC/XE adds the following registers B, S, T and F.

| Mnemonic  | Number | Use                                                                            |
|-----------|--------|--------------------------------------------------------------------------------|
| A         | 0      | Accumulator, used for arithmetic operations.                                   |
| X         | 1      | Index register, used for addressing.                                           |
| L         | 2      | Linkage register, JSUB instruction stores the return address in this register. |
| PC        | 8      | Program counter. Address of the next instruction to be fetched for execution.  |
| SW        | 9      | Status word, includes Condition Code.                                          |

### Data Formats

- Integers are stored as 24-bit binary numbers.
- 2's complement representation is used for negative numbers.
- Characters are stored as 8-bit ASCII codes.

### Instruction Formats

- All machine instructions on the standard version of SIC have the following 24-bit format: 
``| opcode (8-bit) | x (1-bit) | address (15-bit) | ``
- The flag bit `x` is used to indicate indexed-addressing mode.

### Addressing Modes

- There are two addressing modes available, indicated by setting of the `x` bit in the instruction.
- The following table describes how the *target address* is calculated from the address given in the instruction.
- `(X)` represents the contents of the X register.
- Base and PC relative addressing is SIC/XE exclusive.

| Mode            | Indication | Target address calculation           |
|-----------------|------------|--------------------------------------|
| Direct          | x=0        | TA = address                         |
| Indexed         | x=1        | TA = address + (X)                   |

### Instruction Set

- All arithmetic operations involve register A and a word in memory, with the result being left in the register.
- SW register contains condition code, which is used with conditional jumps.
- Two instructions are provided for subroutine linkage. `JSUB` jumps to the subroutine, placing the address in register `L`. `RSUB` returns by jumpting to the address contained in register L.

### Input and Output

- On the standard SIC, input and output are performed by transferring 1 byte at a time to or from the rightmost 8 bits of register A.
- Each device is assigned a unique 8-bit code.
- There are three I/O instructions:
    1. TD tests if the addressed device is ready to send or receive a byte of data. CC is set to indicate the result.
    2. RD reads from the device into register A.
    3. WD writes to the device from register A.
- The sequence of `TD` followed by `RD` or `WD` must be repeated for each byte of data to be read or written.

## Machine architecture of SIC/XE

### Memory

- Maximum available memory is expanded to 1 megabyte or 2^20 bytes.
- Additional addressing modes and instruction formats are added, to work with this expanded memory.

### Registers

- SIC/XE adds three additional registers.
- The following table shows the whole list of available registers in the SIC/XE machine.

| Mnemonic  | Number | Use                                                                            |
|-----------|--------|--------------------------------------------------------------------------------|
| A         | 0      | Accumulator, used for arithmetic operations.                                   |
| X         | 1      | Index register, used for addressing.                                           |
| L         | 2      | Linkage register, JSUB instruction stores the return address in this register. |
| B         | 3      | Base register, used for addressing.                                            |
| S         | 4      | General working register.                                                      |
| T         | 5      | General working register.                                                      |
| F         | 6      | Floating-point accumulator (48bits).                                           |
| PC        | 8      | Program counter. Address of the next instruction to be fetched for execution.  |
| SW        | 9      | Status word, includes Condition Code.                                          |

### Data formats

- Provides an additional 48-bit floating-point data type.
- Format: `` | s (1-bit) | exponent (11-bit) | fraction (38-bit) | ``
- The fraction is interpreted as a value between 0 and 1; that is, the assumed binary point is immediately before the high-order bit.
- For normalized floating point numbers, the high-order bit of the fraction must be 1.
- The exponent is interpreted as an unsigned number between 0 and 2047.
- If the exponent has value *e* and the fraction has value *f*, the absolute value of the number represented is `f*2^(e-1024)
- The sign is indicated by the value of s. `s = 0` is a positive number.
- If all bits are 0, the value is 0.

### Instruction Formats

- The larger memory means that an address will no longer fit into a 15-bit field. Thus the instruction format used on the standard version of SIC is no longer suitable.
- There are two possible options:
    1. Use some form of relative addressing (Instruction format 3).
    2. Extend the address field to 20 bits (Instruction format 4).
- Additionally SIC/XE provides some instruction that do not reference memory at all:
    1. Instruction format 1 consists only of the opcode.
    2. Instruction format 2 uses two registers as operands.
- Following are the new instruction formats:
    1. Format 1 (1 byte) : ``| op (8-bit) |``
    2. Format 2 (2 bytes) : ``| op (8-bit) | r1 (4-bit) | r2 (4-bit) |``
    3. Format 3 (3 bytes) :  ``| op (6-bit) | n (1-bit) | i (1-bit) | x (1-bit) | b (1-bit) | p (1-bit) | e (1-bit) | disp (12-bit) | ``
    4. Format 4 (4 bytes) :  ``| op (6-bit) | n (1-bit) | i (1-bit) | x (1-bit) | b (1-bit) | p (1-bit) | e (1-bit) | address (20-bit) | ``
- Bit *e* is used to distinguish between Formats 3 and 4 (`e = 0` means Format 3).

### Addressing Modes

- Two new relative addressing modes are available for use with instructions assembled using format 3.
- The full table of addressing modes in SIC/XE is described in the following table.

| Mode            | Indication | Target address calculation            |
|-----------------|------------|---------------------------------------|
| Direct          | x=0        | TA = address                          |
| Indexed         | x=1        | TA = address + (X)                    |
| Base relative   | b=1, p=0   | TA = (B) + disp; (0<=disp<=4095)      |
| PC relative     | b=0, p=1   | TA = (PC) + disp; (-2048<=disp<=2047) |

- For **Base relative addressing** the displacement field *disp* is interpreted as a 12-bit unsigned integer.
- For **Program counter relative addressing** the displacement field *disp* is interpreted as a 12-bit signed integer in 2's complement.
- **Direct addressing* can be performed in two ways:
    1. For format 3 instructions, if bits *b* and *p* are set to `0`, the *disp* field is taken to be the target address.
    2. For Format 4 instructions, bits *b* and *p* are normally set to `0`, and the target address is taken from the address field of the instruction.
- Any of this addressing modes can also be combined with *indexed* addressing.
    - If bit `x` is set to `1`, the term `(X)` is added in the target address calculation.
- For instructions in Format 3 and 4, the bits `i` and `n` specify how the target address is used:
    1. `i=1, n=0` the target address itself is used as the operand value; no memory reference is performed. This is called **immediate addressing**.
    2. `i=0, n=1`, the word at the location given by the target address is fetched; the *value* contained in this word is then taken as the address of the operand value. This is called **indirect addressing**.
    3. `i=0,n=0` or `i=1,n=1` the target address is taken as the location of the operand. This is called **simple addressing**.
- Indexing cannot be used with immediate or indirect addressing.
- SIC/XE instructions that specify neither immediate nor indirect addressing are assembled with bits *n* and *i* set to 1. Assembler for SIC will set the bits in both of these positions to 0, since all SIC opcodes end with `00`.
- SIC/XE has a special hardware feature designed to provide the upward compatibility mentioned earlier. If bits *n* and *i* are both `0`, then bits *b,p,e* are considered part of the address field of the instruction (rather than flags indicating addressing modes). This makes instruction Format 3 identical to the format used on the standard version of SIC.

### Instruction Set

- SIC/XE provides all of the instructions that are available on the standard version.
- It also expands the instruction set with:
    - Load and store instructions for the new registers.
    - Floating-point arithmetic operations.
    - Register as operand instructions.
    - Superviosr call instructions.
    - etc.

### Input and Output

- The standard I/O instructions we discussed for SIC are also available on SIC/XE.
- In addition there are I/O channels that can be used to perform input and output while the CPU is executing other instructions.
- This allows overlapping computing and I/O, resulting in more efficient system operation. The instructions SIO, TIO and HIO are used to start, test and halt the operation of I/O channels.

## Main differences between SIC and SIC/XE

- SIC has `2^15` memory, SIC/XE has `2^20` memory.
- SIC has 5 registers (A, X, L, PC and SW), SIC/XE has 9 registers.
- SIC has no floating-point hardware, SIC/XE has floating-point hardware.
- SIC has a single instruction format, SIC/XE has four instruction formats. SIC instruction is actually a variant of the SIC/XE format 3 instruction.
- SIC only has indexed and direct addressing. SIC/XE adds relative addressing.
- For instructions that do not specify neither immediate nor indirect addressing, the SIC assembler sets bits `n` and `i` to `0`, while the SIC/XE assembler sets this bits to `1`. This is because 8-bit binary instructions for all of the SIC instruction end in `00`.
- SIC/XE interprets bits `b`, `p` and `e` as part of the address field, if bits `n` and `i` are set to `0`.
- SIC/XE has I/O channels.
