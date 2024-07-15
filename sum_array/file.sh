#!/bin/bash

set -e

NORMAL_MODULE_NAME="sum_array"
CASCADING_MODULE_NAME="cascading_sum"
NORMAL_DEVICE_NAME="sum_array_dev"
CASCADING_DEVICE_NAME="cascading_sum_dev"
PERF_OUTPUT_DIR="perf_output"

unload_modules() {
    for module in "$NORMAL_MODULE_NAME" "$CASCADING_MODULE_NAME"; do
        if lsmod | grep -q "$module"; then
            sudo rmmod "$module" || echo "Failed to unload $module"
        else
            echo "$module is not loaded"
        fi
    done
}

load_modules() {
    for module in "$NORMAL_MODULE_NAME" "$CASCADING_MODULE_NAME"; do
        if ! lsmod | grep -q "$module"; then
            sudo insmod "${module}.ko" || (echo "Failed to load $module" && exit 1)
            echo "$module loaded successfully"
            sleep 1
        else
            echo "$module is already loaded"
        fi
    done

    # Check if device files are created
    for device in "$NORMAL_DEVICE_NAME" "$CASCADING_DEVICE_NAME"; do
        if [[ ! -e "/dev/$device" ]]; then
            echo "Device file /dev/$device not created. Check dmesg for errors."
            exit 1
        fi
    done
}

compile_and_build() {
    echo "Cleaning and building..."
    make clean
    make
}

setup_permissions() {
    for device in "$NORMAL_DEVICE_NAME" "$CASCADING_DEVICE_NAME"; do
        sudo chmod 666 "/dev/$device"
    done
}

mkdir -p "$PERF_OUTPUT_DIR"

unload_modules
compile_and_build
load_modules
setup_permissions

echo "Modules and user program are compiled and loaded. Ready for profiling."
echo "To profile normal sum: ./userprogram /dev/$NORMAL_DEVICE_NAME"
echo "To profile cascading sum: ./userprogram /dev/$CASCADING_DEVICE_NAME"