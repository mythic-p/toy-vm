#pragma once
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include "common.h"
#include "instruction.h"

#define DECLARE_HANDLER(OPNAME) void handle_##OPNAME(s32 value)

// The running options of the VM
struct Running_Options {
    // The file path of the input bytecode file.
    std::string file_path;
    // The verbosity level of the output while running the VM
    // There are 4 levels that can be configured:
    // 0 for no information will be output
    // 1 for only some necessary runtime information
    // 2 for basic runtime information and some compile-time information
    // 3 for all runtime and compile-time information
    u8 verbosity_level;
    // The running mode of the VM, it can be 'debug' or 'release'
    // If other value is given, the VM will throw an error to the stderr.
    std::string mode;
};

// The Xue language virtual machine.
class Virtual_Machine {
public:
    Virtual_Machine(Running_Options& running_options);
    ~Virtual_Machine();

    // Load bytecode file from the given path
    // and validate the format of the input file.
    // Return 0 if everything goes well.
    // Or return specific error code that represents what kind of error has happened.
    // Currently, this function can only be called once, if it loads
    // bytecode file successfully.
    u32 load_from_file(std::string& input_path);

    // Run the loaded bytecode file, we assert that the file has passed
    // validation check.
    // Returns 0 if there's no error happened during the execution. Otherwise,
    // specific error code will be returned. See the VM error code
    // documentation for details.
    u32 run();

    // Returns true if the VM loads the given bytecode file
    // successfully. Otherwise, false is returned.
    bool is_loaded() const;
private:
    // Variables
    // the data in the bytecode file.
    u32 instruction_count;
    u32* instructions;

    u8* the_stack;
    // A variable that indicates the number of allocated bytes on the stack.
    u32 stack_allocated;

    // Whether the bytecode file is fully loaded.
    bool is_inited;

    // The program counter that points to the next instruction
    u32* pc;
    // The stack pointer that points to the top of the stack
    u8* sp;

    // Routines
    void throw_error(const char* error_message);

    // Utilities that helps manipulating virtual machine
    void push(u8 value);
    void push(u32 value);
    void push(s32 value);
    void push(f32 value);
    u8 pop();
    u32 pop_u32();
    s32 pop_s32();
    f32 pop_f32();

    // Handle various mnemonics
    DECLARE_HANDLER(add);
    DECLARE_HANDLER(sub);
    DECLARE_HANDLER(div);
    DECLARE_HANDLER(mul);
    DECLARE_HANDLER(mod);
    DECLARE_HANDLER(var);
};

#undef DECLARE_HANDLER
