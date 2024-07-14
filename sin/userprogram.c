#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define NUM_ANGLES 13

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device_file>\n", argv[0]);
        return 1;
    }

    const char *DEVICE_FILE = argv[1];
    int fd;
    float angles[NUM_ANGLES] = {0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3};
    float results[NUM_ANGLES * 3];
    struct timespec start, end;
    double cpu_time_used;

    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    if (write(fd, angles, sizeof(angles)) != sizeof(angles)) {
        perror("Failed to write to the device");
        close(fd);
        return 1;
    }

    if (read(fd, results, sizeof(results)) != sizeof(results)) {
        perror("Failed to read results from the device");
        close(fd);
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    close(fd);

    cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution time: %f seconds\n", cpu_time_used);
    printf("Angle | Sine (calculated) | Sine (actual) | Cosine (calculated) | Cosine (actual)\n");
    printf("------|-------------------|---------------|---------------------|------------------\n");
    for (int i = 0; i < NUM_ANGLES; i++) {
        float angle = results[i];
        float sin_result = results[i + NUM_ANGLES];
        float cos_result = results[i + 2 * NUM_ANGLES];
        float actual_sine = sinf(angle * M_PI);
        float actual_cosine = cosf(angle * M_PI);
        printf("%-5.2f | %-17f | %-13f | %-19f | %-18f\n", 
               angle, sin_result, actual_sine, cos_result, actual_cosine);
    }

    return 0;
}
