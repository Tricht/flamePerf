#ifndef FLAME_GRAPH_GENERATOR_H
#define FLAME_GRAPH_GENERATOR_H

#include <string>
#include "CLIParser.h"

class FlameGraphGenerator 
{
public:
    FlameGraphGenerator(const std::string& perfData, CLIParser::ProfilingType profType);

    void generateFlameGraph(const std::string& outputPath);

private:
    std::string perfData;
    CLIParser::ProfilingType profType;

    std::string collapseStack();   
};

#endif
