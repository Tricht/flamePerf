#!/bin/bash

echo "Building flamePerf framework"

mkdir -p ../build && cd ../build

cmake ..
make 