@call ../settings.cmd
setenv /release /x86
cd src
@call msvcbuild.bat static
pause