add_rules("mode.debug", "mode.release")

target("disassembler")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("./include", "./lib")
