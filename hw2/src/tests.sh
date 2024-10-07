#!/usr/bin/bash

OUTPUT_FILE="$1.txt"

rm -rf $OUTPUT_FILE

for target in "matmul" "optim" "fast"
do
    make clean && make $target

    for N in 500 512 1000 1024 2000 2048
    do
        for I in {3..6}
        do
            echo $target $N $I
            ./matmul -r 7 -n $N -i $I $target >> $OUTPUT_FILE
        done
    done
done

make clean && make "debug"

for N in 500 512 1000 1024
do
    for I in {3..6}
    do
        echo "debug" $N $I
        ./matmul -r 7 -n $N -i $I "debug">> $OUTPUT_FILE
    done
done

sed 's/\t/;/g' $OUTPUT_FILE | grep ^runtime > "$1.csv"