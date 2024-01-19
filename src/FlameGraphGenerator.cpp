#include "FlameGraphGenerator.h"

#include <fstream>

#include <array>

#include <memory>

#include <iostream>

#include <sstream>

#include <chrono>

#include <iomanip>

FlameGraphGenerator::FlameGraphGenerator() {}

FlameGraphGenerator::FlameGraphGenerator(const std::string & perfData, CLIParser::ProfilingType profType): perfData(perfData), profType(profType) {}

void FlameGraphGenerator::generateFlameGraph(const std::string & outputPath) {
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
    switch (profType) {
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

    std::string flameGraphCmd = "perl ../FlameGraph/flamegraph.pl " + colorPalette + " " + tempCollOut + " > " + outputPath + ".svg";

    int result = system(flameGraphCmd.c_str());
    if (result != 0) {
        std::cerr << "FlameGraph Command: " << flameGraphCmd << std::endl;
        throw std::runtime_error("Failed to generate flamegraph");
    }

    if (remove(tempCollOut.c_str()) != 0) {
        std::cerr << "Failed to delete temp file" << std::endl;
    }
}

void FlameGraphGenerator::generateCombinedHtml(const std::vector < std::string > & fileNames) {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "./results/flamegraphs_" << std::put_time(std::localtime( & now_c), "%d%m%Y_%H%M%S") << ".html";
    std::string htmlFileName = ss.str();

    std::ifstream templateFile("../utils/template.html");
    std::stringstream buffer;
    buffer << templateFile.rdbuf();
    std::string htmlContent = buffer.str();
    templateFile.close();

    std::string tabs, content;

    for (size_t i = 0; i < fileNames.size(); ++i) {
        // naming for tabs
        size_t lastSlash = fileNames[i].find_last_of("/\\");
        std::string fileName = (lastSlash != std::string::npos) ? fileNames[i].substr(lastSlash + 1) : fileNames[i];
        size_t dotPos = fileName.find_last_of('.');
        std::string profileName = (dotPos != std::string::npos) ? fileName.substr(0, dotPos) : fileName;

        // create tab and content area for specific flamegraph
        tabs += "<div class='tab" + std::string(i == 0 ? " active" : "") + "' onclick='openTab(event, \"tab" + std::to_string(i + 1) + "\")'>" + profileName + "</div>";
        content += "<div id='tab" + std::to_string(i + 1) + "' class='content flamegraph" + std::string(i == 0 ? " active" : "") + "'>";

        // read svg file
        std::ifstream svgFile(fileNames[i]);
        std::stringstream svgBuffer;
        svgBuffer << svgFile.rdbuf();
        svgFile.close();
        // ...and delete javscript
        std::string svgCode = svgBuffer.str();
        size_t scriptStart = svgCode.find("<script");
        if (scriptStart != std::string::npos) {
            size_t scriptEnd = svgCode.find("</script>", scriptStart);
            svgCode.erase(scriptStart, (scriptEnd - scriptStart) + 9); // </script> = 9
        }
        // ...and delete onload function, will be replaced by own js
        size_t onloadStart = svgCode.find("onload=\"init(evt)\"");
        if (onloadStart != std::string::npos) {
            svgCode.erase(onloadStart, std::string("onload=\"init(evt)\"").length());
        }

        size_t svgStart = svgCode.find("<svg");
        if (svgStart != std::string::npos) {
            svgCode.insert(svgStart + 4, " class='flamegraph'");
        }

        content += svgCode;
        content += "</div>";

        /* std::ifstream svgFile(fileNames[i]);
        std::string svgCode((std::istreambuf_iterator<char>(svgFile)), std::istreambuf_iterator<char>());
        svgFile.close();

        content += svgCode;
        content += "</div>"; */
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

std::string FlameGraphGenerator::collapseStack() {
    std::string collapseScriptPath;

    switch (profType) {
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

    std::array < char, 128 > buffer;
    std::string result;
    std::shared_ptr < FILE > pipe(popen(cmd.c_str(), "r"), pclose);
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