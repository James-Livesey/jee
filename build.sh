#!/bin/bash

mkdir -p build

gcc src/*.c src/*/*.c -lm -o build/jee