# Assembler

## Assembler directives

- `START`, specifies name and starting address for the program.
    - `IDENTIFIER? START ADDRESS`
- `END`, indicates the end of the source program and specifies the first executable instruction in the program.
    - `END ADDRESS?`
- `BYTE`, generate character or hexadecimal constant, occupying as many bytes as needed to represent the constant.
    - `BYTE (CHAR | HEX)`
- `WORD`, generate one-word integer constant
    - `WORD NUMBER`
- `RESB`, reserves the indicated number of bytes for a data area.
    - `RESB (NUMBER | STRING)`
- `RESW`, reserves the indidactd number of words for a data area.
    - `RESW NUMBER`


- `.` are comments.

- Numbers:
    - Immeditate operand is denoted with `#`
    - BINARY:
        - B'NUMBERS' -> BIN 
        - 0bNUMBERS -> BIN
    - HEX:
        - X'NUMBERS'-> HEX
        - 0xNUMBERS -> HEX
    - FLOATS:
        - F'' -> FNUM
        - 0fNUMBERS-> FNUM
    - Decial numbers -> NUMS


- #11 -> Immediate addressing, where 11 is used directly as op value
- @RETADR -> Indirect addressing, fetch value then use it as an address
- `+` -> Format 4 instruction
- MAYBE WORD -> EOF
- Assembler directive BASE

- base, pc and direct addressing can be paired with indexed addressing.
- indexing cannot be paired with immediate or indirect addressing.
- 
 
## Assembler directives

- `.` -> Comment.
- `,X` -> Indexed addressing.
- `name START address` -> Specify name and starting address for the program.
- `END address` -> Indicate the end of the source program and (optionally) specify the first executable instruction in the program. Address can be a constant or a label.

Memory:
- `BYTE` -> Generate character or hexadecimal constant, occupy as many bytes as needed to represent the constant.
- `WORD` -> Generate one-word integer constant.
- `RESB` -> Reserve the indicated number of bytes for a data area.
- `RESW` -> Reserve the indicated number of words for a data area.

Constants:
- `C'char'` -> At least a single character or a string.
- `X'hex'` -> Integer constant in hexadecimal.
- `B'bin'` -> Integer constant in binary.
- `D'dec'` -> Integer constant in decimal.
- `decimal` -> Integer constant in decimal.

Addressing:
- `#(label|constant)` -> Immediate addressing.
- `@(label|constant)` -> Indirect addressing.
- `BASE (LABEL|value)` -> Informs the assembler what value is in the `B` register.
- `NOBASE` -> Informs the assembler the contents of the `B` register cannot be used for base addressing anymore.

Instructions:
- `+INSTR` -> Instruction in format 4.

Symbols:
- `symbol EQU value` -> Assigns a constant value to a symbol.
- `ORG value` -> Indirectly assigns value to symbols. A `value` is a constant or an expression involving constant and previously defined symbols. When this directive is encountered during assembly, the assembler resets its location counter to the specified value. `ORG` with no `value` resets the location counter to normal.

Literals:
- `=CONST` -> Literal with the value of the constant. A `value` is an expression that can contains `+,-,*,/`, symbols, constants and special terms like `*`.
- `LTORG` -> Dumps the **literal pool**, containing all the literals since the last `LTORG`, at the location where the directive was encountered.

Extra:
- `EOF` -> End of file value 0x454F46
- `*` -> Refers to the current value of the location counter. Can be used in-place of a constant value.

## Assembler Mnemonic

- `CLEAR reg` -> Translates to `LD{reg} 0` or translates to `LD{reg} #0`
- `A,B,X,...` -> Registers.
