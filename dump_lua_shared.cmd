@cd /d %~dp0
@call settings.cmd
@cd lua51
@echo Dumping %DLLPATH% 
@echo EXPORTS > lua_shared.def
@dumpbin /EXPORTS "%DLLPATH%" | grep -A 99999 "ordinal hint" | tail -n +3 | grep -B 9999 Summary | head -n -2 | awk "{print $4}" >>  lua_shared.def
@lib /MACHINE:x86 /def:lua_shared.def /OUT:lua_shared.lib
@cp lua_shared.lib lua51.lib
@pause
