@echo off
echo Creating solution files...
echo.
..\premake4 --os=windows --platform=x32 --file=premake.lua vs2010
