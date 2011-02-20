#!/bin/bash

GEN='Xcode'

source "${0%/*}/common.sh" "$@"

ARCH=-DCMAKE_OSX_ARCHITECTURES="i386;x86_64"

pushd "$BUILDDIR"
cmake -G "$GEN" -DFB_PROJECTS_DIR="${PROJDIR}" ${ARCH} "$@" "${FB_ROOT}"
if [ "$?" -ne 0 ]; then
    echo "CMake failed. Please check error messages"
    popd > /dev/null
    exit
else
    popd

    if [ -f "${FB_ROOT}/cmake/patch_xcode.py" ]; then
        while read target proj
        do
            python "${FB_ROOT}/cmake/patch_xcode.py" -f "$proj" -t "$target"
        done < "$BUILDDIR/xcode_patch_desc.txt"
    else
        echo "Could not find patch_xcode at ${FB_ROOT}/cmake/patch_xcode.py!  Project will not work!"
    fi
fi
