@cd /d %~dp0
@call ..\settings.cmd
@cd /d %~dp0
@cd geoip/libGeoIP
cp GeoIP_deprecated.c GeoIP_depreciated.c
nmake /f Makefile.vc
pause
