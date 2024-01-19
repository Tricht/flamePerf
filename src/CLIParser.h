#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>
#include <vector>
#include <set>

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
    int getPidToRecord();
    bool shouldRecordAllProfiles() const;
    void addProfilingType(ProfilingType type);
    const std::set<ProfilingType>& getSelectedProfilingTypes() const;


private:
    int argc;
    char** argv;
    std::string perfOpts;
    int duration;
    ProfilingType profType;
    std::string cmdToExecute;
    int pidToRecord;
    bool allProfiles;
    std::set<ProfilingType> selectedProfilingTypes;

};

#endif