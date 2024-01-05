#include "CollectPerfData.h"
#include <array>
#include <memory>


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
    if (!collecting) {
        throw "data is currently not collected";
    }

    system("perf record --stop");

    collecting = false;

    std::string perfData = scriptPerf("perf script");
    return perfData;
}

std::string CollectPerfData::scriptPerf(const std::string &command)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw "popen() failed!";
    }

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }

    return result;
}
