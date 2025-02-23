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

# Milestones (Draft)

* Cerr outputs for all unsupported features
* Register system/type for passing dest reg - at the moment we don't distinguish x and t type which is big bad (why ADD1 fails)
* Other multiplication operations
* Full expression hierarchy - James
* Stack frame structure - how is size determined exactly
* Prepare code for other types/ variable sizes
* Local variables (scoping?)
* Structs
* Control blocks - if-else/while etc.
* Arrays local or global
* Enum
* Ternary operator - James
* Short circuiting logic
* Sizeof
* Pointers


Needs more who did what and collaboration stuff
