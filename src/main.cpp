#include "CLIParser.h"
#include "CollectPerfData.h"
#include "FlameGraphGenerator.h"
#include <iostream>
#include <stdexcept>

/* int main(int argc, char** argv) {
    try {
        CLIParser cliParser(argc, argv);
        cliParser.parseArgs();
        std::cout << cliParser.getPerfOpts() << std::endl;
        std::cout << cliParser.getDuration() << std::endl;
        CLIParser::ProfilingType prof = cliParser.getProfType();
        std::cout << static_cast<std::underlying_type<CLIParser::ProfilingType>::type>(prof) << std::endl;

        CollectPerfData perfDataCollector(cliParser.getPerfOpts(), cliParser.getDuration(), prof, cliParser.getCmdToExecute(), cliParser.getPidToRecord());
        perfDataCollector.initialize();
        perfDataCollector.recordPerf();
        std::string perfData = perfDataCollector.retriveData();

        FlameGraphGenerator flameGraphGenerator(perfData, prof);
        flameGraphGenerator.generateFlameGraph("output.svg");

        std::cout << "Flamegraph successfully generated as 'output.svg'" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} */
int main(int argc, char** argv) {
    try {
        CLIParser cliParser(argc, argv);
        cliParser.parseArgs();
        
        CollectPerfData perfDataCollector(cliParser.getPerfOpts(), cliParser.getDuration(), cliParser.getProfType(), cliParser.getCmdToExecute(), cliParser.getPidToRecord());
        perfDataCollector.initialize();
        

        if (cliParser.shouldRecordAllProfiles()) {
            perfDataCollector.recordAllProfiles();
        } else {
            CLIParser::ProfilingType prof = cliParser.getProfType();
            perfDataCollector.recordPerf();
            std::string perfData = perfDataCollector.retriveData();
            std::string fgName = perfDataCollector.genFileName();

            FlameGraphGenerator flameGraphGenerator(perfData, prof);
            
            flameGraphGenerator.generateFlameGraph(fgName + ".svg");
        }

        std::cout << "Flamegraph(s) successfully generated" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
