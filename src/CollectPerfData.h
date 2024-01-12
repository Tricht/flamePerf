#ifndef COLLECT_PERF_DATA_H
#define COLLECT_PERF_DATA_H

#include "CLIParser.h"

#include <string>

class CollectPerfData
{
public: 
    CollectPerfData(const std::string &options, int duration, CLIParser::ProfilingType profType, const std::string &cmd);
    // ~CollectPerfData();

    void recordPerf();

    std::string retriveData();

private:
    std::string options;
    pid_t perfPID;
    int duration;
    CLIParser::ProfilingType profType;
    std::string cmdToExecute;

    std::string execPerf(const std::string& command);
};

#endif