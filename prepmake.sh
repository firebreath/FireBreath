#!/bin/bash

if [ "${GEN}" = "" ]; then
    GEN='Unix Makefiles'
fi

source ${0%/*}/common.sh "$@"

pushd "$BUILDDIR"
cmake -G "$GEN" -DFB_PROJECTS_DIR="${PROJDIR}" "$@" "${FB_ROOT}"
popd
