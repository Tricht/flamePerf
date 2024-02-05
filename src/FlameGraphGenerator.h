#ifndef FLAME_GRAPH_GENERATOR_H
#define FLAME_GRAPH_GENERATOR_H

#include <string>

#include "CLIParser.h"

class FlameGraphGenerator
{
public:
    FlameGraphGenerator();

    FlameGraphGenerator(const std::string &perfData, CLIParser::ProfilingType profType);

    void generateFlameGraph(const std::string &outputPath);

    void generateCombinedHtml(const std::vector<std::string> &fileNames, const std::string &diffFileName);

    void generateDiffFlameGraph(const std::string &file1, const std::string &file2, const std::string &outputPath);

private:
    std::string perfData;
    CLIParser::ProfilingType profType;

    std::string collapseStack();
};

#endif