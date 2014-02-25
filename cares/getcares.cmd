wget http://c-ares.haxx.se/download/c-ares-1.10.0.tar.gz
gzip -d c-ares-1.10.0.tar.gz
tar -xvf c-ares-1.10.0.tar
mkdir cares || rm -rf cares/*
mv c-ares-1.10.0/* cares/
rmdir c-ares-1.10.0
rm -f c-ares-1.10.0.tar
cd cares
start .
