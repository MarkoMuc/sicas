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


- LDS #11 -> Immediate addressing, where 11 is used directly as op value
- J @RETADR -> Indirect addressing, fetch value then use it as an address
- +JSUB WRREC -> Format 4 instruction
- MAYBE WORD -> EOF
- Assembler directive BASE
