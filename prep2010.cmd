@echo off

IF /I "%1"=="examples" GOTO examples

:normal
echo Building projects
cmake -D GEN="Visual Studio 10" -P cmake\genproject.cmake
goto end

:examples
echo Building example projects
cmake -D GEN="Visual Studio 10" -D BUILD_EXAMPLES="YES" -P cmake\genproject.cmake

:end
