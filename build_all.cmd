@echo Building all...
@echo off

 
for /f %%i in ('dir . /ad /b') do (
cd %%i
IF EXIST build.cmd build.cmd
cd ..
)
