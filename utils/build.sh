#!/bin/bash

cd ..
cmake -S src/ -B build/
cd build/
make
