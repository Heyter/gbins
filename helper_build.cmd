@call ../settings.cmd
@call ../helper_premake.cmd

@echo Building...
@echo.

@if %VSVER%==140 goto vs2015
@if %VSVER%==120 goto vs2013
@if %VSVER%==100 goto vs2010
@goto FAIL

:vs2015
@FOR %%c in (windows\vs2015\*.sln) DO msbuild /nologo %%c
@goto END

:vs2013
@FOR %%c in (windows\vs2013\*.sln) DO msbuild /nologo %%c
@goto END

:vs2010
@FOR %%c in (windows\vs2010\*.sln) DO msbuild /nologo %%c
@goto END

:FAIL
@echo FAILURE!?

:END
@pause