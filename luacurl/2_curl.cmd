@call ../settings.cmd

wget http://curl.haxx.se/download/curl-7.37.0.zip
unzip curl-7.37.0.zip
del curl-7.37.0.zip
move curl-7.37.0 curl-src
cd curl-src\winbuild
nmake -f Makefile.vc mode=dll VC=12 WITH_ZLIB=dll ENABLE_WINSSL=yes ENABLE_IDN=yes ENABLE_IPV6=yes ENABLE_SSPI=yes ENABLE_SPNEGO=yes DEBUG=no MACHINE=x86
cd ..\..
pause

