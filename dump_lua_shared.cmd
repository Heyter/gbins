@cd /d %~dp0
@cd lua51
@set DLLPATH=x:\g\steam\steamapps\common\GarrysMod\garrysmod\bin\lua_shared.dll
@echo Dumping %DLLPATH% 
@echo EXPORTS > lua_shared.def
@dumpbin /EXPORTS "%DLLPATH%" | grep -A 99999 "ordinal hint" | tail +3 | grep -B 9999 Summary | head -n -2 | awk "{print $4}" >>  lua_shared.def
@lib /MACHINE:x86 /def:lua_shared.def /OUT:lua_shared.lib
@pause