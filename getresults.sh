#!/bin/bash
OUTPUT="autoresults.txt"
FORMATED_OUTPUT="results.txt"
> $OUTPUT
for ALG in NRU LRU OPT
do
	echo $ALG | tee -a $OUTPUT
	for FILE in compilador compressor matriz simulador
	do
		echo $FILE | tee -a $OUTPUT
		for PAGE in 8 16 32
		do
			echo $PAGE | tee -a $OUTPUT
			./sim-virtual $ALG $FILE.log $PAGE 16 | grep -oP "(faults|written): \K\d*" | tee -a $OUTPUT
		done
	done
done
python3 autotable.py > $FORMATED_OUTPUT
rm $OUTPUT
