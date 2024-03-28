#!/bin/bash

exec rm CMakeCache.txt & cmake . -DCMAKE_BUILD_TYPE=Release
