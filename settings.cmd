@FOR /F "tokens=2*" %%A IN ('REG.EXE QUERY "HKEY_CURRENT_USER\Software\Valve\Steam" /V "SteamPath"') DO @(
	@set STEAMPATH=%%B
)

@echo Found steam at "%STEAMPATH%"
@set DLLPATH="%STEAMPATH%\steamapps\common\GarrysMod\garrysmod\bin\lua_shared.dll"


@IF defined VS100COMNTOOLS @(
	@set VSTOOLS="%VS100COMNTOOLS%"
	@set VSVER=100
	@set VSVERR=10
)
@IF defined VS120COMNTOOLS @(
	@set VSTOOLS="%VS120COMNTOOLS%"
	@set VSVER=120
	@set VSVERR=12
)

:cont
@call %VSTOOLS%\vsvars32.bat

@msbuild /? > nul
@IF ERRORLEVEL 0 GOTO OKAY
@echo Compiling requires msbuild! Compile manually or fix your paths in settings.cmd...
@pause
@Exit /B 1


:OKAY
