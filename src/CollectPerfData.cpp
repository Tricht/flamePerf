#include "CollectPerfData.h"
#include <array>
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>


CollectPerfData::CollectPerfData(const std::string &options):options(options), collecting(false){}

void CollectPerfData::recordPerf()
{
    if (collecting) {
        throw std::runtime_error("Data collection is already running");
    }

    pid_t pid = fork();
    if (pid == 0) {
        execlp("perf", "perf", "record", options.c_str(), (char *)NULL);
        exit(1);
    } else if (pid > 0) {
        perfPID = pid;
        collecting = true;
    } else {
        throw std::runtime_error("Failed to start perf record");
    }
}

std::string CollectPerfData::retriveData()
{
    if (!collecting) {
        throw std::runtime_error("data is currently not collected");
    }

    kill(perfPID, SIGINT);

    int status;
    waitpid(perfPID, &status, 0);

    collecting = false;

    std::string perfData = execPerf("perf script");
    return perfData;
}

std::string CollectPerfData::execPerf(const std::string &command)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }

    return result;
}
