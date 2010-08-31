#!/bin/bash

if [ "${GEN}" = "" ]; then
    GEN='Unix Makefiles'
fi

source ${0%/*}/common.sh "$@"

pushd "$BUILDDIR"
cmake -G "$GEN" -DPROJECTS_DIR="${PROJDIR}" "$@" "${FB_ROOT}"
popd
