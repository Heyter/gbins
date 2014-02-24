@echo off
echo Creating solution files...
echo.
..\premake4 --os=windows --platform=x32 --file=premake.lua vs2010
IF ERRORLEVEL 0 GOTO OKAY
echo Premake failed. Cancelling.
pause
Exit /B 2

:OKAY
