#ifndef COLLECT_PERF_DATA_H
#define COLLECT_PERF_DATA_H

#include "CLIParser.h"

#include <string>

class CollectPerfData
{
public: 
    CollectPerfData(const std::string& options, int duration, CLIParser::ProfilingType profType, const std::string& cmd, int pidToRecord);
    // ~CollectPerfData();

    void recordPerf();

    std::string retriveData();

    void initialize();

    std::string genFileName();

    void recordAllProfiles();

private:
    std::string options;
    pid_t perfPID;
    int duration;
    CLIParser::ProfilingType profType;
    std::string cmdToExecute;
    int pidToRecord;

    std::string execPerf(const std::string& command);

    void setProfilingType(CLIParser::ProfilingType type);
    
};

#endif