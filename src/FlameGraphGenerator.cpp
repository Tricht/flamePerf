#include "FlameGraphGenerator.h"
#include <fstream>
#include <array>
#include <memory>
#include <iostream>
#include <sstream>

FlameGraphGenerator::FlameGraphGenerator() {}

FlameGraphGenerator::FlameGraphGenerator(const std::string &perfData, CLIParser::ProfilingType profType)
    : perfData(perfData), profType(profType) {}

void FlameGraphGenerator::generateFlameGraph(const std::string &outputPath)
{
    std::string collapsedData = collapseStack();
    std::cout << "Collapsed Data Length: " << collapsedData.length() << std::endl;

    std::string tempCollOut = "tempCollOut.tmp";

    std::ofstream tempOutFile(tempCollOut);
    if (!tempOutFile.is_open()) {
        throw std::runtime_error("Unable to open temp file");
    }

    tempOutFile << collapsedData;
    tempOutFile.close();

    std::string colorPalette;
    switch(profType) {
        case CLIParser::ProfilingType::CPU:
            colorPalette = "--color=java";
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
    
}

    std::string flameGraphCmd = "perl ../FlameGraph/flamegraph.pl " + colorPalette + " " + tempCollOut + " > " + outputPath;

    int result = system(flameGraphCmd.c_str());
    if (result != 0) {
        std::cerr << "FlameGraph Command: " << flameGraphCmd << std::endl;
        throw std::runtime_error("Failed to generate flamegraph");
    }

    if (remove(tempCollOut.c_str()) != 0) {
        std::cerr << "Failed to delete temp file" << std::endl;
    }
}

void FlameGraphGenerator::generateCombinedHtml(const std::vector<std::string> &fileNames)
{
    std::ifstream templateFile("../utils/template.html");
    std::stringstream buffer;
    buffer << templateFile.rdbuf();
    std::string htmlContent = buffer.str();
    templateFile.close();

    std::string tabs, content;

    for (size_t i = 0; i < fileNames.size(); ++i) {
        tabs += "<div class='tab" + std::string(i == 0 ? " active" : "") + "' onclick='openTab(event, \"tab" + std::to_string(i + 1) + "\")'>Tab " + std::to_string(i + 1) + "</div>";
        content += "<div id='tab" + std::to_string(i + 1) + "' class='content" + std::string(i == 0 ? " active" : "") + "'>";

        std::ifstream svgFile(fileNames[i]);
        std::string svgCode((std::istreambuf_iterator<char>(svgFile)), std::istreambuf_iterator<char>());
        svgFile.close();

        content += svgCode;
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

    std::ofstream outputFile("combined_fg.html");
    outputFile << htmlContent;
    
    outputFile.close();
    
    /* std::ofstream htmlFile("combined_fg.html");
    if (!htmlFile.is_open()) {
        throw std::runtime_error("Unable to open combined HTML file");
    }

    htmlFile << "<html><body>";
    for (const auto& name : fileNames) {
        std::ifstream svgFile(name);
        if (!svgFile.is_open()) {
            throw std::runtime_error("Unable to open flamegraph file: " + name);
        }

        std::string svgCode((std::istreambuf_iterator<char>(svgFile)), std::istreambuf_iterator<char>());

        svgFile.close();

        htmlFile << "<div>" << svgCode << "</div>";
    }

    htmlFile << "</body></html>";
    htmlFile.close(); */ 
}

std::string FlameGraphGenerator::collapseStack()
{
    std::string collapseScriptPath;

    switch(profType) {
        case CLIParser::ProfilingType::OffCPU:
            collapseScriptPath = "../FlameGraph/stackcollapse-perf.pl";
            break;
        default:
            collapseScriptPath = "../FlameGraph/stackcollapse-perf.pl";
            break;            
    }

    std::string tempPerfScript = "tempPerfScript.tmp";

    std::ofstream tempScriptFile(tempPerfScript);
    if (!tempScriptFile.is_open()) {
        throw std::runtime_error("Failed to open temp file for perf data");
    }

    tempScriptFile << perfData;
    tempScriptFile.close();

    std::string cmd = "perl " + collapseScriptPath + " --all " + tempPerfScript;

    std::cout << cmd << std::endl;

    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed");
    }

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }

    if (remove(tempPerfScript.c_str()) != 0) {
    std::cerr << "Warning: Failed to delete temp perf data file" << std::endl;
}

    return result; 
}
