@echo off & setlocal enableextensions enabledelayedexpansion

set _FB_GEN="Visual Studio 9 2008 Win64"

call "%~d0%~p0\common.cmd" %*
if %errorlevel% == 2 exit /b 1
call "%~d0%~p0\winprep.cmd"
