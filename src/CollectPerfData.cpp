#include "CollectPerfData.h"


CollectPerfData::CollectPerfData(const std::string &options):options(options), collecting(false){}

void CollectPerfData::recordPerf()
{
    if (collecting) {
        throw "Data collection is already running.";
    }

    std::string cmd = "perf record " + options;
    
    int recordData = system(cmd.c_str());
    
    if (recordData != 0) {
        throw "error in perf record";
    }

    collecting = true;
}

std::string CollectPerfData::retriveData()
{
    return std::string();
}

std::string CollectPerfData::scriptPerf(const std::string &command)
{
    return std::string();
}
