#!/bin/bash -e

if [ "${GEN}" = "" ]; then
    GEN='Unix Makefiles'
fi

source ${0%/*}/common.sh "$@"
shift $USED_ARGS

pushd "$BUILDDIR"
echo cmake -G "$GEN" -DFB_ROOT="${FB_ROOT}" "$@" "${PROJDIR}"
#cmake -G "$GEN" -DFB_PROJECTS_DIR="${PROJDIR}" "$@" "${FB_ROOT}"
cmake -G "$GEN" -DFB_ROOT="${FB_ROOT}" "$@" "${PROJDIR}"
if [ "$?" -ne 0 ]; then
    echo "CMake failed. Please check error messages"
    popd > /dev/null
    exit
else
    popd
fi
