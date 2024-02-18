// CollectPerfData.h
// Defines the CollectPerfData class which is responsible for recording,
// converting and storing perf data

#ifndef COLLECT_PERF_DATA_H
#define COLLECT_PERF_DATA_H

#include "CLIParser.h"
#include <string>
#include <map>

// CollectPerfData class provides functions to record, convert and store perf data
class CollectPerfData
{
public:
    // Constructor: initializes the collector
    CollectPerfData(const std::string &options, int duration, CLIParser::ProfilingType profType,
                    const std::string &cmd, int pidToRecord, const std::string &customFileName);

    // execute perf record with the options from the constructor
    void recordPerf();

    // execute perf script and name output
    std::string retriveData();

    // creates the result directory
    void initialize();

    // generates a unique file name
    std::string genFileName();

    // record all available Profiling Types
    void recordAllProfiles();

    // record selected Profiling Types
    void recordSelectedProfiles(const std::set<CLIParser::ProfilingType> &selectedTypes);

    // signal handler
    static void handleSignal(int signum);

private:
    std::string options;
    static pid_t perfPID;
    int duration;
    CLIParser::ProfilingType profType;
    std::string cmdToExecute;
    int pidToRecord;
    std::map<CLIParser::ProfilingType, std::set<std::string>> optimalEventsForProfiles;
    std::string customFileName;

    // executes a system command, should be used for perf cmds
    std::string execPerf(const std::string &command);

    // sets the perf events to record for different Profiling Types
    void setProfilingType(CLIParser::ProfilingType type, const std::string &customEvents);

    // records and retrieves perf data for the set of selected Profiling Types
    void recordProfiles(const std::set<CLIParser::ProfilingType> &types);

    // collect available perf events on system
    static std::set<std::string> getAvailablePerfEvents();

    // create lists with optimal events for every profiling type
    void initializeOptimalEvents();

    // adjust profile types to only collect the avaiable perf events
    std::set<std::string> getFilteredEventsForType(CLIParser::ProfilingType type);
};

#endif