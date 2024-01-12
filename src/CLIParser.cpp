#include "CLIParser.h"
#include <iostream>

CLIParser::CLIParser(int argc, char **argv)
:argc(argc), argv(argv), perfOpts(""), duration(0), profType(ProfilingType::Default), cmdToExecute(""){}

void CLIParser::parseArgs()
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // options for perf
        if (arg == "-o" || arg == "--perf-options") {
            perfOpts = argv[++i];

        // how long should perf run    
        } else if (arg == "-d" || arg == "--duration") {
            try {
                duration = std::stoi(argv[++i]);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid duration value" << std::endl;
                throw;
            }

        //which profiling type should be used    
        } else if (arg == "-p" || arg == "--profiling-type") {
            std::string type = argv[++i];
            if(type == "cpu") {
                profType = ProfilingType::CPU;
            } else if (type == "offcpu") {
                profType = ProfilingType::OffCPU;
            } else if (type == "mem") {
                profType = ProfilingType::Memory;
            } else if (type == "io") {
                profType = ProfilingType::IO;
            } else {
                std::cerr << "Invalid profiling type" << std::endl;
            }

        // instead of duration, measure a command    
        } else if (arg == "-c" || arg == "--cmd") {
            cmdToExecute = argv[++i];

        // help message    
        } else {
            std::cout << "Usage: " << std::endl;
        }
    }
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
