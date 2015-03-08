@cd /d %~dp0
@call ../settings.cmd
@cd /d %~dp0

cd cares
nmake -f Makefile.msvc
pause
