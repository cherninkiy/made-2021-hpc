#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define NUM_THREADS 4
#define CHUNK_SIZE 100000
#define POINTS_TOTAL 1000000

const char* sched_names[5] = { "", "static", "dynamic", "guided", "auto" };

int main(int argc, char *argv[])
{
    long points_total = POINTS_TOTAL;
    
    // Try get num_points from cmd parameters
    if (argc > 1) {
        points_total = atoi(argv[1]);
    }

    // Try get num_threads from OMP_NUM_THREADS
    int num_threads = omp_get_num_threads();

    // Try get num_threads from cmd parameters
    if (argc > 2) {
        num_threads = atoi(argv[2]);
    }
    if (num_threads < 2) {
        num_threads = NUM_THREADS;
    }

    printf("Requested number of threads = %d\n", num_threads);
    omp_set_num_threads(num_threads);

    int chunk_size;
    omp_sched_t schedule;
    omp_get_schedule(&schedule, &chunk_size);

    unsigned int idx;
    idx = (unsigned int)schedule < (unsigned int)omp_sched_auto ? (unsigned int)schedule : 0;

    // Try get chunk_size from cmd parameters
    if (argc > 3) {
        chunk_size = atoi(argv[3]);
    }
    else
    {
        chunk_size = CHUNK_SIZE;
    }
    omp_set_schedule(schedule, chunk_size);
    printf("Runtime schedule = %s\n", sched_names[idx]);
    printf("Schedule chunk size = %d\n", chunk_size);

    printf("Points total = %ld\n", points_total);

    double x, y, pi, timestamp;
    long points_in_circle = 0;

    num_threads = 0;
    timestamp = omp_get_wtime();

    #pragma omp parallel private (x, y) \
            shared(points_total) \
            reduction(+:points_in_circle) \
            reduction(+:num_threads)
    {
        srand(time(NULL) ^ omp_get_thread_num());

        #pragma omp for schedule(runtime)
        for (int i = 0; i < points_total; i += 1) 
        {
                x = (double) rand() / RAND_MAX;
                y = (double) rand() / RAND_MAX;    
            if (x * x + y * y <= 1) 
            {
                points_in_circle += 1;
            }
        }
        
        num_threads += 1;
    }

    timestamp = omp_get_wtime() - timestamp;

    pi = (4.0 * points_in_circle) / points_total;

    printf("Actual number of threads = %d\n", num_threads);
    printf("Points in circle = %ld\n", points_in_circle);
    printf("Calculated Pi = %.12f\n", pi);
    printf("Execution time = %.12f\n", timestamp);

    return 0;
}