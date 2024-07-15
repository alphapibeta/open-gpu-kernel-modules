#include <linux/slab.h>
#include "sum_array_impl.h"

long long normal_sum(int *array, int size) {
    long long sum = 0;
    int i;
    for (i = 0; i < size; i++) {
        sum += array[i];
    }
    return sum;
}

long long cascading_sum(int *array, int size) {
    int new_size, i;
    int *temp;
    long long result;

    if (size == 1) return array[0];
    if (size == 2) return array[0] + array[1];

    new_size = (size + 1) / 2;
    temp = kmalloc(new_size * sizeof(int), GFP_KERNEL);
    if (!temp) {
        return -ENOMEM;
    }

    for (i = 0; i < size / 2; i++) {
        temp[i] = array[2*i] + array[2*i + 1];
    }

    if (size % 2 != 0) {
        temp[new_size - 1] = array[size - 1];
    }

    result = cascading_sum(temp, new_size);
    kfree(temp);
    return result;
}