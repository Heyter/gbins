wget http://luajit.org/download/LuaJIT-2.0.4.tar.gz
gzip -d LuaJIT-2.0.4.tar.gz
tar -xvf LuaJIT-2.0.4.tar
cp -r LuaJIT-2.0.4/* .
rm -rf LuaJIT-2.0.4.tar LuaJIT-2.0.4
echo Done
pause