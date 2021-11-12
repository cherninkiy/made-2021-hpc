#!/usr/bin/bash

for rule in 110 30 90 184
do
	for n in {1..4}
	do
		echo "Number of processors $n - rule $rule cyclic"
		mpirun -n $n ./rule110.py $rule 200 200 --cyclic

		echo "Number of processors $n - rule $rule bounded"
		mpirun -n $n ./rule110.py $rule 200 200
	done
done

