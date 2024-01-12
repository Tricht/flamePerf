#include "CollectPerfData.h"
#include <array>
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>
#include <chrono>
#include <future>
#include <iostream>
#include <fstream>


CollectPerfData::CollectPerfData(const std::string &options, int duration, CLIParser::ProfilingType profType, const std::string &cmd)
:options(options), duration(duration), profType(profType), cmdToExecute(cmd) {}

void CollectPerfData::recordPerf()
{
    std::cout << static_cast<std::underlying_type<CLIParser::ProfilingType>::type>(profType) << std::endl;
    
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
        case CLIParser::ProfilingType::Default:
            break;    
        default:
            options = "-F 99 -ag";
            break;            
    }

    std::cout << options << std::endl;  

    std::string perfCommand = "perf record " + options;

    if (!cmdToExecute.empty()) {
        perfCommand += " -- " + cmdToExecute;
    } else {
        perfCommand += " -- sleep " + std::to_string(duration);
    }

    int status = system(perfCommand.c_str());
    if (status != 0) {
        std::cerr << "Failed to execute perf record: " << status << std::endl;
    }

/*     std::future<void> collectFuture = std::async(std::launch::async, [this]() {
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

    collectFuture.get(); */
}

std::string CollectPerfData::retriveData()
{
    std::string perfData = execPerf("perf script");
   if (perfData.empty()) {
        throw std::runtime_error("No output from perf script");
    }

    std::ofstream abcd("perfScriptTest");
    abcd << perfData;
    abcd.close();

    return perfData;
}

std::string CollectPerfData::execPerf(const std::string &command)
{
    std::array<char, 1024> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }

    return result;
}
