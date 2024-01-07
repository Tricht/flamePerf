#include "CollectPerfData.h"
#include <array>
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>
#include <chrono>
#include <future>


CollectPerfData::CollectPerfData(const std::string &options, int duration, CLIParser::ProfilingType profType)
:options(options), duration(duration), profType(profType){}

void CollectPerfData::recordPerf()
{
    switch(profType) {
        case CLIParser::ProfilingType::CPU:
            options = "-F 99 -e cycles -ag";
            break;
        case CLIParser::ProfilingType::OffCPU:
            options = "-F 99 -e sched:sched_stat_sleep -e sched:sched_switch -e sched:sched_process_exit -ag";
            break;
        case CLIParser::ProfilingType::Memory:
            options = "-F 99 -e cache-misses,cache-references -ag";
            break;
        case CLIParser::ProfilingType::IO:
            options = "-F 99 -e block:block_rq_issue -ag";
            break;
        default:
            options = "-F 99 -ag";
            break;            
    }

    std::future<void> collectFuture = std::async(std::launch::async, [this]() {
        pid_t pid = fork();
        if (pid == 0) {
            execlp("perf", "perf", "record", options.c_str(), (char *)NULL);
            exit(1);
        } else if (pid > 0) {
            perfPID = pid;

            std::this_thread::sleep_for(std::chrono::seconds(duration));

            kill(perfPID, SIGINT);
            int status;
            waitpid(perfPID, &status, 0);
        } else {
            throw std::runtime_error("Failed to start perf record");
        }
    });

    collectFuture.get();
}

std::string CollectPerfData::retriveData()
{
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
