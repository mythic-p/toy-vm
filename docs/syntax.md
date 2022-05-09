# Syntax of Xue

**English** | [简体中文](./syntax.CN.md)

Note: The Xue language follows the syntax of GNU AT&T.

## Basic data types

Every physic or virtual machine must have its atomic data unit, while most of them are byte, 1 byte consists of 8 bits. In the Xue virtual machine, the basic type is also the byte. Other basic types like string, floating number is also made up of byte. The only difference is that these basic types have its own structure while byte have no data structure. Here are some examples on how to use these basic types.

```nasm
  VAR myint, 12          ; We declared a variable whose name is myint
                         ; and push the value 12 to it and push the
                         ; variable onto the top of the stack.
  
  VAR myfloat, 3.0f      ; myfloat is the second variable that I pushed
                         ; onto the stack. It is a single-precision
                         ; floating number.
  
  VAR mychar, 'A'        ; mychar is a char type variable, it actually is
                         ; a one-byte sized variable. Until now, only
                         ; ASCII character is supported, so the value of
                         ; 'A' in this example should be 97.
  
  VAR mystring, @"Hello" ; mystring is a string type variable, the value
                         ; of this one will be placed on the global data
                         ; area, and the virtual machine will have an
                         ; entry to memorize the address of the string.
```

Should notice that a string literal always start with a `@` symbol and is surrounded by the `"`.

If you want to use a variable in the mnemonic, there're two ways to do this.

1. You can use the name of the local variable directly.

2. If you want to use a global variable, you must prefix it with a `$` symbol, a `$` means that the identifier you use may be a global variable or a foreign function.

## Mnemonics and Pseudo instructions

The Xue language can declare local variables and refer the declared one, you can use the following instruction.

```nasm
  VAR $name $value      ; Declare a variable whose name is $name, and 
                        ; the value of it is $value
  PUSH $name            ; Push the value of the given variable $name onto
                        ; the stack.
```

If you want to declare a global variable, you must declare it in the global variable section, where it starts with a pseudo instruction `global`. And the way to tell the compiler that the global variable declaration is end is that set a named label. Should notice that all pseudo instructions start with a `.`, then the instruction name. The example of how to declare global variable is as follows.

```nasm
.global:
  VAR a 0              ; Declare a global variable named after a with
                       ; initialized value 0.
  VAR b 0f             ; Declare a global floating point variable b with
                       ; value 0.0f
label main:
                       ; Here is the end of the global variable
                       ; declaration.
```

Because this virtual machine is a stack machine, the programmer can only push values onto the stack and pop value from the stack. You can use the following instruction to do such operations.

```nasm
  PUSH $value           ; Push the value $value onto the stack
  POP $variable         ; Pop the top of the stack and send to the given variable
```

This language also supports basic arithmetic operation e.g. addition, subtraction, multiplication, division and modulo. You use the instruction below.

```nasm
  ADD $variable        ; Pops two integer or float-like values from the stack
                       ; and push the result to the given variable
  
  SUB $variable        ; Pops two integer or float-like values from the
                       ; stack, push the result of subtraction into the
                       ; given variable. Note that the first value poped
                       ; from the stack will be the minuend while the
                       ; second poped out value will be the subtrahend.
  
  MUL $variable        ; Pops two integer or float-like values from the
                       ; stack, push the result of multiplication into
                       ; the given variable.
  
  DIV $variable        ; Pops two integer or float-like values from the
                       ; stack, push the result of division into the given
                       ; variable. Note that the first poped out value is
                       ; the dividend while the second one is the divisor.
  
  MOD $variable        ; This instruction is similar to the DIV one, the
                       ; only difference is that the result of modulo
                       ; will be sent into the given variable.
```

Control flow structure is also the basic part of Xue language. You can use label to mark the start of a code block and use jump instruction to implement high-level programming language clause like `if`, `if-else` and `else`.

```nasm
  label lbl:              ; This is a label called lbl, you can use
                          ; branch instruction to jump to the start of
                          ; this label.
  
  JMP $label              ; Jump to the first instruction of the given
                          ; label called $label
  
  CMP [$variable]         ; Pops two integer or float-like value, compare
                          ; them and push the result of comparison into
                          ; the given variable.
  
  JGE $label              ; Pops a integer value from the stack as
                          ; comparison result flag, if the flag is greater
                          ; or equal, then jump to the given label.
  
  JLE $label              ; Similar to the JGE instruction, but only when
                          ; flag is less than the given label will be
                          ; jumped to.
  
  JGT $label              ; Similar to the JLE instruction, except jump
                          ; condition is that the flag is greater than or
                          ; equal.
  
  JLT $label              ; Similar to the JGT instruction, other than
                          ; the jump condition is that the flag is marked
                          ; as less than.
  
  JEQ $label              ; Similar to the previous one, but this one
                          ; jumps to the given label only when the flag
                          ; is equal.
  
  JNE $label              ; Similar to the JEQ instruction, but this
                          ; instruction will jump to the given label
                          ; until the flag is not equal.
```

Except the mnemonics about control flow structure and arithmetic operations, the Xue language also has the ability to call the C-ABI compatible functions from static library by using stdcall or cdecl calling convention. Here are some mnemonics that you may use in your Xue program.

```nasm
  CCALL $function        ; Call a C-ABI compatible function, the compiler
                         ; will collect the information about callee
                         ; during compile-time, so do not worry about it,
                         ; just push the arguments onto the stack and
                         ; call the function you want, the return value
                         ; will be pushed onto the stack once the function
                         ; has done it's job.
```

But there's no concept about user-defined function in Xue, so we will not provide mnemonic like `CALL`, `RET` and so on.

Please note that the Xue language is developed in 2022, so there're  some modern features in Xue language.

- The `CPY` instruction, will pop out top of the stack and copy the popped out value and then push the copied value twice to the stack.

- The `EXIT` mnemonic will interrupt the execution of the program, you can use this mnemonic to finish the program in advance.

- The `import` pseudo instruction, this one can import one or more existing Xue source files into the  program, if you import same file multiple times, the compiler will consider these duplicated imports as one import.

- The `export` pseudo instruction. The Xue language can make some given variables put at global data area, which means that they can be used in everywhere once they have been initialized with explicit value.

## Comment

The Xue language uses ';' as the symbol of line comment, and there's no block comment in this language. You can use line comment like this.

```nasm
  PUSH @"Hello World!"    ; Push a string onto the stack
  CCALL $printf           ; Call the printf function in libc.
```

## Examples

Here are some examples to show how to use Xue to write some classic computer programs.

### Hello World in Xue

```nasm
  PUSH @"Hello World in Xue!"
  CCALL $printf
```

### Fibonacci in Xue

```nasm
.global:
  VAR f1 0
  VAR f2 1
  VAR count 2
label fibonacci_init:
  PUSH @"We will output the first 20 items of Fibonacci.\n1:1\n"
  CCALL $printf
  JMP fibonacci_check
label fibonacci_check:
  PUSH $count
  PUSH 21
  CMP
  JLT fibonacci_body
  PUSH @"Here is the end of the Fibonacci example!"
  CCALL $printf  
label fibonacci_body:
  VAR f3 0
  PUSH $f1
  PUSH $f2
  ADD f3
  PUSH $f2
  PUSH 0
  ADD  $f1
  PUSH $f3
  PUSH 0
  ADD  $f2  
  PUSH @"%d:%d\n"
  PUSH $count
  PUSH f3
  CCALL $printf
  PUSH $count
  PUSH 1
  ADD $count
  JMP fibonacci_check
```
