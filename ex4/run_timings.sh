#!/usr/bin/env bash
# Exercise 4 - time Exercise 2 (sum) and Exercise 3 (pi) on 1, 2, 4, 8 processes.
# Each case runs 5 times and the MEDIAN time is saved (one run alone is too noisy).
# Usage (from the Lab06 folder):   ./ex4/run_timings.sh
#        multi-node:               HOSTS="--hostfile hosts" ./ex4/run_timings.sh
OUT=ex4/results.csv
PROCS="1 2 4 8"
REPS=5
echo "program,processes,time" > $OUT
for prog in ex2/sum ex3/pi; do
  for np in $PROCS; do
    times=$(for r in $(seq $REPS); do
              mpirun --oversubscribe $HOSTS -np $np ./$prog | grep "Time" | awk '{print $(NF-1)}'
            done | sort -g)
    median=$(echo "$times" | sed -n "$(( (REPS + 1) / 2 ))p")
    echo "$(basename $prog),$np,$median" | tee -a $OUT
  done
done
echo "Saved $OUT  ->  now run:  python3 ex4/plot_graphs.py"
