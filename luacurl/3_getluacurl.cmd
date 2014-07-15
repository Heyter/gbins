@call ../settings.cmd

wget --no-check-certificate https://github.com/msva/lua-curl/archive/master.zip -O master.zip
unzip master.zip
del master.zip
move lua-curl-master lua-curl
