#include "CLIParser.h"
#include "CollectPerfData.h"
#include "FlameGraphGenerator.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    try {
        CLIParser cliParser(argc, argv);
        cliParser.parseArgs();

        CollectPerfData perfDataCollector(cliParser.getPerfOpts(), cliParser.getDuration(), cliParser.getProfType(), cliParser.getCmdToExecute(), cliParser.getPidToRecord());
        perfDataCollector.initialize();

        if (cliParser.shouldRecordAllProfiles()) {
            perfDataCollector.recordAllProfiles();
        } else {
            auto selectedTypes = cliParser.getSelectedProfilingTypes();
            if (!selectedTypes.empty()) {
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
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
