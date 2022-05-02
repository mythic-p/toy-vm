#include "vm.h"

// The VM reads 256 bytes at a time
constexpr u32 READ_BUFFER_SIZE = 256;
// The file signature of the bytecode file
constexpr const char* BYTECODE_SIGNATURE = "XUE!";
// The version constants
constexpr u32 MAJOR_VERSION = 0, MINOR_VERSION = 0, REVISION_VERSION = 1;
// Last supported version number, the older version of the input bytecode file
// may not be executed anymore.
constexpr u32 LAST_MAJOR_VERSION = 0, LAST_MINOR_VERSION = 0, LAST_REVISION_VERSION = 1;

// Return the current version number of the bytecode structure.
constexpr u32 make_bytecode_version() {
    return (MAJOR_VERSION << 24) | (MINOR_VERSION << 16) | REVISION_VERSION;
}

inline bool check_bytecode_version(u32 version) {
    u32 major_version = version >> 24,
        minor_version = (version >> 16) & 0xff,
        revision_version = version & 0xffffff;
    if (major_version < LAST_MAJOR_VERSION) {
        if (minor_version < LAST_MINOR_VERSION) {
            if (revision_version < LAST_REVISION_VERSION) {
                return false;
            }
        }
    }
    return true;
}

Virtual_Machine::Virtual_Machine(Running_Options& running_options): instruction_count(0),
    instructions(nullptr),
    is_inited(false),
    pc(nullptr),
    stack_allocated(0) {
        the_stack = new u8[DEFAULT_STACK_SIZE];
        memset(the_stack, 0, DEFAULT_STACK_SIZE * sizeof(u8));
        sp = the_stack;
    }

Virtual_Machine::~Virtual_Machine() {
    delete[] instructions;
    delete[] the_stack;
}

bool Virtual_Machine::is_loaded() const {
    return is_inited;
}

#define START_HANDLE switch (opcode) {
#define HANDLE_MNEMONIC(a, b) case Opcode::##a: handle_##b(value); break
#define END_HANDLE }

u32 Virtual_Machine::run() {

    push((f32)2);
    push((f32)-3);
    push((u8)0x2);
    handle_div(2);
    std::cout << pop_f32();

    while (false && instruction_count) {
        u32 instruction = *pc;
        u8 opcode = static_cast<u8>((instruction >> 26) & 0x3f);
        s32 value = instruction & 0x3ffffff;

        START_HANDLE;
        HANDLE_MNEMONIC(ADD, add);
        HANDLE_MNEMONIC(SUB, sub);
        HANDLE_MNEMONIC(MUL, mul);
        HANDLE_MNEMONIC(DIV, div);
        HANDLE_MNEMONIC(MOD, mod);
        END_HANDLE;

        pc++;
        instruction_count--;
    }

    return 0;
}

#undef START_HANDLE
#undef HANDLE_MNEMONIC
#undef END_HANDLE

u32 Virtual_Machine::load_from_file(std::string& input_path) {
    std::ifstream ifs(input_path, std::ios::binary | std::ios::in);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open given file: " << input_path << std::endl;
        return 1;
    }
    // Initialize the input buffer.
    u8 buffer[READ_BUFFER_SIZE] = {0};
    // Reads the file signature and validates it.
    ifs.read(reinterpret_cast<char*>(buffer), 4);
    if (strcmp(reinterpret_cast<const char*>(buffer), BYTECODE_SIGNATURE) != 0) {
        std::cerr << "Invalid bytecode signature!";
        return 2; // Invalid file signature
    }
    // Reads the bytecode file version
    ifs.read(reinterpret_cast<char*>(buffer), 4);
    if (!check_bytecode_version(*reinterpret_cast<u32*>(buffer))) {
        std::cerr << "The version of bytecode file is too old to be executed.";
        return 3; // Invalid version number
    }
    ifs.close();

    is_inited = true;
    return 0;
}

void Virtual_Machine::throw_error(const char* error_message) {
    std::cerr << error_message << std::endl;
}

#define OVERFLOW_CHECK(type) if (stack_allocated + sizeof(##type) >= DEFAULT_STACK_SIZE) { \
    throw_error("Stack Overflow!"); \
    return; \
}

void Virtual_Machine::push(u8 value) {
    OVERFLOW_CHECK(u8);
    *sp++ = value;
    stack_allocated += sizeof(u8);
}

void Virtual_Machine::push(u32 value) {
    OVERFLOW_CHECK(u32);
    push(static_cast<u8>((value & 0xff000000) >> 24));// 0x[XX]000000
    push(static_cast<u8>((value & 0xff0000) >> 16));// 0x00[XX]0000
    push(static_cast<u8>((value & 0xff00) >> 8));// 0x0000[XX]00
    push(static_cast<u8>(value & 0xff));// 0x000000[XX]
}

void Virtual_Machine::push(s32 value) {
    push(static_cast<u32>(value));
}

void Virtual_Machine::push(f32 value) {
    OVERFLOW_CHECK(f32);
    memcpy(sp, &value, sizeof(f32));
    sp += sizeof(f32);
    stack_allocated += sizeof(f32);
}

#undef OVERFLOW_CHECK

u8 Virtual_Machine::pop() {
    return *--sp;
}

u32 Virtual_Machine::pop_u32() {
    u32 value = pop() | (pop() << 8) | (pop() << 16) | (pop() << 24);
    return value;
}

s32 Virtual_Machine::pop_s32() {
    return static_cast<s32>(pop_u32());
}

f32 Virtual_Machine::pop_f32() {
    f32 value;
    memcpy(&value, sp - 4, 4);
    sp -= 4;
    return value;
}

// Mnemonic handlers
#define IMPL(name) void Virtual_Machine::handle_##name(s32 value)
#define IMPL_ARITHMETIC(op) u8 flag = pop(); \
    bool is_integer = flag & 0x01, \
        is_signed = flag & 0x02; \
    if (is_integer) { \
        if (is_signed) { \
            s32 a = pop_s32(), b = pop_s32(); \
            push(a ##op b); \
        } else { \
            u32 a = pop_u32(), b = pop_u32(); \
            push((u32)(a ##op b)); \
        } \
    } else { \
        f32 a = pop_f32(), b = pop_f32(); \
        push(a ##op b); \
    }


IMPL(add) {
    // This flag indicates whether the ADD operation is an integer operation,
    // and whether the two operands are signed.
    // 0x0[0] this one indicates the operation type
    // 0x[0]0 this one indicates the sign, only works when the operation is an integer one.
    IMPL_ARITHMETIC(+);
}

IMPL(sub) {
    IMPL_ARITHMETIC(-);
}

IMPL(mul) {
    IMPL_ARITHMETIC(*);
}

IMPL(div) {
    IMPL_ARITHMETIC(/);
}

IMPL(mod) {
    // note that only integer can do the modulo operation
    u8 flag = pop();
    bool is_signed = flag & 0x2;
    if (is_signed) {
        s32 a = pop_s32(), b = pop_s32();
        push(a % b);
    } else {
        u32 a = pop_u32(), b = pop_u32();
        push((u32)(a % b));
    }
}

IMPL(var) {}

#undef IMPL
#undef IMPL_ARITHMETIC
