@echo Building all...
@echo off
@cd /d %~dp0

 
for /f %%i in ('dir . /ad /b') do (
cd %%i
IF EXIST build.cmd build.cmd
cd ..
)
