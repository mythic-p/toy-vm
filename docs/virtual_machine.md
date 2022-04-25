# The design of Xue virtual machine

This documentation is used for describing the whole procedure of how to design the Xue stack virtual machine.

## Structure

The Xue virtual machine is made up of four parts. The all four parts are allocated on the heap, only the temporary variables are allocated on the stack. There're constant data area, global variable area, instruction storage area and the virtual machine stack. Except the data area shown above, the VM also has a program counter that pointing to the address of the next instruction that is to be executed and a stack pointer pointing to the address of the stack where it is ready to be pushed with value.

## Input

The virtual machine must accept an input file which satisfies the binary format of Xue bytecode, the input is a path that can be either relative or absolute.

## States of the Xue VM

There're four states in the whole procedure of executing a Xue bytecode: Initialization, validating, executing, finalization. Before reading the given bytecode file, the VM has to initialize the instruction storage area and the virtual machine stack. By default, the VM allocates 1MB to them, and this setting can be changed by setting commandline argument. After the initialization phase, the VM will finally read the input bytecode file. It will try to validate the input file, if it cannot pass the file format check, the VM will throw an error information to the console. After the validation, the VM can execute the bytecode file now. The VM will go to the finalization phase when the execution of the input file is ended. In this phase, the VM will first release the memory space of the virtual machine stack, and then the instruction storage area, then the global variable area and finally the constant data area. When all four parts are released, the VM process will be killed by sending return value 0 to the operating system.

## Simple implementation of the VM in C

```c
typedef Instruction unsigned int;
typedef Byte unsigned char;
typedef u32 unsigned int;
struct Xue_VM {
  Instruction* program_counter;
  Byte* stack_pointer;

  Instruction* instructions;
  u32 instruction_count;
  
  Byte* constant_data;
  u32 constant_data_size;

  Global_Variable* global_variables;
  u32 global_variables_count;

  Byte* the_stack;
};
```
