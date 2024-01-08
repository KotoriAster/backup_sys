mkdir build;
cd build;
cmake ..;
cmake --build .;
mkdir testcases; cd testcases;
mkdir 1 t1 t2 1/2;
touch 1/simpletext;
ln 1/simpletext 1/2/simpletext.1.hlk;
ln 1/simpletext 1/2/simpletext.2.hlk;
ln -s 1/simpletext 1/2/simpletext.1.slk;
cd ..;
./tests/utest