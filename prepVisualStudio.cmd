@echo off & setlocal enableextensions enabledelayedexpansion

REM check if 1st parameter is Visual Studio cmake generator name
echo "%~1" | findstr /c:"Visual Studio" >nul 
if not %errorlevel% == 0  goto args_count_wrong 
goto args_count_ok

:args_count_wrong
REM Show usage message and list available Visual Studio cmake generators
echo Usage %0 "visual studio cmake generator" [project dir] [build dir]  
echo The available Visual Studio cmake generators are : 
cmake --help | FINDSTR "Visual Optional"
exit /b 1

:args_count_ok
REM set generator 
set _FB_GEN="%~1"
set ALL_ARG=%*
call set ALL_BUT_FIRST_ARG=%%ALL_ARG:%_FB_GEN%=%%

call "%~d0%~p0\common.cmd" %ALL_BUT_FIRST_ARG%
if %errorlevel% == 2 exit /b 1
call "%~d0%~p0\winprep.cmd"
