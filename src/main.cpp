#include "CollectPerfData.h"
#include "CLIParser.h"
#include "FlameGraphGenerator.h"
#include <unistd.h>
#include <iostream>

int main(int argc, char** argv) 
{
    try {
        CLIParser cli(argc, argv);
        cli.parseArgs();

        std::string opt = cli.getPerfOpts();
        int dur = cli.getDuration();
        CLIParser::ProfilingType prof = CLIParser::ProfilingType::CPU;
        std::cout << opt << std::endl;
        std::cout << dur << std::endl;


        CollectPerfData cpd(opt, dur, prof);
        cpd.recordPerf();
        std::string perfData = cpd.retriveData();
        std::cout << perfData << std::endl;

        FlameGraphGenerator fgg(perfData, prof);
        //fgg.generateFlameGraph("test1.svg");


    return 0;    
    } catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}