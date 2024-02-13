cmake_minimum_required(VERSION 3.10)
project(flamePerf)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

set(SOURCE_FILES
    src/main.cpp
    src/CLIParser.cpp
    src/CollectPerfData.cpp
    src/FlameGraphGenerator.cpp
    
)

set(HEADER_FILES
    src/CLIParser.h
    src/CollectPerfData.h
    src/FlameGraphGenerator.h
)

add_executable(flamePerf ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(flamePerf ${CMAKE_THREAD_LIBS_INIT})
