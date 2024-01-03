#!/bin/bash

echo "Installation of perf and Flamegraphs"

if ! command -v perf &> /dev/null
then
    echo "perf not installed, will install it"
    sudo apt-get update
    sudo apt-get install -y linux-tools-common linux-tools-generic linux-tools-`uname -r`
else
    echo "perf is already installed"
fi

FLAME_DIR="../FlameGraph"

if [ ! -d "$FLAME_DIR" ]; then
    echo "Clone FlameGraph Repository..."
    git clone https://github.com/brendangregg/FlameGraph.git $FLAME_DIR
else
    echo "FlameGraph directory already exists"
fi