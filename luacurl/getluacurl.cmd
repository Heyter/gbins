@call ../settings.cmd

wget https://github.com/msva/lua-curl/archive/master.zip
unzip master.zip
del master.zip
move lua-curl-master lua-curl
