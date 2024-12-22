# TODO

- [X] Tokenizer:
    - [X] ADD ASM directives
    - [X] ADD comments
    - [X] ADD illegal symbols
    - [X] ADD hexa numbers
    - [X] ADD strings/chars
    - [X] ADD registers
    - [X] EXPAND debug:
        - [X] ADD better location information for debug
    - [X] How should address offsets be handled?
    - [X] OPTIMIZE
        - [X] Token generation
        - [X] Token finding
    - [X] TEST
    - [X] Error messages
    - [X] Add floating point numbers.
        - `f` prefix
        - uses a `.` as a seperator
    - [ ] Add labels?
    - [ ] 0b is a valid token -> should not be
- [ ] Parser:
    - [ ] ADD token parsing
        - PLUS and MINUS around LITERALS, legal or not?
        - Add additional instruction parsing
    - [ ] Add better error reporting

- [ ] Logger macros
- [ ] Testing system

## FAR FUTURE

- [ ] TOKENIZER 
    - [ ] ADD preprocessing
    - [ ] Should the tokenizer change regs to numbers
    - [ ] Privileged instructions: HIO, LPS
- [ ] Build system
    - Compilers
    - Testing for different platforms
