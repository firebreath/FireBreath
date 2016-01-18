#!/bin/bash -e

if [ "${GEN}" = "" ]; then
    GEN='Unix Makefiles'
fi

source "${0%/*}/common.sh" "$@"
shift $USED_ARGS

pushd "$BUILDDIR"
cmake -G "$GEN" -DFB_ROOT="${FB_ROOT}" "$@" "${PROJDIR}"
if [ "$?" -ne 0 ]; then
    echo "CMake failed. Please check error messages"
    popd > /dev/null
    exit
else
    popd
fi
