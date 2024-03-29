# DeSyL | DEductive SYnthesis Logic engine for synthesising computer programs with pointers

DeSyL is a deductive synthesis logic engine for generating computer programs to
implement specifications given in Synthetic Separation Logic. For instance, for
the specification

```
{true; <x, 0> -> a | <y, 0> -> b;}
swap(loc x, loc y)
{true; <x, 0> -> b | <y, 0> -> a;}
```

DeSyL synthesises the program

```c
void swap(loc x, loc y) {
    a_0 = *(x + 0);
    b_0 = *(y + 0);
    *(x + 0) = b_0;
    *(y + 0) = a_0;
}
```

DeSyL is an implementation of the synthesis method described in
[**Structuring the Synthesis of Heap-Manipulating
Programs**](https://dl.acm.org/doi/10.1145/3290385). For more details on the
specification syntax see [Specification](#specification).

The ability to synthesise programs with unrestricted pointer arithmetic is an
advantage of Synthetic Separation Logic. Additionally, the synthesis method
includes a restricted form of cyclic proof generation that allows synthesising
programs with recursive calls, like the following implementation of
`examples/listfree.sep` that deallocates a linked list rooted in `x`:

```c
void listfree(loc x) {
    if ((x == null)) {} else {
        v_0 = *(x + 0);
        nxt_0 = *(x + 1);
        free(x);
        listfree(nxt_0);
    }
}
```

## Compilation

Since the CMake configuration fetches all third party dependencies compilation
only requires creating the build files with `cmake -S . -B build`, and building
the repository with `cmake --build build`. Then the tool is run as
`build/apps/desyl specification-file` where `specification-file` is
user-provided or one of the files in `examples/`, e.g. `build/apps/desyl
examples/swap.sep` produces the example above.

## Usage

The basic synopsis of desyl is `desyl [-v | -g] <specification>`. Essentially,
to synthesise a program for a specification in the file
`specification-file.sep`, simply run `desyl specification-file.sep`, to get
verbose logging use the `-v` flag, and to perform guided synthesis use the `-g`
flag.

During guided synthesis DeSyL will repeatedly output the current goal and prompt
the user for the name of a rule. Once given, DeSyL will use said rule to perform
the next proof step, report the number of subderivations (multiple possible
applications of the rule that can backtrack) and proceed similarly for each
subderivation. To skip an unsuitable subderivation that should be backtracked
from, simply use `Skip`.

## Features

Aside from synthesising basic combinations of reading from/writing to pointers,
the DeSyL engine can:

- Unify variables in the pre- and postcondition.
- Perform basic logical inference with integers and sets
- Discover candidate logical conditions and synthesise branching with an if-else
  statement.
- Unfold conditional predicates and solve the subgoals separately.
- Synthesise the allocation and freeing of memory blocks.
- Generate recursive calls while guaranteeing termination.

See the examples folder for specifications leveraging each of these features.

## Specification

Specifications are of the form

```
{<pre_pure>; <pre_heap>;}
function_name(type arg1, ...)
{<post_pure>; <post_heap>;}
```

specifying a function with name `function_name` and parameters `arg1, ...`, each
with type `loc` (pointer) or `int` respectively. Then `{<pre_pure>;
<pre_heap>;}` is the "precondition", specifying the pure logical constraint and
the heap structure required before the function call, and `{<post_pure>;
<post_heap>;}` is the "postcondition", similarly specifying a pure logical
proposition and heap structure after the function is called.

`<pure>` is a boolean expression over the parameters `arg1, ...` and
possibly other variables, using the standard operators such as `&&`, `||`, `!`,
`+`, `*`, `<=`, etc. as well as:

- The logical implication operator `=>`.
- Set literals of the form `{elem, ...}`.
- Expressions with set operators:
  - `=i` for isomorphism/set equality.
  - `++` and `**` for union and intersection respectively.
  - `<s`, `<=s`, `>s`, and `>=s` for (proper) sub- and superset relations.

`<heap>` is given by a series of "heaplets" of one of three types:

- _Pointer declarations_ of the form `<x, i> -> v` where `x` and `v` are integer
  expressions/variables and `i` is always an integer literal representing offset.
- _Array declarations_ of the form `[x, n]` where `x` is an identifier and `n`
  is an integer literal representing length.
- _Predicate calls_ of the form `predicate_name(arg1, ...)` where
  `predicate_name` is an identifier and `arg1, ...` are also exclusively identifiers.

or `emp` as a placeholder. These heaplets are separated by the seperating
conjunction operator `|`.

Both the pure and heap specification are terminated by a `;`.

### Predicate calls

As indicated by the existence of predicate calls in the heap specification,
Synthetic Seperation Logic supports inductive predicates like

```
predicate lseg(loc x, set s) {
|  x == null    |=> {s =i {}; emp;}
|  !(x == null) |=> {s =i {v} ++ ss; [x, 2] | <x, 0> -> v | <x, 1> -> nxt | lseg(nxt, ss);}
}
```

which defines that `x` is a linked list containing the elements from the set
`s`. These predicates are always of the form

```
predicate predicate_name(type arg1, ...) {
|  guard |=> {<pure>; <heap>;}
...
}
```

specifying that if `guard` holds for `arg1, ...`, then `<pure>` and `<heap>`
must hold too. For predicates the possible types are `int`, `loc`, and `set`.
