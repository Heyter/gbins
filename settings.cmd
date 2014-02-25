@rem We need msbuild and nmake! Make sure they are found on PATH

@set PATH=C:\windows\Microsoft.NET\Framework\v4.0.30319;%PATH%

@rem Testing...

@msbuild /? > nul
IF ERRORLEVEL 0 GOTO OKAY
@echo Compiling requires msbuild! Compile manually...
@pause
@Exit /B 1

:OKAY
