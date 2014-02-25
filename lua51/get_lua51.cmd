wget http://www.lua.org/ftp/lua-5.1.5.tar.gz
gzip -d lua-5.1.5.tar.gz
tar -xvf lua-5.1.5.tar
cp -r lua-5.1.5/src .
rm -rf lua-5.1.5.tar lua-5.1.5
echo Done
