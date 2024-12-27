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
    - [ ] `C'EOF'` should be valid
- [ ] Parser:
    - [ ] Formats and addressing types.
    - [ ] ADD token parsing
        - PLUS and MINUS around LITERALS, legal or not?
        - Add additional instruction parsing
    - If label found, then get next token and perform analysis
    - How do I handle this:
    https://github.com/jurem/SicDemos/blob/80ef4102e821da3694187aa8c6b1cc963fadc51d/graph-screen/space-shooters/space_shooters.asm#L882
    - [ ] Symbols can be of type constexpr!!!
- [ ] Add better error reporting
- [ ] Logger macros
- [ ] Testing system
- [X] Hashmap for symbols
    - [Tsoding](https://www.youtube.com/watch?v=n-S9DBwPGTo)
    - [Hash Functions](https://www.cse.yorku.ca/~oz/hash.html)


## FAR FUTURE

- [ ] TOKENIZER 
    - [ ] Should the tokenizer change regs to numbers
    - [ ] Privileged instructions: HIO, LPS
- [ ] Build system
    - Compilers
    - Testing for different platforms
- [ ] Use Arenas & better strings?
    - [Tsoding](https://www.youtube.com/watch?v=3IAlJSIjvH0)
    - [Linear Allocation](https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/)
    - [More C-Like Lists](https://felipec.wordpress.com/2024/03/03/c-skill-issue-how-the-white-house-is-wrong/)
- [ ] Add `embed` directive
    - Embeds the file contents as BYTE
    - Structure is `LABEL EMBED "path`
- [ ] Add `export` directive to export labels
- [ ] Refactor code, use more consts
