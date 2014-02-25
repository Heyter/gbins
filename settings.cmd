@FOR /F "tokens=2*" %%A IN ('REG.EXE QUERY "HKEY_CURRENT_USER\Software\Valve\Steam" /V "SteamPath"') DO @(
	@set STEAMPATH=%%B
)

@echo Found steam at "%STEAMPATH%"
@set DLLPATH=%STEAMPATH%\steamapps\common\GarrysMod\garrysmod\bin\lua_shared.dll

@rem --------------------------------
@rem We need VS2010 msbuild and nmake!

@set PATH=%PATH%;%VS100COMNTOOLS%
@call %VS100COMNTOOLS%\vsvars32.bat

@msbuild /? > nul
@IF ERRORLEVEL 0 GOTO OKAY
@echo Compiling requires msbuild! Compile manually or fix your paths in settings.cmd...
@pause
@Exit /B 1


:OKAY
