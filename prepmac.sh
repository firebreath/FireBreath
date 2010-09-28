#!/bin/bash

GEN='Xcode'

source ${0%/*}/common.sh "$@"

ARCH=-DCMAKE_OSX_ARCHITECTURES="i386;x86_64"

pushd "$BUILDDIR"
cmake -G "$GEN" -DPROJECTS_DIR="${PROJDIR}" ${ARCH} "$@" "${FB_ROOT}"
if [ "$?" -ne 0 ]; then
    echo "CMake failed. Please check error messages"
    popd > /dev/null
    exit
else
    popd

    if [ -f "cmake/patch_xcode.py" ]; then
        while read target proj
        do
            python cmake/patch_xcode.py -f "$proj" -t "$target"
        done < $BUILDDIR/xcode_patch_desc.txt
    fi
fi
