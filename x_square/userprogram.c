#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define NUM_INPUTS 13

void run_test(const char *device_file) {
    int fd;
    float inputs[NUM_INPUTS] = {0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3};
    float results[NUM_INPUTS * 4];
    struct timespec start, end;
    double cpu_time_used;

    fd = open(device_file, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        exit(1);
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    if (write(fd, inputs, sizeof(inputs)) != sizeof(inputs)) {
        perror("Failed to write to the device");
        close(fd);
        exit(1);
    }

    if (read(fd, results, sizeof(results)) != sizeof(results)) {
        perror("Failed to read results from the device");
        close(fd);
        exit(1);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    close(fd);

    cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution time: %f seconds\n", cpu_time_used);
    printf("Input | x^2 (forward) | 2x (backward) | x^2 (forward_op) | 2x (backward_op) | Forward Error | Backward Error\n");
    printf("------|----------------|---------------|------------------|------------------|---------------|----------------\n");
    for (int i = 0; i < NUM_INPUTS; i++) {
        float input = inputs[i];
        float forward_result = results[i];
        float backward_result = results[i + NUM_INPUTS];
        float forward_result_op = results[i + 2 * NUM_INPUTS];
        float backward_result_op = results[i + 3 * NUM_INPUTS];
        float true_result = input * input;
        float forward_error = fabs(forward_result - true_result);
        float backward_error = (input == 0) ? 0.0 : fabs(forward_result - true_result) / fabs(2 * input);
        float forward_error_op = fabs(forward_result_op - true_result);
        float backward_error_op = (input == 0) ? 0.0 : fabs(forward_result_op - true_result) / fabs(2 * input);

        printf("%-5.2f | %-14f | %-13f | %-16f | %-16f | %-13f | %-14f\n", 
               input, forward_result, backward_result, forward_result_op, backward_result_op, forward_error, backward_error);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device_file>\n", argv[0]);
        return 1;
    }

    const char *device_file = argv[1];

    run_test(device_file);

    return 0;
}
