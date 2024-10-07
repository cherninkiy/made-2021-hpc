#!/usr/bin/bash


for i in {2..5}
do
	echo "psize = $i"
	for j in {1..3}
	do
		mpirun -n $i ./meet
		echo ""
	done
	echo ""
done
