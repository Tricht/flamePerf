// CLIParser.h
// Defines the CLIParser class which is responsible for parsing
// command-line arguments for the flamePerf framework.

#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <set>
#include <string>
#include <vector>
#include <map>

// CLIParser class provides functions to parse and store cli arguments
class CLIParser
{
public:
  // Constructor: initializes the parser
  CLIParser(int argc, char **argv);

  // enum to represent the different Profiling Types
  enum class ProfilingType
  {
    CPU,
    OffCPU,
    Memory,
    IO,
    Network,
    Default,
    Custom
  };

  // Parses the cli arguments and sets flags/values.
  bool parseArgs();

  // Returns the options for perf record
  std::string getPerfOpts();

  // Returns the duration for which perf record should run
  int getDuration();

  // Returns the selected Profiling Type
  ProfilingType getProfType() const;

  // Returns the command that should be executed and recorded
  std::string getCmdToExecute() const;

  // Returns the pid that should be executed and recorded
  int getPidToRecord();

  // Checks if all available Profiling Types should be recorded
  bool shouldRecordAllProfiles() const;

  // Adds a Profiling Type to the set of selected types
  void addProfilingType(ProfilingType type);

  // Returns the set of selected types
  const std::set<ProfilingType> &getSelectedProfilingTypes() const;

  // Cheks if a differential Flamegraph should be created
  bool shouldCreateDiffGraph() const;

  // Get first diff file
  std::string getDiffFile1() const;

  // Get second diff file
  std::string getDiffFile2() const;

  // set custom file name from user input
  void setCustomFileName(const std::string name);

  // get custom file name
  std::string getCustomFileName();

private:
  int argc;
  char **argv;
  std::string perfOpts = "";
  int duration = 0;
  ProfilingType profType = ProfilingType::Default;
  std::string cmdToExecute = "";
  int pidToRecord = -1;
  bool allProfiles = false;
  std::set<ProfilingType> selectedProfilingTypes;
  bool diffFlag = false;
  std::string diffFile1, diffFile2;
  std::string customFileName;

  // show help/usagem message
  void showHelp();
};

#endif