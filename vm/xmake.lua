add_rules("mode.debug", "mode.release")

target("vm")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("./include", "./lib")
