#ifndef COLLECT_PERF_DATA_H
#define COLLECT_PERF_DATA_H

#include <string>

class CollectPerfData
{
public: 
    CollectPerfData(const std::string& options);
    ~CollectPerfData();

    void recordPerf();

    std::string retriveData();

private:
    std::string options;
    bool collecting;

    std::string scriptPerf(const std::string& command);
};

#endif