# C Compiler Project Management

# Overview

This file outlines the planning and completion of the C Compiler project. It includes design decisions, details of
milestones and related effort and some key design decisions.

# Environment Setup

- CLion / VSCode
- CMake project used alongside make project to allow IDE to interact with debugger, Valgrind and coverage in Docker and
  for contextual static analysis.
- Testing example.c successfully in both of our IDEs.

# Planning

We realised early in the project that planning every feature in detail was pointless before starting the project (we
didn't even know how many features were realistic to implement).

Instead, having only planned practical aspects such as managing the Git repository, we began by implementing (what we
thought was) a simple feature - mul.c. We both found this a great way to get over the steep curve of understanding how
the compiler would work. With this knowledge, some high-level decisions were made such as using a strictly-typed parser
and AST. That way, minimal refactoring was required to adhere to our standards before proceeding with other features.
These design decisions are listed in the section below, along with some feature-specific decisions.

Once these were decided, we created a list of milestones as we thought of features to add, assigning to one or both of
us, and listing dependencies. We had great visions of a Kanban board of milestones and associated comments, but with a
team of just 2 people, and very rapid development, this ended up being a fairly unstructured Markdown document and text
messages.

We found most edge cases and potential bugs to be obvious whilst writing code, so liberally included TODOs and extra
tests as features were developed so as not to lose track of unfinished/incorrect code.

Throughout the project, we tried to adopt an attitude of comparing options to solve problems and remain original, rather
than copying any other single compiler implementation.

# Key Design Decisions

- **AST Typing:** It was decided to implement a strictly typed AST and parser to allow nodes to have their own
  functions, and to minimise reusing inherited functions for differing purposes.
- **Context object:** It was decided to minimise the size of the Context object, preferring where possible to store
  node-specific information in AST nodes. The Context object was also split up into multiple classes in separate files
  such as `StackFrame` and `Bindings` for readability.
- **Naming/Style:** Functions classes and members were consistently named throughout the project. Names of AST nodes
  matched those in the provided parser grammar where possible.
- **Use of persistent registers:** It was decided to detect when register clobbering would occur rather than lazily and
  unnecessarily persisting all temporary registers.
    - Because the call chain of mutually recursive functions is unpredictable, it was decided to try and avoid functions
      all using the same persistent registers. Round robin allocation is used to heuristically minimise this.
- **Storing function definitions and forward declarations in context:** This was done to provide type information,
  necessary for passing arguments.
- **Return logic:** To minimise program size in the same way as GCC, return logic is deferred to the end of the
  function, and return statements simply load the return value and jump to this.
- **Function and array types:** These were challenging because they are part of the declarator not the type but
  constructing the type requires this information. This challenge was solved with the `BuildArray` and `BuildFunction`
  members.
- **Switch Cases:** Technically switch labels can be inside nested blocks, so need to be found recursively. Then, before
  emitting these blocks, the compare and jump RISC needs to be emitted. This was done with member functions
  `SetInSwitchScope()` and `GetSwitchLabelCasePairs()` exploiting the inheritance of our base `Statement` class.
- **External Declarations:** These require very different RISC so the parser constructs a subclass with different
  codegen. These are also stored separately in the Context object since information such as offset in memory is
  irrelevant.
- **Strings & Chars:** It was decided to implement much of the logic for these in the lexer, in particular the different
  types of char literal.
- **TypeSpecifier class:** The TypeSpecifier enum was extended to a class with static constant members to maintain
  backward compatibility and compiler warnings for missing switch cases to store information about complex types such as
  structs, enums, and arrays.
    - Initially arrays were stored in the context differently as a subclass of `Variable`, but this change superseded
      this.
- **Constexpr evaluation:** Initially, constexprs were evaluated at runtime for simplicity. However, compile-time
  evaluation was implemented in order to make enums simpler.
- **Stack Alignment:** Although it does not cause Spike to fail, it was decided to properly align all variable types
  within stack frames. This logic is delegated to the `Bindings` class.
- **Arithmetic Helpers:** Since arithmetic such as addition is used in multiple places, in particular special assigment
  operators, this codegen logic was extracted to the `Utils` namespace.
- **LHS context:** It was decided to include a boolean in the Context object, set when emitting the left hand side of an assignment. This was to "return" an address rather than relying on compile-time knowledge of complex variables such as struct members and pointees.


# Milestones, Dependencies, Assignee, Status

Dependencies are omitted where they have been implemented before the milestone was added.

Order is roughly but not strictly chronological.

| Milestone                                                 | Dependencies                | Assignee       | Status           |
|-----------------------------------------------------------|-----------------------------|----------------|------------------|
| Basic register allocation and type to avoid using strings |                             | James          | Completed Feb 24 |
| Multiplicative operator codegen                           | Register allocation         | Eliott         | Completed Feb 24 |
| Local Variables                                           |                             | James          | Completed Feb 26 |
| Scoping                                                   |                             | James          | Completed Feb 27 |
| Function calls                                            |                             | James          | Completed Feb 27 |
| Short circuiting test                                     |                             | James          | Completed Mar 3  |
| Selection statements                                      |                             | Eliott         | Completed Feb 28 | 
| Iteration statements                                      |                             | Eliott         | Completed Feb 28 |
| Multiple functions in a translation unit                  |                             | James          | Completed Feb 28 |
| Break & continue                                          |                             | Eliott & James | Completed Mar 8  |
| Stack frame structure                                     |                             | James          | Completed Mar 5  |
| Tests for break & continue                                | Break & continue            | Eliott         | Completed Mar 19 |
| Sizeof                                                    |                             | James          | Mar 10           |
| Tests for unary/postfix operators                         |                             | Eliott         | Mar 14           |
| Tests for null statements                                 |                             | James          | Mar 12           |
| Tests for nested switch                                   |                             | Eliott         | Mar 14           |
| Char implementation                                       |                             | James          | Mar 11           |
| Float/double implementation                               |                             | Eliott         | Mar 3            |
| Global variables                                          |                             | James          | Mar 9            |
| Tests for global variables of all types                   | Float/double implementation | Eliott         | Mar 16           |
| Tests for caller/callee alignment                         | Float/double implementation | Eliott         | Mar 16           |
| Arrays                                                    |                             | James          | Mar 12           |
| Assignment operators & tests                              |                             | James          | Mar 14           |
| Void type                                                 |                             | James          | Mar 14           |
| Tests for global/local arrays of different types          | Arrays                      | Eliott         | Mar 15           |
| Tests returning an array as pointer                       | Arrays                      | Eliott         | Mar 16           |
| Tests void function with and without return               |                             | Eliott         | Mar 15           |
| Tests for relational float operations                     |                             | Eliott         | Mar 17           |
| Tests for logical float operations                        |                             | Eliott         | Mar 17           |
| Tests to assign to global pointer                         |                             | Eliott         | Mar 15           |
| Tests for implicit boolean conversion                     |                             | Eliott         | Mar 15           |
| Tests for float equality                                  |                             | Eliott         | Mar 15           |
| Tests for multidimensional arrays                         |                             | James          | Mar 20           |
| Tests for unsigned int alias                              |                             | Eliott         | Mar 16           | 
| Tests for integral promotion                              |                             | Eliott         | Mar 16           |
| Tests for incrementing arrays and pointers                |                             | Eliott         | Mar 16           |
| Tests for chained assignment                              |                             | James          | Mar 18           |
| Ternary operator                                          |                             | Eliott         | Mar 16           |
| Pointer arithmetic                                        |                             | James          | Mar 15           |
| Tests multiplying dereferenced pointers                   |                             | James          | Mar 21           |
| Tests incrementing floats/doubles                         |                             | James          | Mar 18           |
| Enums                                                     |                             | James          | Mar 16           |
| Enum tests: arithmetic and switch cases                   |                             | Eliott         | Mar 19           |
| Tests initializing ints from enums                        |                             | Eliott         | Mar 19           |
| Structs                                                   |                             | James          | Mar 16           |
| Functions with many arguments and/or structs              |                             | James          | Mar 18           |
| Complex struct tests: nested, arrays and addressof        |                             | Structs        | Mar 19           |
| Test for typedef struct/enum                              |                             | James          | Mar 21           |

# Unfinished Features

* Arrays in structs as function parameters
* Register spilling - the current approach is heuristic and not always correct
* Multidimensional arrays as function parameters
* Edge cases where structs can half fit in argument registers
* Typedef context in the grammar - shadowing a global typedef with a local variable was very challenging
* Proper stack size calculation - the current approach is in a separate branch because it breaks the half-baked typedef
  logic.
* Optimising evaluation order of binary expressions to minimise register stores

