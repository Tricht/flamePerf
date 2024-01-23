#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <set>
#include <string>
#include <vector>

class CLIParser {
 public:
  CLIParser(int argc, char** argv);
  enum class ProfilingType { CPU, OffCPU, Memory, IO, Network, Default };

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
  std::string perfOpts = "";
  int duration = 0;
  ProfilingType profType = ProfilingType::Default;
  std::string cmdToExecute = "";
  int pidToRecord = -1;
  bool allProfiles = false;
  std::set<ProfilingType> selectedProfilingTypes;
};

#endif