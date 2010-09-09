@echo off

set FB_ROOT=%~dp0
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
IF EXIST %PROJDIR% GOTO direxists
echo ERROR: Project directory %PROJDIR% does not exist.  usage: prep[ver].cmd [project dir] [build dir]
exit 1

:direxists
REM We need the full path of the directories
REM This is a hack, but it works
pushd %PROJDIR%
set PROJDIR=%CD%
popd
mkdir %BUILDDIR% > NUL
pushd %BUILDDIR%
set BUILDDIR=%CD%
popd

echo Using projects in: %PROJDIR%
echo Generating build files in: %BUILDDIR%
echo NOTE: The build files in %BUILDDIR% should *NEVER* be modified directly.
echo Make project changes in cmake and re-run this script.

