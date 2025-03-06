# C Compiler Project Management

[//]: # (TODO rearrange this garbage and rename my headers)

# First Steps

## Environment Setup
- CLion
- IDE with debugger, Valgrind, coverage, syntax highlighting; running individual tests quickly
- CMake project
- Testing example.c successfully

## Planning
- It was felt that in order to make an effective plan, it was necessary to work with the codebase for a while to understand it.
- (Stupidly?) decided to try and get mul working first which involved implementing functions, and several layers of the expression AST.
- However, this revealed numerous design decisions that were required that helped the planning process greatly.
  - E.g. the challenges of types and how the structure of the AST types/inheritance affects writing code.
  - What information is required in the Context object.
- Naming consistency - quickly became confusing without adopting a consistent style/approach.
  - With exceptions like function declarator - but these are clear if you read the parser
    - Good example to discuss as declarator handles just variable declarations and function declarations
- There may be things that get untidy - but prioritise making them neat as it makes future problems/features easier to solve/develop.
- Trying to stay original - neither of us familiar with other C compilers.
- Deciding when to use a persistent register:
  - Option 1: Persist in the callee all temporary registers - disbanded because it is super slow
  - Option 2: Detect clobbering e.g. right hand evaluation contains a function call - this is the current approach
    - Future improvements (if we had time after finishing other core features)
      - Optimising order of evaluation
  - Option 3: Copy register context state through function calls - won't work for recursion
- We'll see how we go but probably accept some program size inefficiencies such as still having a jump after a return statement in a control block.
- Floats and pointers before structs.

# Milestones (Draft)

* ~~Register system/type for passing dest reg - at the moment we don't distinguish x and t type which is big bad (why ADD1 fails) - James~~
* ~~Other multiplication operations - Eliott~~
* ~~Full expression hierarchy - James~~
* ~~Local variables - James~~
* ~~Context make unique - James~~
* ~~Scoping - James~~
* ~~Function calls - James~~
* ~~Short-circuiting logic tst - James~~
* ~~Selection and iteration statements - if-else/while etc. - Eliott~~
* ~~Multiple functions in a translation unit/ file - James~~
* Break/switch/continue
* Cerr outputs for all unsupported features
* Prepare code for other types/ variable sizes
* Global variables
* Stack frame structure - how is size determined exactly - see related comments
* Arrays local or global
* Register spilling
* Void type/functions
* Assignment operators impl and testing
* Floats
* Structs
* Enum
* Ternary operator
* Sizeof
* Pointers


Needs more who did what and collaboration stuff and timelines
