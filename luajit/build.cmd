@call ../settings.cmd
@cd src
setenv /release /x86
call msvcbuild.bat static
@pause