# Bytecode of Xue

The Xue program will be compiled into a specific binary format of bytecode before being executed. This documentation will describe the binary format and design objective of it in detail.

## Objective

The objective of the Xue bytecode is as small as possible.

And once the Xue language has its own bytecode, it can be very portable and run everywhere e.g. Linux, macOS, Android.

Since there's no optimizer in this project, The goal about size will be hard to achieve. But I will try my best to compress the bytecode output size.

## File Format

The suffix of Xue language bytecode file is `.xbc`.

Here is the data structure of Xue bytecode file in C.

```c
typedef Instruction unsigned int;
typedef Byte unsigned char;
struct Xue_Bytecode_File {
  int magic_signature;   // We use 4 ASCII characters to represent its
                         // file signature, the content of it is XUE!
  int file_version;      // Here we use low 16 bits for patch version
                         // Highest 8 bits for major version and second
                         // high 8 bits for minor version.
  unsigned int constant_pool_size;
  Byte constant_pool_data[constant_pool_size];

  int constant_pool_descriptor_count;
  Constant_Pool_Descriptor constant_pool_descriptors[constant_pool_descriptor_count];

  int foreign_function_count;    // Indicating the number of foreign
                                 // functions used in this file.
  Foreign_Function_Descriptor foreign_function_descriptors[foreign_function_count];

  int global_data_count;
  Global_Data_Descriptor global_data_descriptors[global_data_count];

  int instruction_count; // The number of instructions in this file.
  Instruction instructions[instruction_count];
};
```

The bytecode file starts with four ASCII characters `'XUE!'`, then follows four bytes, meaning the version of this bytecode file. And then we have the size of constant pool which is used to store all constant values used in the whole Xue program, and the data array of constant pool. Then the number of foreign functions used in this bytecode file. Then we have foreign function descriptors to descrbe the information of foreign function such as the foreign function name, the library name etc. Before we read in the instructions, the virtual machine have to know the number of global variables used in this bytecode file, and parse the type and data of each global variable. At last, we reached the instruction section, where the vm reads in the number of instructions and the instruction array.

## Instruction Format

Each instruction in Xue bytecode is a 32-bit unsigned integer, and we use higher 6 bits to represent its operation code and low 26bits to represent its address code.

Currently, there're 20 operation codes, the value of them are:

- NOP: 0x000000

- ADD: 0x000001

- SUB: 0x000010

- MUL: 0x000011

- DIV: 0x000100

- MOD: 0x000101

- VAR: 0x000110

- PUSH: 0x000111

- POP: 0x001000

- JMP: 0x001001

- CMP: 0x001010

- JGE: 0x001011

- JLE: 0x001100

- JGT: 0x001101

- JLT: 0x001110

- JEQ: 0x001111

- JNE: 0x010000

- CCALL: 0x010001

- CPY: 0x010010

- EXIT: 0x010011

The address code of Xue bytecode can be a 26-bit signed or unsigned number. Whether it is signed is based on the type of operation code.

- The `NOP` opcode, the remaining 26-bits is reserved.

- The `ADD`, `SUB`, `MUL`, `DIV` and `MOD` opcode, the address code part can be the offset of the top of the stack, (i.e. the address of local variable) a 26-bit signed integer, a pointer pointing to a constant floating-point number, a pointer pointing to the string constant pool or a pointer pointing to the global variable. (i.e. the address of a global variable)

- `VAR` opcode, the name of both local and global variable is just a compile-time variable, so the whole address code is the same as the previous five mnemonics.

- `PUSH` and `POP` opcode, the address code is also the same as the previous one. It either stores a 26-bit signed integer or the address of any other basic data type.

- `CMP` opcode is a bit different than previous mentioned opcodes, it uses high 1 bit to indicate whether the content of address code is a plain address or nothing. So the available address is smaller than other opcodes. We highly recommend you to store the comparison result into the local variable.

- `JMP`, `JLE`, `JGE`, `JGT`, `JLT`, `JEQ`, `JNE` opcode, the address code part stores the address of start of instruction the given label points to.

- `CCALL` opcode, uses the address code as the foreign function index, which means that the whole Xue program can use 2^26 foreign functions.

- `CPY` and `EXIT` opcode does not use the address code part, so we can consider the address code part as reserved for future :) 
