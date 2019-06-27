#!/bin/bash
OUTPUT="autoresults.txt"
> $OUTPUT
for ALG in NRU LRU NOVO
do
	echo $ALG | tee $OUTPUT
	for FILE in compilador compressor matriz simulador
	do
		echo $FILE | tee $OUTPUT
		for PAGE in 8 16 32
		do
			echo $PAGE | tee $OUTPUT
			./sim-virtual $ALG $FILE.log $PAGE 16 | grep -oP "(faults|written): \K\d*" | tee $OUTPUT 
		done
	done
done
