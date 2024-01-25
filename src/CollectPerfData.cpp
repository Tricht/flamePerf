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

CollectPerfData::CollectPerfData(const std::string & options, int duration, CLIParser::ProfilingType profType,
    const std::string & cmd, int pidToRecord): options(options), duration(duration), profType(profType), cmdToExecute(cmd), pidToRecord(pidToRecord) {}

void CollectPerfData::recordPerf() {

    std::string perfCommand = "perf record ";

    if (!options.empty()) {
        perfCommand += options;
    } else {
        setProfilingType(profType);
        perfCommand += options;
    }

    std::cout << options << std::endl; // debug msg to see which options are recorded

    if (!cmdToExecute.empty()) {
        perfCommand += " -- " + cmdToExecute;
    } else if (pidToRecord > -1) {
        perfCommand += " --pid " + std::to_string(pidToRecord);
    } else {
        perfCommand += " -- sleep " + std::to_string(duration);
    }

    int status = system(perfCommand.c_str());
    if (status != 0) {
        throw std::runtime_error("Failed to execute perf record: " + status);
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

std::string CollectPerfData::retriveData() {
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

void CollectPerfData::initialize() {
    const char * dirName = "./results";
    if (mkdir(dirName, 0777) == -1) {
        if (errno != EEXIST) {
            throw std::runtime_error("Failed to create directory");
        }
    }
}

std::string CollectPerfData::execPerf(const std::string & command) {
    std::array <char, 1024> buffer;
    std::string result;
    std::shared_ptr <FILE> pipe(popen(command.c_str(), "r"), pclose);
    
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

void CollectPerfData::setProfilingType(CLIParser::ProfilingType type) {
    /* profType = type;
    switch (type) {
    case CLIParser::ProfilingType::CPU:
        options = "-F 99 -e cpu-clock -e cycles -ag";
        break;
    case CLIParser::ProfilingType::OffCPU:
        options = "-F 99 -e sched:sched_stat_sleep -e sched:sched_switch -e sched:sched_process_exit -ag";
        break;
    case CLIParser::ProfilingType::Memory:
        options = "-F 99 -e cache-misses -e cache-references -ag";
        break;
    case CLIParser::ProfilingType::IO:
        options = "-F 99 -e syscalls:sys_enter_read -e syscalls:sys_enter_write -ag";
        break;
    case CLIParser::ProfilingType::Network:
        options = "-F 99 -e net:net_dev_queue -e net:net_dev_xmit -e tcp:tcp_retransmit_skb -e sock:inet_sock_set_state -ag";
        break;    
    case CLIParser::ProfilingType::Default:
        options = "-F 99 -ag";
        break;
    default:
        options = "-F 99 -ag";
        break;
    } */
    profType = type;
    auto filteredEvents = getFilteredEventsForType(type);

    options.clear();
    for (const auto& event : filteredEvents) {
        options += "-e " + event + " ";
    }

    // Füge weitere allgemeine Optionen hinzu
    options += "-F 99 -ag";
}

std::string CollectPerfData::genFileName() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "./results/profile_" << std::put_time(std::localtime( & now_c), "%d%m%Y_%H%M%S");

    switch (profType) {
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
    case CLIParser::ProfilingType::Network:
        ss << "_Net";
        break;        

    default:
        ss << "";
        break;
    }

    if (!cmdToExecute.empty()) {
        std::string safeCmd = cmdToExecute;
        std::replace(safeCmd.begin(), safeCmd.end(), ' ', '_');
        safeCmd.erase(std::remove(safeCmd.begin(), safeCmd.end(), '-'), safeCmd.end());
        safeCmd.erase(std::remove(safeCmd.begin(), safeCmd.end(), '.'), safeCmd.end());
        safeCmd.erase(std::remove(safeCmd.begin(), safeCmd.end(), '/'), safeCmd.end());

        ss << "_" << safeCmd;
    }

    return ss.str();
}

void CollectPerfData::recordProfiles(const std::set < CLIParser::ProfilingType > & types) {
    std::vector < std::string > fgFileNames;

    for (auto type: types) {
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

        FlameGraphGenerator flameGraphGenerator(perfData, type);
        flameGraphGenerator.generateFlameGraph(profFileName);
        fgFileNames.push_back(profFileName + ".svg");
    }

    FlameGraphGenerator fgGenerator;
    fgGenerator.generateCombinedHtml(fgFileNames);
}

void CollectPerfData::recordAllProfiles() {
    std::set < CLIParser::ProfilingType > allTypes = {
        CLIParser::ProfilingType::CPU,
        CLIParser::ProfilingType::OffCPU,
        CLIParser::ProfilingType::Memory,
        CLIParser::ProfilingType::IO,
        CLIParser::ProfilingType::Network
    };

    recordProfiles(allTypes);
}

void CollectPerfData::recordSelectedProfiles(const std::set < CLIParser::ProfilingType > & selectedTypes) {
    recordProfiles(selectedTypes);
}

std::set<std::string> CollectPerfData::getAvailablePerfEvents()
{
    std::set<std::string> events;
    std::array<char, 1024> buffer;
    std::string command = "perf list";
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            std::string line = buffer.data();
            auto pos = line.find('[');
            if (pos != std::string::npos) {
                std::string event = line.substr(0, pos);
                event.erase(event.find_last_not_of(" \n\r\t") + 1);
                events.insert(event);
            }
        }
    }

    return events;
}

void CollectPerfData::initializeOptimalEvents()
{
    optimalEventsForProfiles[CLIParser::ProfilingType::CPU] = {"cpu-clock", "cycles"};
    optimalEventsForProfiles[CLIParser::ProfilingType::OffCPU] = {"sched:sched_stat_sleep", "sched:sched_switch", "sched:sched_process_exit"};
    optimalEventsForProfiles[CLIParser::ProfilingType::Memory] = {"cache-misses", "cache-references"};
    optimalEventsForProfiles[CLIParser::ProfilingType::IO] = {"syscalls:sys_enter_read", "syscalls:sys_enter_write"};
    optimalEventsForProfiles[CLIParser::ProfilingType::Network] = {"net:net_dev_queue", "net:net_dev_xmit", "tcp:tcp_retransmit_skb", "sock:inet_sock_set_state"};
}

std::set<std::string> CollectPerfData::getFilteredEventsForType(CLIParser::ProfilingType type)
{
    auto availableEvents = getAvailablePerfEvents();
    for(auto event : availableEvents) {
        std::cout << event << std::endl;
    }
    auto& optimalEvents = optimalEventsForProfiles[type];
    for(auto event : optimalEvents) {
        std::cout << event << std::endl;
    }
    std::set<std::string> filteredEvents;


    for (const auto& event : optimalEvents) {
        if (availableEvents.find(event) != availableEvents.end()) {
            filteredEvents.insert(event);
        }
    }

    return filteredEvents;
}