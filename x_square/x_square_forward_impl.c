#include "x_square_impl.h"
#include <linux/types.h>
#include <linux/kernel.h>

void x_square_forward(float *inputs, float *results, int num_inputs) {
    int i;
    for (i = 0; i < num_inputs; i++) {
        results[i] = inputs[i] * inputs[i];
    }
}
