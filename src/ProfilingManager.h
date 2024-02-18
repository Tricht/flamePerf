// ProfilingManager.h

#ifndef PROFILING_MANAGER_H
#define PROFILING_MANAGER_H

#include <string>
#include <map>
#include <fstream>

class ProfilingManager
{
public:
    ProfilingManager(const std::string &filePath);

    void saveProfile(const std::string &name, const std::string &events);

    std::map<std::string, std::string> loadProfiles();

private:
    std::string filePath;
};

#endif