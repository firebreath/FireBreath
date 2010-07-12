#!/bin/bash

GEN='Unix Makefiles'

if [ "$1" = "examples" ]; then
    echo Building example projects
    BUILDDIR=buildex
    EXAMPLES='-DBUILD_EXAMPLES:BOOL=YES'
else
    echo Building projects
    BUILDDIR=build
fi

mkdir -p "$BUILDDIR"
pushd "$BUILDDIR"
cmake -G "$GEN" "$EXAMPLES" ..
popd
