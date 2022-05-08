# Xue语言字节码

[English](./bytecode.md) | **简体中文**

使用Xue语言编写的程序在执行之前会被编译成特定格式的字节码。本文档将会详尽地介绍这种特殊格式的字节码，以及其设计目标。

## 目标

Xue语言字节码的设计目的是充分利用每一个比特位，使得输出的字节码尽可能的小。

如果Xue语言有了自己的字节码格式，那就意味着Xue语言拥有了便携性这一特点。接下来它将不仅可以运行在Windows平台，还可以运行在macOS, Linux, Android等操作系统下。从而实现了跨平台！

不过，因为本项目没有类似字节码优化器这样的部件，所以想要达到目标还是有一定难度的。尽力而为吧。

## 文件格式

Xue语言字节码的文件后缀名是`.xbc`，也就是`Xue ByteCode`。

以下是Xue语言字节码的C语言描述：

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

字节码文件以4个ASCII码字符`XUE!`开始，紧接着是4个字节，表示该字节码文件的版本号。然后是常量池的大小（字节）以及常量池的具体数据。然后是外部函数描述符的数量，以及每个描述符的二进制表示。接着是全局变量的数量，以及每个全局变量的类型和具体值。最后，是在该字节码文件中的指令数量，以及具体的指令数据。

## 指令格式

在Xue语言字节码中，每条指令都是由32个比特位组成。其中，我们使用高6位来表示每条指令的操作码，使用余下的26位表示其地址码。

目前Xue语言总共有19条指令，每条指令对应的操作码如下。

- NOP: 0x00

- ADD: 0x01

- SUB: 0x02

- MUL: 0x03

- DIV: 0x04

- MOD: 0x05

- PUSH: 0x06

- POP: 0x07

- JMP: 0x08

- CMP: 0x09

- JGE: 0x0A

- JLE: 0x0B

- JGT: 0x0C

- JLT: 0x0D

- JEQ: 0x0E

- JNE: 0x0F

- CCALL: 0x10

- CPY: 0x11

- EXIT: 0x12

地址码可以是一个26位的有/无符号整数字面量，也可以是一个地址或者索引，具体取决于操作码。

- `NOP` 操作码，不使用余下的26位地址码。

- `ADD`, `SUB`, `MUL`, `DIV`和`MOD`操作码，不仅需要倒序压入两个值，还需要压入一个标志变量，用来表示压入值的类别。而这些操作码的地址码可以是26位整数、常量池索引、局部变量的偏移值、字符串的索引或者是全局变量索引。

- `PUSH`和`POP`操作码，其地址码的内容同上。

- `CMP`指令，使用地址码中的高一位来判断是否有值，如果有地址或者其他数据则为1，否则为0.因此，CMP指令的地址码最多只能使用25位地址。这里建议将比较结果保存到局部变量上，而不是指定一个虚拟地址。

- `JMP`, `JLE`, `JGE`, `JGT`, `JLT`, `JEQ`, `JNE` 指令，其地址码存储的是从字节码文件第一跳指令开始的偏移量，也就是第N条指令的位置。

- `CCALL`指令，将地址码视为外部函数索引，这样也就是说，你可以在一个字节码文件中最多使用2^26次方个外部函数。

- `CPY`和`EXIT`指令不使用地址码。
