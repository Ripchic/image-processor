#pragma once
#include <map>
#include <string>
#include <vector>

class ParsedInput {
public:
    std::string InputFile;
    std::string OutputFile;
    std::vector<std::pair<std::string, std::vector<float>>> FilterData;
    static ParsedInput Parse(int argc, char** argv);
};