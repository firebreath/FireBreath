#!/bin/bash

RUN_FROM=`pwd`
FB_ROOT_REL=${0%/*}
FB_ROOT=`cd $FB_ROOT_REL; pwd`

if [ "$1" = "" ]; then
    BUILDDIR=${RUN_FROM}/build
    PROJDIR=${RUN_FROM}/projects
elif [ "$2" = "" ]; then
    if [ "$1" = "examples" ]; then
        BUILDDIR=${RUN_FROM}/buildex
    else
        BUILDDIR=${RUN_FROM}/build
    fi
    PROJDIR=`cd $1; pwd`
else
    PROJDIR=`cd $1; pwd`
    mkdir -p "$2"
    BUILDDIR=`cd $2; pwd`
fi
mkdir -p "$BUILDDIR"

echo Using projects in: $PROJDIR
echo Generating build files in: $BUILDDIR
echo NOTE: The build files in $BUILDDIR should *NEVER* be modified directly.  Make changes in cmake and re-run this script.

# Remove the first two arguments; the remainder are cmake arguments
shift 2
