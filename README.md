# flamePerf

`flamePerf` is a performance analysis framework designed to simplify the process of collecting and visualizing profiling data with flame graphs. Utilizing perf for data collection and integrating flame graph generation, it aids in identifying performance bottlenecks by visualizing call stacks.

## Prerequisites

- CMake
- A C++17 compatible compiler

## Install and build the framework

1. Clone the repository and navigate into the project directory:

```
git clone (this repository)
cd flamePerf/
```

2. Navigate to the scripts folder and install Linux perf and clone the FlameGraphs repository with the install_uitls.sh script:

```
cd scripts/
./install_utils.sh
```

3. Build the framework by using CMake and the build.sh script:

```
./build.sh
```

## Usage

To use the framework, you have a few options depending on what you want to analyze. Here are some common scenarios:

1. Profiling an executable:

```
./flamePerf -c "<path/to/executable> <args>"
```

2. Profiling a running process by PID:

```
./flamePerf --pid <PID>
```

3. Generating a differential flame graph between two profiling sessions:

```
./flamePerf --diff <path/to/generated_perf_script_file1> <path/to/generated_perf_script_file2>
```

## Customizing Profiling

`flamePerf` allows customization of the profiling process through various flags:

- -o OR --options "\<options\>": Custom options to pass to 'perf record'.
- -d OR --duration \<seconds\>: Duration for the profiling session.
- -p OR --profile-types \<types\>: Specify types of profiles to record (cpu, offcpu, mem, io, net).

## Viewing Flame Graphs

After running a profiling session, `flamePerf` generates an SVG file containing the flame graph. Additionally, an HTML file is generated containing different tabs with the flame graphs when you choose more than one profile type. This HTML file is also available when generating differential flame graphs.
