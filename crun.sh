HOST=localhost
PORT=8888

make clean
make
./bin/request $HOST $PORT
#for ((i=0; i<5; i++))
#do
#	Pi=$((P+i))
#	echo P=$Pi
#	echo multi-thread program is running.
#	./bin/server_shm $T $B $Pi $C $P_t $R_s $C_t1 $C_t2 $p_i
#	echo multi-process program is running.
#	./bin/server_msq $T $B $Pi $C $P_t $R_s $C_t1 $C_t2 $p_i 
#done

