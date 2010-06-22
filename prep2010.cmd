@echo off

set GEN="Visual Studio 10"

IF /I "%1"=="examples" GOTO examples

:normal
echo Building projects
set BUILDDIR=build
goto runcmake

:examples
echo Building example projects
set BUILDDIR=buildex
set EXAMPLES=-DBUILD_EXAMPLES:BOOL=YES
goto runcmake

:runcmake
mkdir %BUILDDIR%
pushd %BUILDDIR%
cmake -G %GEN% %EXAMPLES% ..
popd
