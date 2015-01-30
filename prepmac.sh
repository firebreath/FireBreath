#!/bin/bash -e

GEN='Xcode'

source "${0%/*}/common.sh" "$@"
shift $USED_ARGS

ARCH=-DCMAKE_OSX_ARCHITECTURES="${CMAKE_OSX_ARCHITECTURES:-i386;x86_64}"

pushd "$BUILDDIR"
echo cmake -G "$GEN" -DFB_ROOT="${FB_ROOT}" ${ARCH} "$@" "${PROJDIR}"
cmake -G "$GEN" -DFB_ROOT="${FB_ROOT}" ${ARCH} "$@" "${PROJDIR}"
if [ "$?" -ne 0 ]; then
    echo "CMake failed. Please check error messages"
    popd > /dev/null
    exit
else
    popd
fi
