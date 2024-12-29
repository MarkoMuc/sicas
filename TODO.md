# TO-DO

## Main goals

- Tokenizer:
    - [X] ADD ASM directives.
    - [X] ADD comments.
    - [X] ADD illegal symbols.
    - [X] ADD hexa numbers.
    - [X] ADD strings/chars.
    - [X] ADD registers.
    - [X] EXPAND debug:
        - [X] ADD better location information for debug.
    - [X] How should address offsets be handled?
    - [X] OPTIMIZE.
        - [X] Token generation.
        - [X] Token finding.
    - [X] TEST.
    - [X] Error messages.
    - [X] Add floating point numbers.
        - `f` prefix.
        - uses a `.` as a separator.
    - Fixes:
        - [ ] 0b is a valid token -> should not be.
        - [ ] `C'EOF'` should be valid.
        - [ ] Signed integers.
- Parser:
    - [X] Formats and addressing types.
    - [X] ADD token parsing:
        - PLUS and MINUS around LITERALS, legal or not?
        - Add additional instruction parsing.
    - [ ] Symbols can be of type constexpr!!!
    - [X] Hashmap for symbols:
        - [Tsoding.](https://www.youtube.com/watch?v=n-S9DBwPGTo)
        - [Hash Functions.](https://www.cse.yorku.ca/~oz/hash.html)
    - [ ] Literals.
    - [ ] EQU and ORG.
- [ ] Logger macros.
- [ ] Testing system.
- [ ] Characters/strings: Special characters, etc.

## Quality of life changes and expansion

- QOL:
    - [ ] Add better error reporting.

- Tokenizer:
    - [ ] Privileged instructions.
    - [ ] Only keywords can be lower or upper case.
- Build system:
    - [ ] Compilers.
    - [ ] Testing for different platforms.
- Use Arenas & better strings:
    - [Tsoding.](https://www.youtube.com/watch?v=3IAlJSIjvH0)
    - [Linear Allocation.](https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/)
    - [More C-Like Lists.](https://felipec.wordpress.com/2024/03/03/c-skill-issue-how-the-white-house-is-wrong/)
- Directives:
    - [ ] Add `embed` directive:
        - Embeds the file contents as BYTE.
        - Structure is `LABEL EMBED "path"`.
    - [ ] Add `export`:
        - Directive to export labels.
        - `export LABEL`.
- Refactors:
    - [ ] Refactor code, use more consts.
- Miscellaneous: 
    - [ ] Output as ASCII object code or normal object code.
    - [ ] Detect and inform user on base addressing.
