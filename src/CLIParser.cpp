#include "CLIParser.h"

#include <iostream>

#include <sstream>

CLIParser::CLIParser(int argc, char ** argv): argc(argc), argv(argv), perfOpts(""), duration(0), profType(ProfilingType::Default), cmdToExecute(""), pidToRecord(-1), allProfiles(false) {}

void CLIParser::parseArgs() {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-o" || arg == "--perf-options") // options for perf
        {
            perfOpts = argv[++i];
        } else if (arg == "-d" || arg == "--duration") // how long should perf run
        {
            try {
                duration = std::stoi(argv[++i]);
            } catch (const std::invalid_argument & e) {
                std::cerr << "Invalid duration value" << std::endl;
                throw;
            }
        } else if (arg == "-p" || arg == "--profiling-type") // which profiling type should be used
        {
            std::string type = argv[++i];
            if (type == "cpu") {
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
        } else if (arg == "-c" || arg == "--cmd") // instead of duration, measure a command...
        {
            cmdToExecute = argv[++i];
        } else if (arg == "--pid") // ...or pid
        {
            try {
                pidToRecord = std::stoi(argv[++i]);
            } catch (const std::invalid_argument & e) {
                std::cerr << "Invalid pid value" << std::endl;
                throw;
            }
        } else if (arg == "--all-profiles") // record all profiles
        {
            allProfiles = true;
        } else if (arg == "--profile-types") // record chosen profiles
        {
            std::string types = argv[++i];
            std::istringstream typesStream(types);
            std::string type;
            while (std::getline(typesStream, type, ',')) {
                if (type == "cpu") {
                    addProfilingType(ProfilingType::CPU);
                } else if (type == "offcpu") {
                    addProfilingType(ProfilingType::OffCPU);
                } else if (type == "mem") {
                    addProfilingType(ProfilingType::Memory);
                } else if (type == "io") {
                    addProfilingType(ProfilingType::IO);
                } else {
                    std::cerr << "Invalid profiling type: " << type << std::endl;
                }
            }
        } else // help message
        {
            std::cout << "Usage: " << std::endl;
        }
    }
}

std::string CLIParser::getPerfOpts() {
    return perfOpts;
}

int CLIParser::getDuration() {
    return duration;
}

CLIParser::ProfilingType CLIParser::getProfType() const {
    return profType;
}

std::string CLIParser::getCmdToExecute() const {
    return cmdToExecute;
}

int CLIParser::getPidToRecord() {
    return pidToRecord;
}

bool CLIParser::shouldRecordAllProfiles() const {
    return allProfiles;
}

const std::set < CLIParser::ProfilingType > & CLIParser::getSelectedProfilingTypes() const {
    return selectedProfilingTypes;
}

void CLIParser::addProfilingType(ProfilingType type) {
    selectedProfilingTypes.insert(type);
}