#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>
#include <vector>

class CLIParser
{

public:
    CLIParser(int argc, char** argv);
    enum class ProfilingType {CPU, OffCPU, Memory, IO, Default};

    void parseArgs();

    std::string getPerfOpts();
    int getDuration();
    ProfilingType getProfType() const;
    std::string getCmdToExecute() const;


private:
    int argc;
    char** argv;
    std::string perfOpts;
    int duration;
    ProfilingType profType;
    std::string cmdToExecute;

};

#endif