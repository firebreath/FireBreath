@echo off

:runcmake
pushd "%BUILDDIR%" > NUL
if %errorlevel% == 1 goto builddir_error
REM ** shift off the first 2 params so the rest goes to cmake
shift
shift

IF %FB_ROOT:~-2,-1% == \ SET FB_ROOT=%FB_ROOT:~0,-2%%FB_ROOT:~-1%

set FB_ROOT_ARG=-DFB_ROOT=
set FB_ROOT_ARG=%FB_ROOT_ARG%%FB_ROOT%

set PDIR=-DFB_PROJECTS_DIR=
set PDIR=%PDIR%%PROJDIR%

@echo on
cmake -G %_FB_GEN% %FB_ROOT_ARG% %CMAKE_PARAMS% %PROJDIR%
@echo off

popd
goto end

:builddir_error
echo ERROR: Could not change to %BUILDDIR%
exit /b 1

:end
