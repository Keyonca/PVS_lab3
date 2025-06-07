#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == 0) {
        const char *env_size = getenv("ARRAY_SIZE");
        int size = env_size ? atoi(env_size) : 200000;

        int *array = malloc(size * sizeof(int));
        if (array == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        for (int i = 0; i < size; i++) {
            array[i] = i + 1;
        }

        clock_t start = clock();

        long long sum = 0;
        for (int i = 0; i < size; i++) {
            sum += array[i];
        }

        clock_t end = clock();
        double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

        printf("Сумма массива: %lld\n", sum);
        printf("Время выполнения последовательной программы: %f секунд\n", time_taken);

        free(array);
    }
    
    MPI_Finalize();
    return 0;
}
