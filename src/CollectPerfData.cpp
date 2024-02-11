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
#include <cstring>
#include <iterator>
#include <thread>
#include "FlameGraphGenerator.h"

CollectPerfData::CollectPerfData(const std::string &options, int duration, CLIParser::ProfilingType profType,
                                 const std::string &cmd, int pidToRecord, const std::string &customFileName) : options(options), duration(duration), profType(profType), cmdToExecute(cmd),
                                                                                                               pidToRecord(pidToRecord), customFileName(customFileName) {}

void CollectPerfData::handleSignal(int signum)
{
    std::cout << "Got SIGINT, stop perf record" << std::endl;
    if (perfPID > 0)
    {
        kill(perfPID, SIGINT);
        waitpid(perfPID, NULL, 0);
    }
    // exit(signum);
}

pid_t CollectPerfData::perfPID = 0;

void CollectPerfData::recordPerf()
{
    signal(SIGINT, CollectPerfData::handleSignal);

    std::vector<std::string> args;
    args.push_back("perf");
    args.push_back("record");

    if (!options.empty())
    {
        std::istringstream iss(options);
        std::copy(std::istream_iterator<std::string>{iss},
                  std::istream_iterator<std::string>(),
                  std::back_inserter(args));
    }
    else
    {
        setProfilingType(profType);
        std::istringstream iss(options);
        std::copy(std::istream_iterator<std::string>{iss},
                  std::istream_iterator<std::string>(),
                  std::back_inserter(args));
    }

    std::cout << "Recorded perf events: " + options << std::endl;

    if (!cmdToExecute.empty())
    {
        args.push_back("--");
        args.push_back(cmdToExecute);
    }
    else if (pidToRecord > -1)
    {
        args.push_back("--pid");
        args.push_back(std::to_string(pidToRecord));
    }

    std::vector<const char *> c_args;
    for (const auto &arg : args)
    {
        c_args.push_back(arg.c_str());
    }
    c_args.push_back(nullptr);

    pid_t pid = fork();

    if (pid == -1)
    {
        throw std::runtime_error("Error creating new process");
    }
    else if (pid == 0)
    {
        execvp("perf", const_cast<char *const *>(c_args.data()));
        exit(1);
    }
    else
    {
        if (duration > 0)
        {
            std::this_thread::sleep_for(std::chrono::seconds(duration));
            kill(pid, SIGINT);
        }
        int status;
        waitpid(pid, &status, 0);
    }
}

std::string CollectPerfData::retriveData()
{
    std::string perfData = execPerf("perf script");
    if (perfData.empty())
    {
        throw std::runtime_error("No output from perf script");
    }

    std::string fileName = genFileName();
    std::ofstream outFile(fileName);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    outFile << perfData;
    outFile.close();

    return perfData;
}

void CollectPerfData::initialize()
{
    const char *dirName = "./results";
    if (mkdir(dirName, 0777) == -1)
    {
        if (errno != EEXIST)
        {
            throw std::runtime_error("Failed to create directory");
        }
    }

    initializeOptimalEvents();
}

std::string CollectPerfData::execPerf(const std::string &command)
{
    std::array<char, 1024> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);

    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
    }

    return result;
}

void CollectPerfData::setProfilingType(CLIParser::ProfilingType type)
{
    profType = type;
    auto filteredEvents = getFilteredEventsForType(type);
    options.clear();

    options += "-F 99 -ag ";
    for (const auto &event : filteredEvents)
    {
        options += "-e " + event + " ";
    }
}

std::string CollectPerfData::genFileName()
{
    std::string baseName = "./results/" + (!customFileName.empty() ? customFileName + "_" : "profile_");

    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << baseName << std::put_time(std::localtime(&now_c), "%d%m%Y_%H%M%S");

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
    case CLIParser::ProfilingType::Network:
        ss << "_Net";
        break;

    default:
        ss << "";
        break;
    }

    if (!cmdToExecute.empty())
    {
        std::string safeCmd = cmdToExecute;
        std::replace(safeCmd.begin(), safeCmd.end(), ' ', '_');
        safeCmd.erase(std::remove(safeCmd.begin(), safeCmd.end(), '-'), safeCmd.end());
        safeCmd.erase(std::remove(safeCmd.begin(), safeCmd.end(), '.'), safeCmd.end());
        safeCmd.erase(std::remove(safeCmd.begin(), safeCmd.end(), '/'), safeCmd.end());

        ss << "_" << safeCmd;
    }

    return ss.str();
}

