@echo Creating solution files...
@echo.
..\premake5 --os=windows --file=premake.lua vs2015
@IF NOT ERRORLEVEL 0 GOTO FAIL
..\premake5 --os=windows --file=premake.lua vs2013
@IF NOT ERRORLEVEL 0 GOTO FAIL
..\premake5 --os=windows --file=premake.lua vs2010
@IF NOT ERRORLEVEL 0 GOTO FAIL
@GOTO OKAY
:FAIL
@echo Premake failed. Cancelling.
@pause
@Exit /B 2

:OKAY
