#!/bin/bash

NORMAL_SUM_DIR="perf_output/normal_sum"
CASCADING_SUM_DIR="perf_output/cascading_sum"

# Create directories if they do not exist
mkdir -p "$NORMAL_SUM_DIR"
mkdir -p "$CASCADING_SUM_DIR"

# Profile normal sum
echo "Profiling normal sum..."
sudo perf record -g -o "$NORMAL_SUM_DIR/perf.data" -- ./normal_sum_userprogram /dev/sum_array_dev
sudo perf report -i "$NORMAL_SUM_DIR/perf.data" > "$NORMAL_SUM_DIR/perf_report.txt"
sudo perf script -i "$NORMAL_SUM_DIR/perf.data" > "$NORMAL_SUM_DIR/perf_script.txt"

# Generate flame graph for normal sum
sudo perf script -i "$NORMAL_SUM_DIR/perf.data" | ~/FlameGraph/stackcollapse-perf.pl > "$NORMAL_SUM_DIR/out.folded"
~/FlameGraph/flamegraph.pl "$NORMAL_SUM_DIR/out.folded" > "$NORMAL_SUM_DIR/flamegraph.svg"

# Profile cascading sum
echo "Profiling cascading sum..."
sudo perf record -g -o "$CASCADING_SUM_DIR/perf.data" -- ./cascading_sum_userprogram /dev/cascading_sum_dev
sudo perf report -i "$CASCADING_SUM_DIR/perf.data" > "$CASCADING_SUM_DIR/perf_report.txt"
sudo perf script -i "$CASCADING_SUM_DIR/perf.data" > "$CASCADING_SUM_DIR/perf_script.txt"

# Generate flame graph for cascading sum
sudo perf script -i "$CASCADING_SUM_DIR/perf.data" | ~/FlameGraph/stackcollapse-perf.pl > "$CASCADING_SUM_DIR/out.folded"
~/FlameGraph/flamegraph.pl "$CASCADING_SUM_DIR/out.folded" > "$CASCADING_SUM_DIR/flamegraph.svg"

echo "Profiling complete. Results are stored in $NORMAL_SUM_DIR and $CASCADING_SUM_DIR"
