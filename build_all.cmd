@echo Building all...
@cd /d %~dp0
@set AUTOBUILDING=1
 
@for /f %%i in ('dir . /ad /b') do @(
	@cd %%i
	@IF EXIST build.cmd @(
		@echo ====== COMPILING %%i ========
		build.cmd
	)
	@cd ..
)
