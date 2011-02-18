@echo off

:runcmake
pushd "%BUILDDIR%" > NUL
if %errorlevel% == 1 goto builddir_error
REM ** shift off the first 2 params so the rest goes to cmake
shift
shift

set PDIR=-DFB_PROJECTS_DIR=
set PDIR=%PDIR%%PROJDIR%

IF %FB_ROOT:~-2,-1% == \ SET FB_ROOT=%FB_ROOT:~0,-2%%FB_ROOT:~-1%

@echo on
cmake -G %_FB_GEN% %PDIR% %CMAKE_PARAMS% %FB_ROOT%
@echo off

popd
goto end

:builddir_error
echo ERROR: Could not change to %BUILDDIR%
exit /b 1

:end
