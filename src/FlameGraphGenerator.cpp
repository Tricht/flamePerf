#include "FlameGraphGenerator.h"

#include <fstream>

#include <array>

#include <memory>

#include <iostream>

#include <sstream>

#include <chrono>

#include <iomanip>

FlameGraphGenerator::FlameGraphGenerator() {}

FlameGraphGenerator::FlameGraphGenerator(const std::string &perfData, CLIParser::ProfilingType profType) : perfData(perfData), profType(profType) {}

void FlameGraphGenerator::generateFlameGraph(const std::string &outputPath)
{
    std::string collapsedData = collapseStack();
    std::cout << "Collapsed Data Length: " << collapsedData.length() << std::endl;

    std::string tempCollOut = "tempCollOut.tmp";

    std::ofstream tempOutFile(tempCollOut);
    if (!tempOutFile.is_open())
    {
        throw std::runtime_error("Unable to open temp file");
    }

    tempOutFile << collapsedData;
    tempOutFile.close();

    std::string colorPalette;
    switch (profType)
    {
    case CLIParser::ProfilingType::CPU:
        colorPalette = "--color=js";
        break;
    case CLIParser::ProfilingType::OffCPU:
        colorPalette = "--color=io";
        break;
    case CLIParser::ProfilingType::Memory:
        colorPalette = "--color=mem";
        break;
    case CLIParser::ProfilingType::IO:
        colorPalette = "--color=yellow";
        break;
    case CLIParser::ProfilingType::Network:
        colorPalette = "--color=aqua";
        break;
    }

    std::string flameGraphCmd = "perl ../FlameGraph/flamegraph.pl " + colorPalette + " " + tempCollOut + " > " + outputPath + ".svg";

    int result = system(flameGraphCmd.c_str());
    if (result != 0)
    {
        std::cerr << "FlameGraph Command: " << flameGraphCmd << std::endl;
        throw std::runtime_error("Failed to generate flamegraph");
    }

    if (remove(tempCollOut.c_str()) != 0)
    {
        std::cerr << "Failed to delete temp file" << std::endl;
    }
}

void FlameGraphGenerator::generateCombinedHtml(const std::vector<std::string> &fileNames, const std::string &diffFileName = "")
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "./results/flamegraphs_" << std::put_time(std::localtime(&now_c), "%d%m%Y_%H%M%S") << ".html";
    std::string htmlFileName = ss.str();

    std::ifstream templateFile("../utils/template.html");
    std::stringstream buffer;
    buffer << templateFile.rdbuf();
    std::string htmlContent = buffer.str();
    templateFile.close();

    std::string tabs, content;

    if (!diffFileName.empty()) {
        tabs += "<div class='tab active' onclick='openTab(event, \"tabDiff\")'>Differential</div>";
        content += "<div id='tabDiff' class='content flamegraph active'>";
        content += "<object data='" + diffFileName + "' type='image/svg+xml' id='flamegraphDiff' width='100%' height='100%'></object>";
        content += "</div>";
    }

    // Tabs for the original FlameGraphs
    for (size_t i = 0; i < fileNames.size(); ++i) {
        // naming for tabs
        size_t lastSlash = fileNames[i].find_last_of("/\\");
        std::string fileName = (lastSlash != std::string::npos) ? fileNames[i].substr(lastSlash + 1) : fileNames[i];
        size_t dotPos = fileName.find_last_of('.');
        std::string profileName = (dotPos != std::string::npos) ? fileName.substr(0, dotPos) : fileName;

        // edit path of svg files
        std::string svgFilePath = fileNames[i];
        if (lastSlash != std::string::npos)
        {
            svgFilePath = svgFilePath.substr(lastSlash + 1);
        }

        // create tab and content area for specific flamegraph
        tabs += "<div class='tab" + std::string(i == 0 ? " active" : "") + "' onclick='openTab(event, \"tab" + std::to_string(i + 1) + "\")'>" + profileName + "</div>";
        content += "<div id='tab" + std::to_string(i + 1) + "' class='content flamegraph" + std::string(i == 0 ? " active" : "") + "'>";
        content += "<object data='" + svgFilePath + "' type='image/svg+xml' id='flamegraph" + std::to_string(i + 1) + "' width='100%' height='100%'></object>";
        content += "</div>";

    }

    size_t tabsPos = htmlContent.find("{{tabs}}");
    if (tabsPos != std::string::npos) {
        htmlContent.replace(tabsPos, 8, tabs);
    }

    size_t contentPos = htmlContent.find("{{content}}");
    if (contentPos != std::string::npos) {
        htmlContent.replace(contentPos, 11, content);
    }

    std::ofstream outputFile(htmlFileName);
    outputFile << htmlContent;
    outputFile.close();
}

std::string FlameGraphGenerator::collapseStack()
{
    std::string collapseScriptPath;

    switch (profType)
    {
    case CLIParser::ProfilingType::OffCPU:
        collapseScriptPath = "../FlameGraph/stackcollapse-perf.pl";
        break;
    default:
        collapseScriptPath = "../FlameGraph/stackcollapse-perf.pl";
        break;
    }

    std::string tempPerfScript = "tempPerfScript.tmp";

    std::ofstream tempScriptFile(tempPerfScript);
    if (!tempScriptFile.is_open())
    {
        throw std::runtime_error("Failed to open temp file for perf data");
    }

    tempScriptFile << perfData;
    tempScriptFile.close();

    std::string cmd = "perl " + collapseScriptPath + " --all " + tempPerfScript;

    std::cout << cmd << std::endl;

    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed");
    }

    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
    }

    if (remove(tempPerfScript.c_str()) != 0)
    {
        std::cerr << "Warning: Failed to delete temp perf data file" << std::endl;
    }

    return result;
}

void FlameGraphGenerator::generateDiffFlameGraph(const std::string &file1, const std::string &file2, const std::string &outputPath)
{
    std::string foldCmd1 = "perl ../FlameGraph/stackcollapse-perf.pl " + file1 + " > out.folded1";
    std::string foldCmd2 = "perl ../FlameGraph/stackcollapse-perf.pl " + file2 + " > out.folded2";
    std::string diffFileName = outputPath + "_diff.svg";
    std::string diffCmd = "perl ../FlameGraph/difffolded.pl out.folded1 out.folded2 | ../FlameGraph/flamegraph.pl > " + diffFileName;

    system(foldCmd1.c_str());
    system(foldCmd2.c_str());
    int result = system(diffCmd.c_str());
    if (result != 0)
    {
        throw std::runtime_error("Failed to generate differential Flamegraph");
    }

    if (remove("out.folded1") != 0) {
        std::cerr << "Warning: Failed to delete folded data file 1" << std::endl;
    }
    if (remove("out.folded2") != 0) {
        std::cerr << "Warning: Failed to delete folded data file 2" << std::endl;
    }

    size_t lastSlashIndex = diffFileName.find_last_of("/\\");
    std::string diffFileOnly = (lastSlashIndex != std::string::npos) ? diffFileName.substr(lastSlashIndex + 1) : diffFileName;

    std::vector<std::string> originalFileNames = {file1 + ".svg", file2 + ".svg"};
    generateCombinedHtml(originalFileNames, diffFileOnly);
}