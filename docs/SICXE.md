# The Simplified Instruction Computer or SIC

There are two versions of SIC, one is the basic SIC the other one is known as SIC/XE (XE as in extra equipment or extra expensive). The two versions are **upward compatible**.

## Machine architecture of SIC and SIC/XE

### Memory

- Memory consists of 8-bit bytes.
- Three (3) consecutive bytes form a **word** (24bits).
- All addresses on SIC are byte addresses.
- Words are addressed by the location of their lowest numbered byte.
- There are a total of 2^15 bytes in the computer memory.
- SIC/XE: 
    - Maximum memory available is expanded to 1 megabyte or 2^20 bytes.

### Registers

- There are five registers, all of them are special use.
- Each register is 24 bits in length.
- SIC/XE: registers B, S, T and F are added.

| Mnemonic | Number | Use                                                                            |
|-----------|--------|--------------------------------------------------------------------------------|
| A         | 0      | Accumulator, used for arithmetic operations.                                   |
| X         | 1      | Index register, used for addressing.                                           |
| L         | 2      | Linkage register, JSUB instruction stores the return address in this register. |
| B         | 3      | Base register, used for addressing.                                            |
| S         | 4      | General working register.                                                      |
| T         | 5      | General working register.                                                      |
| F         | 6      | Floating-point accumulator(48bits).                                            |
| PC        | 8      | Program counter. Address of the next instruction to be fetched for execution.  |
| SW        | 9      | Status word, includes Condition Code.                                          |

### Data Formats

- Integers are stored as 24-bit binary numbers.
- 2's complement representation is used for negative numbers.
- SIC/XE: 
    - Provides an additional 48-bit floating-point data type.
    - Format: `` | s (1-bit) | exponent (11-bit) | fraction (38-bit) | ``
    - The fraction is interpreted as a value between 0 and 1; that is, the assumed binary point is immediately before the high-order bit.
    - For normalized floating point numbers, the high-order bit of the fraction must be 1.
    - The exponent is interpreted as an unsigned number between 0 and 2047.
    - If the exponent has value *e* and the fraction has value *f*, the absolute value of the number represented is `f*2^(e-1024)
    - The sign is indicated by the value of s. `s = 0` is a positive number.
    - If all bits are 0, the value is 0.

### Instruction Formats

- All machine instructions have the following 24-bit format: ``| opcode (8-bit) | x (1-bit) | address (15-bit) | ``
- The flag bit x is used to indicate indexed-addressing mode.
- SIC/XE:
    - New instruction formats are added:
        1. Format 1 (1 byte) : ``| op (8-bit) |``
        2. Format 2 (2 bytes) : ``| op (8-bit) | r1 (4-bit) | r2 (4-bit) |``
        3. Format 3 (3 bytes) :  ``| op (6-bit) | n (1-bit) | i (1-bit) | x (1-bit) | b (1-bit) | p (1-bit) | e (1-bit) | disp (12-bit) | ``
        4. Format 4 (4 bytes) :  ``| op (6-bit) | n (1-bit) | i (1-bit) | x (1-bit) | b (1-bit) | p (1-bit) | e (1-bit) | address (20-bit) | ``
    - Bit *e* is used to distinguish between Formats 3 and 4 (`e = 0` means Format 3).
### Addressing Modes

- The following table describes how the *target address* is calculated from the address given in the instruction.
- `(X)` represents the contents of the X register.
- Base and PC relative addressing is SIC/XE exclusive.

| Mode            | Indication | Target address calculation           |
|-----------------|------------|--------------------------------------|
| Direct          | x=0        | TA = address                         |
| Indexed         | x=1        | TA = address + (X)                   |
| Base relative   | b=1, p=0   | TA = (B) + disp; (0<=disp<=4095)     |
| PC-relative     | b=0, p=1   | TA = (PC) + disp; (-2048<=disp<=2047 |

- In base relative addressing, *disp* is interpreted as a 12-bit unsigned integer.
- In PC-relative addressing, *disp* is a 12-bit signed integer in 2's complement.
- If bits *b* and *p* are set to 0, the *disp* field from Format 3 instruction is taken to be the target address.
- For a Format 4 instruction, bits *b* and *p* are normally set to 0, and the target address is taken from the address field of the instruction.
- This is called **direct addressing**.
- Any of this addressing modes can also be combined with *indexed* addressing, if bit *x* is set to 1.
    - The term `(X)` is added in the target address calculation.
- Standard SIC only uses direct addressing.
- Bits *i* and *n* specify how the target address is used:
    1. `i=1, n=0` the target address itself is used as the operand value; no memory reference is performed. This is called **immediate addressing**.
    2. `i=0, n=1`, the word at the location given by the target address is fetched; the *value* contained in this word is then taken as the address of the operand value. This is called **indirect addressing**.
    3. `i=0,n=0` or `i=1,n=1` the target address is taken as the location of the operand. This is called **simple addressing**.
- Indexing cannot be used with immediate or indirect addressing.
- SIC/XE instructions that specify neither immediate nor indirect addressing are assembled with bits *n* and *i* set to 1. Assembler for SIC will set the bits in both of these positions to 0, since all SIC opcodes end with `00`.
- If bits *n* and *i* are both `0`, then bits *b,p,e* are considered part of the address field of the instruction. This makes instruction Format 3 identical to the format used on the standard version of SIC.


### Input and Output

- On the standard SIC, input and output are performed by transferring 1 byte at a time to or from the rightmost 8 bits of register A.
- Each device is assigned a unique 8-bit code.
- There are three I/O instructions:
    1. TD tests if device is available.
    2. RD reads from the device into register A.
    3. WD writes to the device from register A.
- SIC/XE adds I/O channels
    - Perform I/O while CPU is executing other instructions.
    - This are instructions SIO, TIO and HIO.
