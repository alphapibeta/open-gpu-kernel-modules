#!/bin/bash

set -e

MODULE_NAME="x_square"
DEVICE_NAME="x_square_dev"
PERF_OUTPUT_DIR="perf_output"

unload_module() {
    if lsmod | grep -q "$MODULE_NAME"; then
        sudo rmmod "$MODULE_NAME" || echo "Failed to unload $MODULE_NAME"
    else
        echo "$MODULE_NAME is not loaded"
    fi
}

load_module() {
    if ! lsmod | grep -q "$MODULE_NAME"; then
        sudo insmod "${MODULE_NAME}.ko" || (echo "Failed to load $MODULE_NAME" && exit 1)
        echo "$MODULE_NAME loaded successfully"
        sleep 1
        if [[ ! -e "/dev/$DEVICE_NAME" ]]; then
            echo "Device file /dev/$DEVICE_NAME not created. Check dmesg for errors."
            exit 1
        fi
    else
        echo "$MODULE_NAME is already loaded"
    fi
}

run_test() {
    echo "Running test for $MODULE_NAME using device /dev/$DEVICE_NAME"
    sudo ./userprogram "/dev/$DEVICE_NAME" > "${PERF_OUTPUT_DIR}/performance.txt"
}

run_test_op() {
    echo "Running test for $MODULE_NAME with optimizations using device /dev/$DEVICE_NAME"
    sudo ./userprogram_op "/dev/$DEVICE_NAME" > "${PERF_OUTPUT_DIR}/performance_op.txt"
}

analyze_perf() {
    echo "Analyzing performance..."
    sudo perf record -o ${PERF_OUTPUT_DIR}/perf.data -g ./userprogram "/dev/$DEVICE_NAME"
    sudo perf report -i ${PERF_OUTPUT_DIR}/perf.data > ${PERF_OUTPUT_DIR}/perf_report.txt
    sudo perf script -i ${PERF_OUTPUT_DIR}/perf.data > ${PERF_OUTPUT_DIR}/perf_script_output.txt

    echo "Analyzing performance with optimizations..."
    sudo perf record -o ${PERF_OUTPUT_DIR}/perf.data.op -g ./userprogram_op "/dev/$DEVICE_NAME"
    sudo perf report -i ${PERF_OUTPUT_DIR}/perf.data.op > ${PERF_OUTPUT_DIR}/perf_report_op.txt
    sudo perf script -i ${PERF_OUTPUT_DIR}/perf.data.op > ${PERF_OUTPUT_DIR}/perf_script_output_op.txt
}

mkdir -p "$PERF_OUTPUT_DIR"
unload_module

echo "Cleaning and building..."
make clean
make

echo "Testing x_square version without optimizations..."
load_module
run_test
analyze_perf

echo "Testing x_square version with optimizations..."
run_test_op
analyze_perf

unload_module

echo "Cleaning up..."
make clean

echo "Performance results stored in ${PERF_OUTPUT_DIR}/performance.txt and ${PERF_OUTPUT_DIR}/performance_op.txt"
