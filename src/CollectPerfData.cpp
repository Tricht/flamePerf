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
#include <sys/stat.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "FlameGraphGenerator.h"


CollectPerfData::CollectPerfData(const std::string& options, int duration, CLIParser::ProfilingType profType, const std::string& cmd, int pidToRecord)
:options(options), duration(duration), profType(profType), cmdToExecute(cmd), pidToRecord(pidToRecord) {}

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
    } else if (pidToRecord > -1) {
        perfCommand += " --pid " + std::to_string(pidToRecord);
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

    std::string fileName = genFileName();
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    outFile << perfData;
    outFile.close();

    return perfData;
}

void CollectPerfData::initialize()
{
    const char* dirName = "./stack_profiles";
    if (mkdir(dirName, 0777) == -1) {
        if (errno != EEXIST) {
            throw std::runtime_error("Failed to create directory");
        }
    }
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

void CollectPerfData::setProfilingType(CLIParser::ProfilingType type)
{
    profType = type;
    switch (type) {
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
}

std::string CollectPerfData::genFileName()
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "./stack_profiles/profile_" << std::put_time(std::localtime(&now_c), "%d%m%Y_%H%M%S");

    switch (profType)
    {
    case CLIParser::ProfilingType::CPU:
        ss << "_CPU";
        break;
    case CLIParser::ProfilingType::OffCPU:
        ss << "_OffCPU";
        break;        
    case CLIParser::ProfilingType::Memory:
        ss << "_Mem";
        break;    
    case CLIParser::ProfilingType::IO:
        ss << "_IO";
        break;

    default:
        ss << "";
        break;
    }

    if (!cmdToExecute.empty()) {
        std::string safeCmd = cmdToExecute;
        std::replace(safeCmd.begin(), safeCmd.end(), ' ', '_');
        std::replace(safeCmd.begin(), safeCmd.end(), '-', '_');
        ss << "_" << safeCmd;
    }

    return ss.str();
}

void CollectPerfData::recordAllProfiles()
{
    const std::array<CLIParser::ProfilingType, 4> allTypes = {
        CLIParser::ProfilingType::CPU,
        CLIParser::ProfilingType::OffCPU,
        CLIParser::ProfilingType::Memory,
        CLIParser::ProfilingType::IO        
    };

    std::vector<std::string> fgFileNames;

    for (auto type : allTypes) {
        setProfilingType(type);
        recordPerf();
        
        std::string perfData = retriveData();
        std::string profFileName = genFileName();

        std::ofstream outFile(profFileName);
        if (!outFile.is_open()) {
            throw std::runtime_error("Unable to open file: " + profFileName);
        }
        outFile << perfData;
        outFile.close();

        std::string fgFileName = profFileName + ".svg";
        FlameGraphGenerator flameGraphGenerator(perfData, type);
        flameGraphGenerator.generateFlameGraph(fgFileName);  
        fgFileNames .push_back(fgFileName);              
    }

    FlameGraphGenerator fgGenerator;
    fgGenerator.generateCombinedHtml(fgFileNames);
}


