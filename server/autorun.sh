HOST=localhost
PORT=8888
P=5
C=5

make clean
make
./bin/mapserver $PORT $P $C &

