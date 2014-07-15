@call ../settings.cmd

wget http://zlib.net/zlib128.zip
unzip zlib128.zip
del zlib128.zip
move zlib-1.2.8 zlib
cd zlib
set Include=%cd%;%cd%\win32;%Include%
nmake -f win32/Makefile.msc
cd ..
copy zlib\zlib.lib deps\lib\zlib.lib
copy zlib\*.h deps\include\
copy zlib\zlib1.dll deps\bin\zlib1.dll
