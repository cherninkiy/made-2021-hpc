#!/usr/bin/bash

ARRAY=()
for i in {1..15};
do 
	ARRAY=("${ARRAY[@]}" "$i")
done

for value in "${ARRAY[@]}"
do
     echo $value
done
