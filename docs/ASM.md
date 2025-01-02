# Assembler

## Assembler directives

Basic:
- `.` -> Comment.
- `NAME START ADDR` -> Specify name and starting address for the program.
- `END ADDR` -> Indicate the end of the source program and (optionally) specify the first executable instruction in the program. Address can be a constant or a label.

Memory:
- `BYTE CONST` -> Generate character or hexadecimal constant, occupy as many bytes as needed to represent the constant.
- `WORD CONST` -> Generate one-word integer constant.
- `RESB CONST` -> Reserve the indicated number of bytes for a data area.
- `RESW CONST` -> Reserve the indicated number of words for a data area.

Constants:
- `C'CHAR'` -> At least a single character or a string.
- `X'HEX'` or `0xHEX` -> Integer constant in hexadecimal.
- `B'BIN'` or `0bBIN`-> Integer constant in binary.
- `D'DEC'` or `DECIMAL` -> Integer constant in decimal.
- `F'FLOAT'` or `0fFLOAT` -> Floating point constant.

Addressing:
- NOTE: 
    - Indexing cannot be paired with immediate or indirect addressing.
- `ADDR,X` -> Indexed addressing.
- `#(LABEL|CONST)` -> Immediate addressing.
- `@(LABEL|CONST)` -> Indirect addressing.
- `BASE (LABEL|value)` -> Informs the assembler what value is in the `B` register.
- `NOBASE` -> Informs the assembler the contents of the `B` register cannot be used for base addressing anymore.

Instructions:
- `+INSTR` -> Instruction in format 4.

Symbols/labels:
- `symbol EQU value` -> Assigns a constant value to a symbol.
- `ORG value` -> Indirectly assigns value to symbols. A `value` is a constant or an expression involving constant and previously defined symbols. When this directive is encountered during assembly, the assembler resets its location counter to the specified value. `ORG` with no `value` resets the location counter to normal.

Literals:
- `=CONST` -> Literal with the value of the constant. A `value` is an expression that can contains `+,-,*,/`, symbols, constants and special terms like `*`.
- `LTORG` -> Dumps the **literal pool**, containing all the literals since the last `LTORG`, at the location where the directive was encountered.

Extra:
- `EOF` -> End of file value 0x454F46
- `*` -> Refers to the current value of the location counter. Can be used in-place of a constant value.

## Assembler Mnemonic

- `A,B,X,...` -> Registers.

## Structure assembled object code

Object code consists of three main parts, the header, text record and end record.

Header structure:
    - Column 1: `H`.
    - Column 2-7: Program name.
    - Column 8-13: Starting address of object program (in hexadecimal).
    - Column 14-19: Length of object program in bytes (in hexadecimal).

Text record structure:
    - Column 1: `T`.
    - Column 2-7: Starting address for object code in this record (hexadecimal).
    - Column 8-9: Length of object code in this record in bytes (hexadecimal).
    - Column 10-69: Object code, represented in hexadecimal (2 columns per byte of object code).

End record structure:
    - Column 1: `E`.
    - Column 2-7: Address of first executable instruction in object program (hexadecimal).
