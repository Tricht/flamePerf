#include "CLIParser.h"
#include "CollectPerfData.h"
#include "FlameGraphGenerator.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
    try
    {
        // parse cli arguments
        CLIParser cliParser(argc, argv);
        if (!cliParser.parseArgs())
        {
            return 1;
        }

        // create perf data collector with arguments parsed from cli
        CollectPerfData perfDataCollector(cliParser.getPerfOpts(), cliParser.getDuration(), cliParser.getProfType(), cliParser.getCmdToExecute(), cliParser.getPidToRecord(), cliParser.getCustomFileName());
        // create results directory
        perfDataCollector.initialize();

        // create diff graph if flag is set
        if (cliParser.shouldCreateDiffGraph())
        {
            FlameGraphGenerator flameGraphGenerator;
            std::string fileName = perfDataCollector.genFileName();
            flameGraphGenerator.generateDiffFlameGraph(cliParser.getDiffFile1(), cliParser.getDiffFile2(), fileName);
        }
        // record all profiles if flag is set
        else if (cliParser.shouldRecordAllProfiles())
        {
            perfDataCollector.recordAllProfiles();
        }
        else
        {
            auto selectedTypes = cliParser.getSelectedProfilingTypes();
            // if profile type and manually options are given, manually options are prioritized
            if (!selectedTypes.empty() && cliParser.getPerfOpts().empty())
            {
                perfDataCollector.recordSelectedProfiles(selectedTypes);
            }
            else
            {
                perfDataCollector.recordPerf();
                std::string perfData = perfDataCollector.retriveData();
                std::string fgName = perfDataCollector.genFileName();
                FlameGraphGenerator flameGraphGenerator(perfData, CLIParser::ProfilingType::Default);
                flameGraphGenerator.generateFlameGraph(fgName);
            }
        }

        std::cout << "Flamegraph(s) successfully generated" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}