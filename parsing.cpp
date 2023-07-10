#include "parsing.h"
#include <iostream>
ParsedInput ParsedInput::Parse(int argc, char **argv) {
    ParsedInput data;
    std::string current_filter;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.length() > 4 && arg.substr(arg.length() - 4) == ".bmp") {
            if (data.InputFile.empty()) {
                data.InputFile = arg;
            } else if (data.OutputFile.empty()) {
                data.OutputFile = arg;
            } else {
                throw std::runtime_error("Wrong file format");
            }
        } else if (arg.length() > 1 && arg[0] == '-') {
            current_filter = arg;
            data.FilterData.emplace_back(current_filter, std::vector<float>());
        } else if (!current_filter.empty() && isdigit(arg[0])) {
            data.FilterData.back().second.emplace_back(std::stof(arg));
        } else if (arg[0] != '.' && arg[0] != '/' && data.InputFile.empty()) {
            std::cerr << "Wrong input file format\n";
        } else if (arg[0] != '.' && arg[0] != '/' && data.OutputFile.empty()) {
            std::cerr << "Wrong input file format\n";
        }
    }
    return data;
}
