cd source-sdk-2013-master/mp/src

./createallprojects

cd tier1
make -f tier1_linux32.mak
cd ..

cd mathlib
make -f mathlib_linux32.mak
cd  ..

