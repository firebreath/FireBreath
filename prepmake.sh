#!/bin/bash

GEN='Unix Makefiles'

if [ "$1" = "examples" ]; then
    echo Building example projects
    cmake -D GEN="${GEN}" -D BUILD_EXAMPLES="YES" -P cmake/genproject.cmake
else
    echo Building projects
    cmake -D GEN="${GEN}" -P cmake/genproject.cmake
fi
