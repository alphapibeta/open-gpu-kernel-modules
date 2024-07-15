#include "x_square_impl.h"
#include <linux/types.h>
#include <linux/kernel.h>

// Define a vector type for SIMD
typedef float v4sf __attribute__ ((vector_size (16)));

void x_square_backward_op(float *inputs, float *results, int num_inputs) {
    int i;
    for (i = 0; i < num_inputs; i += 4) {
        v4sf x = *((v4sf*) &inputs[i]);
        v4sf x2 = x * 2.0;
        *((v4sf*) &results[i]) = x2;
    }
}
