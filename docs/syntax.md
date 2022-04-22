# Syntax of Xue

Note: The Xue language follows the syntax of GNU AT&T.

## Basic Operation

The Xue language can declare variable and refer the declared one, by using the following instruction.

```
  VAR $name $value      ; Declare a variable whose name is $name, and the value of it is $value
  PUSH $name            ; Push the value of the given variable $name
```

Because this virtual machine is a stack machine, the programmer can only push values onto the stack and pop value from the stack. You can use the following instruction to do such operations.

```
  PUSH $value           ; Push the value $value onto the stack
  POP $variable         ; Pop the top of the stack and send to the given variable
```

This language also supports basic arithmetic operation e.g. addition, subtraction, multiplication, division and modulo. You use the instruction below.

```
  ADD $variable        ; Pops two integer or float-like values from the stack
                       ; and push the result to the given variable
  
  SUB $variable        ; Pops two integer or float-like values from the
                       ; stack, push the result of subtraction into the
                       ; given variable. Note that the first value poped
                       ; from the stack will be the minuend while the
                       ; second poped out value will be the subtrahend.
```



## Comment

The Xue language uses ';' as the symbol of line comment, and there's no block comment in this language. You can use line comment like this.

```
  PUSH @"Hello World!"    ; Push a string onto the stack
  CALL $printf            ; Call the printf function in Xue std
  RET 0                   ; Push 0 as the return value of this program.
```
