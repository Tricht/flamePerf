#include "ProfilingManager.h"
#include <iostream>

ProfilingManager::ProfilingManager(const std::string &filePath) : filePath(filePath) {}

void ProfilingManager::saveProfile(const std::string &name, const std::string &events)
{
    std::ofstream file(filePath, std::ios::app);
    if (file.is_open())
    {
        file << name << "=" << events << std::endl;
    }

    std::cout << "Saved profile: " + name << std::endl;
}

std::map<std::string, std::string> ProfilingManager::loadProfiles()
{
    std::map<std::string, std::string> profiles;

    std::ifstream file(filePath);
    std::string line;

    while (std::getline(file, line))
    {
        auto deliPos = line.find("=");
        if (deliPos != std::string::npos)
        {
            std::string profName = line.substr(0, deliPos);
            std::string profEvents = line.substr(deliPos + 1);
            profiles[profName] = profEvents;
        }
    }

    return profiles;
}
