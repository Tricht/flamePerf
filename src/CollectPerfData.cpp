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

CollectPerfData::CollectPerfData(const std::string &options, int duration, CLIParser::ProfilingType profType, const std::string &cmd, int pidToRecord, const std::string &customFileName) : options(options), duration(duration), profType(profType), cmdToExecute(cmd), pidToRecord(pidToRecord), customFileName(customFileName) {}

// handle SIGINT to stop perf record, but not the whole process
void CollectPerfData::handleSignal(int signum)
{
    std::cout << "Got SIGINT, stop perf record" << std::endl;
    if (perfPID > 0)
    {
        kill(perfPID, SIGINT);
        waitpid(perfPID, NULL, 0);
    }
    // do not exit
    // exit(signum);
}

// initialize static pid for perf
pid_t CollectPerfData::perfPID = 0;

void CollectPerfData::recordPerf()
{
    // handle SIGINT
    signal(SIGINT, CollectPerfData::handleSignal);

    // create arguments vector and add perf and record as first two args
    std::vector<std::string> args;
    args.push_back("perf");
    args.push_back("record");

    // insert perf options chosen by user
    if (!options.empty())
    {
        std::istringstream iss(options);
        std::copy(std::istream_iterator<std::string>{iss},
                  std::istream_iterator<std::string>(),
                  std::back_inserter(args));
    }
    // when no user input for options, insert profiling type
    else
    {
        setProfilingType(profType);
        std::istringstream iss(options);
        std::copy(std::istream_iterator<std::string>{iss},
                  std::istream_iterator<std::string>(),
                  std::back_inserter(args));
    }

    std::cout << "Recorded perf events: " + options << std::endl;

    // add command to analyze
    if (!cmdToExecute.empty())
    {
        std::istringstream iss(cmdToExecute);
        std::vector<std::string> cmdArgs(std::istream_iterator<std::string>{iss}, {});
        args.push_back("--");
        args.insert(args.end(), cmdArgs.begin(), cmdArgs.end());
    }
    // or add pid to analyze
    else if (pidToRecord > -1)
    {
        args.push_back("--pid");
        args.push_back(std::to_string(pidToRecord));
    }
    // when no cmd or pid was selected, analyze all cpus and processes. Add -a flag after perf record
    else
    {
        args.insert(args.begin() + 2, "-a");
    }

    // convert args to c_string
    std::vector<const char *> c_args;
    for (const auto &arg : args)
    {
        c_args.push_back(arg.c_str());
    }
    c_args.push_back(nullptr);

    // create new process
    pid_t pid = fork();

    if (pid == -1)
    {
        throw std::runtime_error("Error creating new process");
    }
    // execute perf record with arguments
    else if (pid == 0)
    {
        execvp("perf", const_cast<char *const *>(c_args.data()));
        std::cerr << "Error during execvp: " << strerror(errno) << std::endl;
        exit(1);
    }
    else
    {
        // kill process when duration is reached
        if (duration > 0)
        {
            std::this_thread::sleep_for(std::chrono::seconds(duration));
            kill(pid, SIGINT);
        }
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            throw std::runtime_error("Child process ended with error");
        }
    }
}

std::string CollectPerfData::retriveData()
{
    // execute perf script
    std::string perfData = execPerf("perf script");
    if (perfData.empty())
    {
        throw std::runtime_error("No output from perf script");
    }
    // give unique name to scripted data and save it
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
    // create results directory
    const char *dirName = "./results";
    if (mkdir(dirName, 0777) == -1)
    {
        if (errno != EEXIST)
        {
            throw std::runtime_error("Failed to create directory");
        }
    }
    // set optimal events for each profiling type
    initializeOptimalEvents();
}

std::string CollectPerfData::execPerf(const std::string &command)
{
    // execute system command
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
    // return data created by cmd
    return result;
}

void CollectPerfData::setProfilingType(CLIParser::ProfilingType type)
{
    // get available perf events for profiling type
    profType = type;
    auto filteredEvents = getFilteredEventsForType(type);
    options.clear();

    // options 'base' will operate by a frequency of 99Hz and creates stacktraces
    options += "-F 99 -g ";
    for (const auto &event : filteredEvents)
    {
        // add event to call
        options += "-e " + event + " ";
    }
}

std::string CollectPerfData::genFileName()
{
    // file name starts with result dir and 'profile_'
    std::string baseName = "./results/" + (!customFileName.empty() ? customFileName + "_" : "profile_");

    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    // add date and time to filename
    ss << baseName << std::put_time(std::localtime(&now_c), "%d%m%Y_%H%M%S");

    // add profiling type to filename
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

    // remove special chars and whitespace from command to analyze; add it to filename
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
        // set profiling type
        setProfilingType(type);
        // execute perf record
        recordPerf();

        // perf script data
        std::string perfData = retriveData();
        // generate filename
        std::string profFileName = genFileName();

        // save scripted perf data to a file
        std::ofstream outFile(profFileName);
        if (!outFile.is_open())
        {
            throw std::runtime_error("Unable to open file: " + profFileName);
        }
        outFile << perfData;
        outFile.close();

        // generate Flame Graph for perf data
        FlameGraphGenerator flameGraphGenerator(perfData, type);
        flameGraphGenerator.generateFlameGraph(profFileName);
        // push back Flame Graph filenames
        fgFileNames.push_back(profFileName + ".svg");
    }
    // generate the combined Flame Graph html file with created Flame Graphs from before
    FlameGraphGenerator fgGenerator;
    fgGenerator.generateCombinedHtml(fgFileNames, "");
}

void CollectPerfData::recordAllProfiles()
{
    // define what are 'all' profiles
    std::set<CLIParser::ProfilingType> allTypes = {
        CLIParser::ProfilingType::CPU,
        CLIParser::ProfilingType::OffCPU,
        CLIParser::ProfilingType::Memory,
        CLIParser::ProfilingType::IO,
        CLIParser::ProfilingType::Network};

    // record them
    recordProfiles(allTypes);
}

void CollectPerfData::recordSelectedProfiles(const std::set<CLIParser::ProfilingType> &selectedTypes)
{
    // record selected profiles
    recordProfiles(selectedTypes);
}

std::set<std::string> CollectPerfData::getAvailablePerfEvents()
{
    std::set<std::string> events;
    std::array<char, 1024> buffer;
    std::string command = "perf list";
    // execute perf list to get all available events
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
            // search for end of event line
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