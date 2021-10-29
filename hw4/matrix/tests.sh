#!/usr/bin/bash

OUTPUT_FILE="result.txt"
rm -rf $OUTPUT_FILE

rm -rf ./bin/* ./build/*

mkdir ./build
cd ./build

cmake .. 
cmake --build .
cd ../

for N in 500 512 1000 1024
do
    for P in {2..5}
    do
        for I in {3..6}
        do
            export OMP_NUM_THREADS=1
            export GOTO_NUM_THREADS=1
            export OPENBLAS_NUM_THREADS=1

            echo -t 1 -r 7 -n $N -p $P -i $I
            ./bin/matpower -t 1 -r 7 -n $N -p $P -i $I >> $OUTPUT_FILE

            export OMP_NUM_THREADS=4
            export GOTO_NUM_THREADS=4
            export OPENBLAS_NUM_THREADS=4

            echo -t 4 -r 7 -n $N -p $P -i $I
            ./bin/matpower -t 4 -r 7 -n $N -p $P -i $I >> $OUTPUT_FILE

            export OMP_NUM_THREADS=8
            export GOTO_NUM_THREADS=8
            export OPENBLAS_NUM_THREADS=8
            
            echo -t 8 -r 7 -n $N -p $P -i $I
            ./bin/matpower -t 8 -r 7 -n $N -p $P -i $I >> $OUTPUT_FILE
        done
    done
done


sed 's/\t/;/g' $OUTPUT_FILE | grep ^runtime > "result.csv"