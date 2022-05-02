#include <iostream>
#include <string>

#include "vm.h"

#include <lyra/lyra.hpp>

int main(int argc, char** argv)
{
    Running_Options running_options;
    bool show_help = false;
    auto cli = lyra::cli();
    cli.add_argument(lyra::help(show_help))
        .add_argument(lyra::arg(running_options.file_path, "file").help("The path of the input bytecode file.").required())
        .add_argument(lyra::opt(running_options.verbosity_level, "verbose").name("-v").name("--verbose").help("The verbosity level of the output"))
        .add_argument(lyra::opt(running_options.mode, "mode").name("-m").name("--mode").help("The running mode of the Xue virtual machine."));
    auto result = cli.parse({ argc, argv });
    if (!result) {
        std::cerr << "[ERROR] " << result.message() << std::endl << std::endl;
        std::cerr << cli << std::endl;
        return 1;
    }
    if (show_help) {
        std::cout << cli << std::endl;
        return 0;
    }
    Virtual_Machine the_vm(running_options);
    // int ret_code = the_vm.load_from_file(running_options.file_path);
    // if (ret_code > 0) {
    //     //
    // }
    the_vm.run();
    return 0;
}