void CollectPerfData::recordProfiles(const std::set<CLIParser::ProfilingType> &types)
{
    std::vector<std::string> fgFileNames;

    for (auto type : types)
    {
        setProfilingType(type);
        recordPerf();

        std::string perfData = retriveData();
        std::string profFileName = genFileName();

        std::ofstream outFile(profFileName);
        if (!outFile.is_open())
        {
            throw std::runtime_error("Unable to open file: " + profFileName);
        }
        outFile << perfData;
        outFile.close();

        FlameGraphGenerator flameGraphGenerator(perfData, type);
        flameGraphGenerator.generateFlameGraph(profFileName);
        fgFileNames.push_back(profFileName + ".svg");
    }

    FlameGraphGenerator fgGenerator;
    fgGenerator.generateCombinedHtml(fgFileNames, "");
}

void CollectPerfData::recordAllProfiles()
{
    std::set<CLIParser::ProfilingType> allTypes = {
        CLIParser::ProfilingType::CPU,
        CLIParser::ProfilingType::OffCPU,
        CLIParser::ProfilingType::Memory,
        CLIParser::ProfilingType::IO,
        CLIParser::ProfilingType::Network};

    recordProfiles(allTypes);
}

void CollectPerfData::recordSelectedProfiles(const std::set<CLIParser::ProfilingType> &selectedTypes)
{
    recordProfiles(selectedTypes);
}

std::set<std::string> CollectPerfData::getAvailablePerfEvents()
{
    std::set<std::string> events;
    std::array<char, 1024> buffer;
    std::string command = "perf list";
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);

    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
        {
            std::string line = buffer.data();
            auto pos = line.find('[');
            if (pos != std::string::npos)
            {
                std::string event = line.substr(0, pos);
                // delete "whitespace" chars
                event.erase(0, event.find_first_not_of(" \n\r\t"));
                event.erase(event.find_last_not_of(" \n\r\t") + 1);
                events.insert(event);

                // handle 'OR' in event names
                auto orPos = event.find(" OR ");
                if (orPos != std::string::npos)
                {
                    // split string into two events
                    std::string event1 = event.substr(0, orPos);
                    std::string event2 = event.substr(orPos + 4);

                    events.insert(event1);
                    events.insert(event2);
                }
                else
                {
                    events.insert(event);
                }
            }
        }
    }

    return events;
}

void CollectPerfData::initializeOptimalEvents()
{
    optimalEventsForProfiles[CLIParser::ProfilingType::CPU] = {"cpu-clock", "cycles", "instructions"};
    optimalEventsForProfiles[CLIParser::ProfilingType::OffCPU] = {"sched:sched_stat_sleep", "sched:sched_switch", "sched:sched_process_exit"};
    optimalEventsForProfiles[CLIParser::ProfilingType::Memory] = {"cache-misses", "cache-references", "page-faults"};
    optimalEventsForProfiles[CLIParser::ProfilingType::IO] = {"syscalls:sys_enter_read", "syscalls:sys_enter_write", "block:block_rq_issue", "block:block_rq_complete"};
    optimalEventsForProfiles[CLIParser::ProfilingType::Network] = {"net:net_dev_queue", "net:net_dev_xmit", "tcp:tcp_retransmit_skb", "sock:inet_sock_set_state", "udp:udp_*"};
}

std::set<std::string> CollectPerfData::getFilteredEventsForType(CLIParser::ProfilingType type)
{
    auto availableEvents = getAvailablePerfEvents();
    auto &optimalEvents = optimalEventsForProfiles[type];

    std::set<std::string> filteredEvents;

    for (const auto &event : optimalEvents)
    {
        if (availableEvents.find(event) != availableEvents.end())
        {
            filteredEvents.insert(event);
        }
    }

    return filteredEvents;
}