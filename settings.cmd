@echo off

set PATH=C:\windows\Microsoft.NET\Framework\v4.0.30319;%PATH%

msbuild /? > nul
IF ERRORLEVEL 0 GOTO OKAY
echo Compiling requires msbuild! Compile manually...
pause
Exit /B 1

:OKAY
