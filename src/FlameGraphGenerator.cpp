#include "FlameGraphGenerator.h"
#include <fstream>
#include <array>
#include <memory>

FlameGraphGenerator::FlameGraphGenerator(const std::string &perfData, CLIParser::ProfilingType profType)
:perfData(perfData), profType(profType){}

void FlameGraphGenerator::generateFlameGraph(const std::string &outputPath)
{
    std::string collapsedData = collapseStack();

}

std::string FlameGraphGenerator::collapseStack()
{
    std::string collapseScriptPath;

    switch(profType) {
        case CLIParser::ProfilingType::OffCPU:
            collapseScriptPath = "../Flamegraph/stackcollapse-perf-sched.awk";
            break;
        default:
            collapseScriptPath = "../Flamegraph/stackcollapse-perf.pl";
            break;            
    }

    std::string cmd = "perl " + collapseScriptPath + " " + perfData;

    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }

    return result; 
}
