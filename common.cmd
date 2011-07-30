@echo off

set FB_ROOT="%~dp0"
set RUN_FROM=%CD%

set _SHIFTTOK=0

set _NPARAM=%~1
IF /I "%_NPARAM%"=="" goto defaults
IF /I "%_NPARAM:~0,1%"=="-" goto defaults
set PROJDIR=%1
shift
set _SHIFTTOK=1

set _NPARAM=%~1
IF /I "%_NPARAM%"=="" goto default_build
IF /I "%_NPARAM:~0,1%"=="-" goto default_build
set BUILDDIR=%1
shift
set _SHIFTTOK=2

goto finish

:defaults
set PROJDIR="%RUN_FROM%\projects"

:default_build
IF /I %PROJDIR%==examples goto examples
set BUILDDIR="%RUN_FROM%\build"
goto finish

:examples
echo Building examples
set BUILDDIR="%RUN_FROM%\buildex"

:finish
IF EXIST %PROJDIR% GOTO direxists
echo ERROR: Project directory %PROJDIR% does not exist.  usage: prep[ver].cmd [project dir] [build dir]
exit /B 2
goto end

:direxists
REM We need the full path of the directories
REM This is a hack, but it works
pushd ""%PROJDIR%"" > NUL
set PROJDIR="%CD%"
popd > NUL
mkdir ""%BUILDDIR%""
pushd ""%BUILDDIR%"" > NUL
if %errorlevel% == 1 goto error
set BUILDDIR="%CD%"
popd > NUL

for /f "tokens=%_SHIFTTOK%*" %%a in ('echo.%*') do set CMAKE_PARAMS=%%b
goto finished

:error
echo "Could not create build directory %BUILDDIR%"
exit /B 2

:finished
echo Using projects in: %PROJDIR%
echo Generating build files in: %BUILDDIR%
echo NOTE: The build files in %BUILDDIR% should *NEVER* be modified directly.
echo When needed, make project changes in cmake files and re-run this script.
echo   Project-specific cmake files are found in [plugin dir]\CMakeLists.txt and 
echo   [plugin dir]\Win\projectDef.cmake.
echo Note that parameters for cmake should be enclosed in double quotes, e.g. "-DVERBOSE=1"

echo CMAKE parameters: %CMAKE_PARAMS%
:end
