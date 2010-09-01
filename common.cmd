@echo off

set FB_ROOT=%~d0%~p0
set RUN_FROM=%CD%

IF /I "%1"=="" goto defaults
set PROJDIR=%1

IF /I "%2"=="" goto default_build
set BUILDDIR=%2

echo "Provided"
goto finish

:defaults
echo "Defaults"
set PROJDIR=%RUN_FROM%\projects

:default_build
IF /I "%1"=="examples" goto examples
set BUILDDIR=%RUN_FROM%\build
goto finish

:examples
set BUILDDIR=%RUN_FROM%\buildex

:finish
echo Using projects in: %PROJDIR%
echo Generating build files in: %BUILDDIR%
echo NOTE: The build files in %BUILDDIR% should *NEVER* be modified directly.
echo Make project changes in cmake and re-run this script.

mkdir %BUILDDIR% > NUL
