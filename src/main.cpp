#include "CLIParser.h"

#include "CollectPerfData.h"

#include "FlameGraphGenerator.h"

#include <iostream>

#include <stdexcept>

int main(int argc, char ** argv) {
    try {
        // parse cli arguments
        CLIParser cliParser(argc, argv);
        cliParser.parseArgs();

        // create perf data collector with arguments parsed from cli
        CollectPerfData perfDataCollector(cliParser.getPerfOpts(), cliParser.getDuration(), cliParser.getProfType(), cliParser.getCmdToExecute(), cliParser.getPidToRecord());
        // create results directory
        perfDataCollector.initialize();

        // record all profiles if flag is set
        if (cliParser.shouldRecordAllProfiles()) {
            perfDataCollector.recordAllProfiles();    
        } else {
            auto selectedTypes = cliParser.getSelectedProfilingTypes();
            // if profile type and manually options are given, manually options are prioritized
            if (!selectedTypes.empty() && cliParser.getPerfOpts().empty()) {
                perfDataCollector.recordSelectedProfiles(selectedTypes);
            } else {
                perfDataCollector.recordPerf();
                std::string perfData = perfDataCollector.retriveData();
                std::string fgName = perfDataCollector.genFileName();
                FlameGraphGenerator flameGraphGenerator(perfData, cliParser.getProfType());
                flameGraphGenerator.generateFlameGraph(fgName);
            }
        }

        std::cout << "Flamegraph(s) successfully generated" << std::endl;
    } catch (const std::exception & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}