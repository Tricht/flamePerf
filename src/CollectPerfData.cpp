#include "CollectPerfData.h"

CollectPerfData::CollectPerfData(const std::string &options):options(options), collecting(false){}

void CollectPerfData::startCollection()
{
}

std::string CollectPerfData::retriveData()
{
    return std::string();
}

std::string CollectPerfData::executePerf(const std::string &command)
{
    return std::string();
}
