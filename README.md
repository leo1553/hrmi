## HRM Interpreter
Human Resource Machine (HRM) is a computer game that introduces a simple assembly-like visual programming language to solve logical problems.

HRM Interpreter is a simple HRM assembly-like language interpreter build with FLEX and BISON.

### Building
Building the project requires [cmake](https://cmake.org/download/), [flex](https://github.com/westes/flex) and [bison](https://www.gnu.org/software/bison/).

#### Windows
To build on Windows execute the build script `utils\build.bat`. The build binary will be located at `build\hrmi.exe`.

A [Dev-C++](https://www.bloodshed.net/) project is located at `src\HRM.dev`.

#### Linux
To build on Linux execute the build script `utils/build.sh`. The build binary will be located at `build/hrmi`.

A installation script for Debian dependencies is located at `utils/install.sh`.

### Language
The interpreted file has two parts: header and body.

#### Header
##### input
`input` is a required section that specifies the program's inbox. Each input must be separated by a colon. It is possible to execute the program with one or more input sets, separated by a semicolon.

Example:
- `input 1, 2, 3, 4, 5`
- `inputs 1, 2, 3, 4, 5; 1, 2, 3, 4, 6`

##### output
`output` is an optional section that specifies the program's expected output. Each output must be separated by a colon. If supplied, the amount of output sets must be the same of input sets.

Example:
- `output 0, 1, 2, 3, 4`
- `outputs 0, 1, 2, 3, 4; 0, 1, 2, 3, 5`

##### var
`var` is an optional section that specifies the program's starting variable values. Each variable declaration must be separated by a colon.

Example:
- `var 0 = 1`
- `vars a = 1, b = 2`

#### Body
The body is composed by one or more commands. Each command may or may not contain parameters. Each parameter must be separated by a colon.

##### HRM commands:
- [x] INBOX
- [x] OUTBOX
- [x] COPYFROM
- [x] COPYTO
- [x] ADD
- [x] SUB
- [x] BUMPUP
- [x] BUMPDN
- [x] JUMP
- [x] JUMPZ
- [x] JUMPN
- [ ] COMMENT

At the moment pointers are not implemented.

##### Custom commands:
For testing porpouses, some custom commands were added. They can be removed by undefining the `USE_CUSTOM_COMMANDS` definition on `src/interp.h`.

- [x] MUL
- [x] DIV
- [x] MOD
- [x] JUMPP
- [x] JUMPNZ
- [x] JUMPEQ
- [x] JUMPNEQ
- [x] JUMPLT
- [x] JUMPLTEQ
- [x] JUMPGT
- [x] JUMPGTEQ

#### Sample
```
-- Header
inputs
    1, 2, 3, 4, 5;
    1, 2, 3, 4, 6
outputs
    0, 1, 2, 3, 4;
    0, 1, 2, 3, 5
vars
    one = 1

-- Body
start:
    INBOX
    SUB one
    OUTBOX
    JUMP start
```

### TODOs
- [x] Parser
- [x] AST
- [x] Vars
- [x] Labels
- [x] Interpreter
- [ ] Pointers
- [ ] Text values

### License
The source code for this project is licensed under the MIT license, which you can find in the `LICENSE` file.
