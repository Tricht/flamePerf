#include "FlameGraphGenerator.h"
#include <fstream>
#include <array>
#include <memory>
#include <iostream>

FlameGraphGenerator::FlameGraphGenerator(const std::string &perfData, CLIParser::ProfilingType profType)
:perfData(perfData), profType(profType){}

void FlameGraphGenerator::generateFlameGraph(const std::string &outputPath)
{
    std::string collapsedData = collapseStack();

    std::string tempCollOut = "tempCollOut.tmp";

    std::ofstream tempOutFile(tempCollOut);
    if (!tempOutFile.is_open()) {
        throw std::runtime_error("Unable to open temp file");
    }

    tempOutFile << collapsedData;
    tempOutFile.close();

    std::string flameGraphCmd = "perl ../FlameGraph/flamegraph.pl " + tempCollOut + " > " + outputPath;

    int result = system(flameGraphCmd.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to generate flamegraph");
    }

    if (remove(tempCollOut.c_str()) != 0) {
        std::cerr << "Failed to delete temp file" << std::endl;
    }
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
    if (!pipe) {
        throw std::runtime_error("popen() failed");
    }

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }

    return result; 
}
