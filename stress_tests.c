#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t running = 1;

typedef struct {
    long cpu_iterations;
    long disk_bytes_written;
    double duration;
} TestMetrics;

TestMetrics metrics = {0, 0, 0.0};
pthread_mutex_t metrics_lock = PTHREAD_MUTEX_INITIALIZER;

void signal_handler(int sig) {
    running = 0;
}

double get_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

void* cpu_stress(void* arg) {
    long local_iterations = 0;
    while (running) {
        for (int i = 2; i < 100000; i++) {
            int is_prime = 1;
            for (int j = 2; j * j <= i; j++) {
                if (i % j == 0) {
                    is_prime = 0;
                    break;
                }
            }
            local_iterations++;
        }
    }
    pthread_mutex_lock(&metrics_lock);
    metrics.cpu_iterations += local_iterations;
    pthread_mutex_unlock(&metrics_lock);
    return NULL;
}

void* disk_stress(void* arg) {
    long local_written = 0;
    int fd = open("stress_test_file", O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        perror("File open failed");
        return NULL;
    }

    char buffer[4096];
    memset(buffer, 'A', sizeof(buffer));

    while (running) {
        ssize_t written = write(fd, buffer, sizeof(buffer));
        if (written < 0) {
            perror("Write failed");
            break;
        }
        local_written += written;
    }

    close(fd);
    unlink("stress_test_file");

    pthread_mutex_lock(&metrics_lock);
    metrics.disk_bytes_written += local_written;
    pthread_mutex_unlock(&metrics_lock);

    return NULL;
}

void print_report() {
    printf("\n=== Stress Test Report ===\n");
    printf("Test Duration: %.2f seconds\n", metrics.duration);
    printf("CPU Stress: %ld iterations\n", metrics.cpu_iterations);
    printf("Disk Stress: %ld bytes written\n", metrics.disk_bytes_written);
    printf("===========================\n");
}

int main(int argc, char* argv[]) {
    int num_threads = 1;
    int duration = 10;
    int opt;

    while ((opt = getopt(argc, argv, "t:d:")) != -1) {
        switch (opt) {
            case 't': num_threads = atoi(optarg); break;
            case 'd': duration = atoi(optarg); break;
            default:
                fprintf(stderr, "Usage: %s [-t threads] [-d duration]\n", argv[0]);
                exit(1);
        }
    }

    signal(SIGINT, signal_handler);

    pthread_t* cpu_threads = malloc(num_threads * sizeof(pthread_t));
    pthread_t disk_thread;

    if (!cpu_threads) {
        perror("Allocation failed");
        return 1;
    }

    double start_time = get_time_sec();

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&cpu_threads[i], NULL, cpu_stress, NULL)) {
            perror("CPU thread creation failed");
            return 1;
        }
    }

    if (pthread_create(&disk_thread, NULL, disk_stress, NULL)) {
        perror("Disk thread creation failed");
        return 1;
    }

    sleep(duration);
    running = 0;

    for (int i = 0; i < num_threads; i++) {
        pthread_join(cpu_threads[i], NULL);
    }
    pthread_join(disk_thread, NULL);

    metrics.duration = get_time_sec() - start_time;
    print_report();

    free(cpu_threads);
    return 0;
}
