#!/bin/sh

cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-32-linux.cmake $* ../..
