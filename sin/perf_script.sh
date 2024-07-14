#!/bin/bash

PERF_DATA_FILE="perf_output/perf.data"
PERF_OUTPUT_DIR="perf_output"
PERF_SCRIPT_OUTPUT="${PERF_OUTPUT_DIR}/perf_script_output.txt"
FILTERED_OUTPUT="${PERF_OUTPUT_DIR}/filtered_perf_output.txt"
SUMMARY_OUTPUT="${PERF_OUTPUT_DIR}/summary.txt"

# Ensure the output directory exists
mkdir -p "$PERF_OUTPUT_DIR"

# Extract data using perf script
sudo perf script -i "$PERF_DATA_FILE" > "$PERF_SCRIPT_OUTPUT"

# Filter relevant information (cycles, memory loads, memory stores)
grep -E "cycles|mem-loads|mem-stores|time" "$PERF_SCRIPT_OUTPUT" > "$FILTERED_OUTPUT"

# Create a summary with function times, memory loads, and stores
awk '
BEGIN { FS=" "; OFS="\t"; print "Function", "Time (ns)", "Cycles", "Memory Loads", "Memory Stores" }
/cycles/ { cycles = $1 }
/mem-loads/ { mem_loads = $1 }
/mem-stores/ { mem_stores = $1 }
/time/ { time = $1; func = $2; print func, time, cycles, mem_loads, mem_stores; cycles=""; mem_loads=""; mem_stores="" }
' "$FILTERED_OUTPUT" > "$SUMMARY_OUTPUT"

echo "Performance summary saved to $SUMMARY_OUTPUT"
