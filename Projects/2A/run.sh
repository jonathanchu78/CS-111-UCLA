#! /bin/bash

let threadnum=5
let iterationnum=8
declare -a YIELD
declare -a SYNCS
declare -a THREADS
declare -a ITERATIONS
YIELD=( '' '--yield' )
SYNCS=( '' '--sync=m' '--sync=s' '--sync=c' )
THREADS=( 1 2 4 8 12 )
ITERATIONS=( 10 20 40 80 100 1000 10000 100000 )

#without sync
rm lab2_add.csv
for (( k=0; k<2; k++ ))
	do
	for (( i=0; i<$threadnum; i++ ))
	do
		for (( j=0; j<$iterationnum; j++ ))
		do
			
			for (( l=0; l<4; l++ ))
			do
				./copy "${SYNCS[$l]}" "${YIELD[$k]}" --threads "${THREADS[$i]}" --iterations "${ITERATIONS[$j]}" >> lab2_add.csv
				
			done
			echo "${ITERATIONS[$j]}"
		done
	done
done


