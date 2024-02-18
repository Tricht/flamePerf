// CLIParser.cpp
// Implements the CLIParser class methods defined in CLIParser.h.

#include "CLIParser.h"
#include "ProfilingManager.h"
#include <iostream>
#include <sstream>

CLIParser::CLIParser(int argc, char **argv) : argc(argc), argv(argv) {}

bool CLIParser::parseArgs()
{
    if (argc == 1)
    {
        showHelp();
        return false;
    }

    ProfilingManager profilingManager("../profiles.txt");
    auto customProfiles = profilingManager.loadProfiles();

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        // Handling different cli options
        if (arg == "--diff") // create a diff graph
        {
            diffFlag = true;
            diffFile1 = argv[++i];
            diffFile2 = argv[++i];
        }
        else if (arg == "-o" || arg == "--perf-options") // options for perf command
        {
            perfOpts = argv[++i];
        }
        else if (arg == "-d" || arg == "--duration") // how long should perf run
        {
            try
            {
                duration = std::stoi(argv[++i]);
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Invalid duration value" << std::endl;
                throw;
            }
        }
        else if (arg == "-c" || arg == "--cmd") // instead of duration, measure a command...
        {
            cmdToExecute = argv[++i];
        }
        else if (arg == "--pid") // ...or pid
        {
            try
            {
                pidToRecord = std::stoi(argv[++i]);
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Invalid pid value" << std::endl;
                throw;
            }
        }
        else if (arg == "--all-profiles") // record all profiles
        {
            allProfiles = true;
        }
        else if (arg == "-p" || arg == "--profile-types") // record chosen profiles
        {
            std::string types = argv[++i];
            std::istringstream typesStream(types);
            std::string type;

            while (std::getline(typesStream, type, ','))
            {
                if (customProfiles.find(type) != customProfiles.end())
                {
                    std::string events = customProfiles[type];
                    std::istringstream eventsStream(events);
                    std::string event;
                    while (std::getline(eventsStream, event, ','))
                    {
                        perfOpts += "-e " + event + " ";
                    }

                    addProfilingType(ProfilingType::Custom);
                }

                else if (type == "cpu")
                {
                    addProfilingType(ProfilingType::CPU);
                }
                else if (type == "offcpu")
                {
                    addProfilingType(ProfilingType::OffCPU);
                }
                else if (type == "mem")
                {
                    addProfilingType(ProfilingType::Memory);
                }
                else if (type == "io")
                {
                    addProfilingType(ProfilingType::IO);
                }
                else if (type == "net")
                {
                    addProfilingType(ProfilingType::Network);
                }
                else
                {
                    std::cerr << "Invalid profiling type: " << type << std::endl;
                }
            }
        }
        else if (arg == "-f" || arg == "--filename")
        {
            setCustomFileName(argv[++i]);
        }
        else if (arg == "--custom-profile")
        {
            std::string profile = argv[++i];
            auto deliPos = profile.find("=");
            if (deliPos != std::string::npos)
            {
                std::string profName = profile.substr(0, deliPos);
                std::string profEvents = profile.substr(deliPos + 1);
                profilingManager.saveProfile(profName, profEvents);
                return false;
            }
        }
        else if (arg == "-h" || "--help")
        {
            showHelp();
            return false;
        }
        else // help message
        {
            showHelp();
            return false;
        }
    }

    return true;
}

std::string CLIParser::getPerfOpts()
{
    return perfOpts;
}

int CLIParser::getDuration()
{
    return duration;
}

CLIParser::ProfilingType CLIParser::getProfType() const
{
    return profType;
}

std::string CLIParser::getCmdToExecute() const
{
    return cmdToExecute;
}

int CLIParser::getPidToRecord()
{
    return pidToRecord;
}

bool CLIParser::shouldRecordAllProfiles() const
{
    return allProfiles;
}

const std::set<CLIParser::ProfilingType> &CLIParser::getSelectedProfilingTypes() const
{
    return selectedProfilingTypes;
}

void CLIParser::addProfilingType(ProfilingType type)
{
    selectedProfilingTypes.insert(type);
}

bool CLIParser::shouldCreateDiffGraph() const
{
    return diffFlag;
}

std::string CLIParser::getDiffFile1() const
{
    return diffFile1;
}

std::string CLIParser::getDiffFile2() const
{
    return diffFile2;
}

void CLIParser::setCustomFileName(const std::string name)
{
    customFileName = name;
}

std::string CLIParser::getCustomFileName()
{
    return customFileName;
}

void CLIParser::showHelp()
{
    std::cout << "Usage: flamePerf [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -o, --perf-options\tSet options for the perf command" << std::endl;
    std::cout << "  -d, --duration\t\tSpecify the duration for the perf recording" << std::endl;
    std::cout << "  -c, --cmd\t\tExecute a specific command" << std::endl;
    std::cout << "  --pid\t\t\tRecord a specific process by PID" << std::endl;
    std::cout << "  --all-profiles\t\tRecord all available profiles" << std::endl;
    std::cout << "  -p, --profile-types\tSpecify types of profiles to record" << std::endl;
    std::cout << "  -f, --filename\tSet name for sstack profiles and flamegraphs" << std::endl;
    std::cout << "Example: flamePerf -d 5 --all-profiles" << std::endl;
}
