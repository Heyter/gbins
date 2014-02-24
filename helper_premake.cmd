@echo off
echo Creating solution files...
echo.
..\premake5 --os=windows --file=premake.lua vs2010
IF ERRORLEVEL 0 GOTO OKAY
echo Premake failed. Cancelling.
pause
Exit /B 2

:OKAY
