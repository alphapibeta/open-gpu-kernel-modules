#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define ARRAY_SIZE 1000000

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device_file>\n", argv[0]);
        return 1;
    }

    const char *DEVICE_FILE = argv[1];
    int fd;
    int *array;
    long long result;
    struct timespec start, end;
    double cpu_time_used;
    int size = ARRAY_SIZE;
    int total_len = sizeof(int) + ARRAY_SIZE * sizeof(int);
    char *buffer;

    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return 1;
    }

    array = malloc(ARRAY_SIZE * sizeof(int));
    if (!array) {
        perror("Failed to allocate memory");
        close(fd);
        return 1;
    }

    buffer = malloc(total_len);
    if (!buffer) {
        perror("Failed to allocate memory for buffer");
        free(array);
        close(fd);
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 100;  // Random numbers between 0 and 99
    }

    memcpy(buffer, &size, sizeof(int));
    memcpy(buffer + sizeof(int), array, ARRAY_SIZE * sizeof(int));

    printf("Array sample (first 10 elements): ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    printf("Array sample (last 10 elements): ");
    for (int i = ARRAY_SIZE - 10; i < ARRAY_SIZE; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    clock_gettime(CLOCK_MONOTONIC, &start);

    if (write(fd, buffer, total_len) != total_len) {
        perror("Failed to write to the device");
        free(buffer);
        free(array);
        close(fd);
        return 1;
    }

    if (read(fd, &result, sizeof(result)) != sizeof(result)) {
        perror("Failed to read result from the device");
        free(buffer);
        free(array);
        close(fd);
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("User program execution time: %f seconds\n", cpu_time_used);
    printf("Normal sum result: %lld\n", result);

    free(buffer);
    free(array);
    close(fd);

    return 0;
}
