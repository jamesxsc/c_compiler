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
- Switch challenges:
  - We need to recursively search for case labels and generate comparisons and jumps (since they can technically be inside while/if/{} etc). This was done with a member SetInSwitchScope() and GetSwitchLabelCasePairs()
  - We also need to emit the RISC for the condition constant expressions at the start for comparisons. This was achieved by using shared pointers. It's not ideal but it's better than a map of reference_wrappers.
- External/global variable and function (forward) declarations
  - We need very different codegen for these compared to function definitions and local variables. The solution was to inherit Declaration in a new type ExternalDeclaration.
  - These are also stored separately in the context since fields such as offset are not relevant.
- Arrays
  - Arrays were complicated because they create a lot of new cases to consider across the codebase e.g. global vs local, and switching on the underlying type for each case.
  - We decided to subclass the Variable struct to Array to support arrays in the Bindings class. This was also used in a map for globals.
  - * Later deleted due to modified TypeSpecifier class!
  - Because the Array struct required information from different node types, we pass the type to a BuildArray virtual method on (Array)Declarator which is consistent with how we build and store Function structs. 
  - Parameter list wasn't implemented with arrays initially because it's a bitch.
- Strings and chars required changes to the lexer to handle edge cases such as escaped characters and ignoring the string quotes.
- To save instructions, some of the common return logic is deferred to the end of the function and jumped to using the return label in the context.
- TypeSpecifier
  - TypeSpecifier was changed prior to implementing structs and enums. Instead of an enumerated type, a class was used to support holding underlying types of pointers and aggregates. The class was implemented such that missing switch case warnings still worked.
- GetType on expressions is designed to always return the type of the actual operation (return) being performed to easily know whether to use a floating-point register and switch on the type.
  - TODO we return int from logical, is that ok for storing result in a char? if thats allowed anyway
- Constexpr evaluation. I didn't want to do it but enums would be super ugly without it.
- Round robin persistent register allocation to reduce bashing
- Correctly aligned stack (local variables) even though it doesn't fail in the simulator (spike)


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
* ~~Tests for break, switch, continue~~
* ~~Tests for unary/postfix operators (pointers done)~~
* ~~Tests for null statements~~
* ~~Test for nested switch~~
* ~~Tests for global floats doubles and pointers if there aren't already any~~
* ~~Test for calling a function from our test with several arguments of different sizes e.g. char, float, int and calling external~~
  * ~~I want one with just float, double as well to check alignment, both calling from and being called by our compiled code~~
* ~~Tests for global/local arrays of different types~~
* ~~Test for accessing a pointer when the original variable is out of scope (if this is possible without malloc and we think this will be tested; or just when the underlying variable has been shadowed)~~
* ~~Test returning an array from a function as pointer~~
* ~~Test void with and without return~~
* ~~Test for relational operators on float and double~~
* ~~Test for logical operators on float and double~~
* ~~Test to assign to global ptr (not dereferenced)~~
* ~~Test for if (f) and while/for where f is a float or double to be implicitly converted. Same for pointer and char~~
* ~~Test for multidimensional arrays~~
* ~~Test for float equality e.g. f == 2.0f or d == 2.0d~~
* Test using "unsigned int" type
* Tests for integral promotion
* ~~Test for sizeof (char + char) should be 4 because of promotion~~
* Test for ++ and = x + 1 on arrays (like int a[] not a[index]) and pointers (similar to pointer/arithmetic) and member incr on struct
* ~~Test for chained assignment~~
* Check if we need nops anywhere/load delay slots
* ~~Ternary operator - Eliott~~ Mar 16
* Test with multiply where right hand is dereferenced pointer e.g. int *p = &x; int y = 2 * *p;
* ~~Float/double unary and postfix operators AND for pointers/array identifier e.g. float++ --float float[0]++ --float[0], globals and local ideally. and addressof minus difference, good trick for checking ops on ptrs/addresses~~
* Char operation byte masking where applicable
* Enum tests: arithmetic e.g. adding multiplying enums with each other and ints; scoped enums; enum in switch case; initialize global int from enum identifier
* Struct tests: various including nested structs and arrays of structs (maybe will get time for implementing this), address of struct
* ~~Stack alignment uhh vars~~
* Test for calling external function returning a pointer
* Test for array param not as pointer and array if we haven't already
* Test case for loads of recursion that needs persistent registers
* ~~Floats Eliott & James~~ Mar 10
* ~~Char type - James~~ Mar 11
* ~~Global variables - James~~ Mar 9
* ~~Arrays local or global - James~~ Mar 11
* ~~Char constant (override same methods as int but accept char or int in constructor; needs work in lexer also) - James~~ Mar 13
* Function calls with many arguments and test
* Register spilling and test - just be transactional, pick a register, store it and unstore when it's freed? our current alloc/dealloc isn't watertight but this should work
* ~~Void type/functions~~ Mar 14
* ~~Assignment operators impl and testing - James~~ Mar 14
* Structs
* ~~Enum~~ Mar 16
* ~~Sizeof~~
* ~~Pointer arithmetic~~ Mar 15 


Needs more who did what and collaboration stuff and timelines
