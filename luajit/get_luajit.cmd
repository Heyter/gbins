wget http://luajit.org/download/LuaJIT-2.0.3.tar.gz
gzip -d LuaJIT-2.0.3.tar.gz
tar -xvf LuaJIT-2.0.3.tar
cp -r LuaJIT-2.0.3/* .
rm -rf LuaJIT-2.0.3.tar LuaJIT-2.0.3
echo Done
pause