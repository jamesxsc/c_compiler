# C Compiler Project Management

[//]: # (TODO rearrange this garbage and rename my headers)

# First Steps

## Environment Setup
- CLion
- IDE with debugger, Valgrind, coverage, syntax highlighting; running individual tests quickly
- CMake project
- Testing example.c successfully

## Planning

[//]: # (TODO Separate out the design notes/choices at some point)
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
- Storing functions in context - storage logic abstracted in function declarator. Type provided to build function when called by function definition or declaration since exact type grammar differs.

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
* ~~Break/switch/continue - James~~
* ~~Stack frame structure - how is size determined exactly - see related comments~~
* Tests for break, switch, continue
* Tests for unary/postfix operators (pointers done)
* Tests for null statements
* Tests for global floats doubles and pointers if there aren't already any
* Test for calling a function from our test with several arguments of different sizes e.g. char, float, int
* Ternary operator
* Float/double unary and postfix operators
* ~~Floats Eliott & James~~ Mar 10
* ~~Char type - James~~ Mar 11
* Char constant (override same methods as int but accept char or int in constructor; needs work in lexer also)
* Float int double char conversion pain - but implicit casting isn't tested apparently
* Cerr outputs for all unsupported features
* ~~Global variables - James~~ Mar 9
* Function calls with many arguments and test
* Arrays local or global
* Register spilling
* Void type/functions
* Assignment operators impl and testing
* Structs
* Enum
* Sizeof
* Pointer arithmetic - loads of places this needs to be added


Needs more who did what and collaboration stuff and timelines
