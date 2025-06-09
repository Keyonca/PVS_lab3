#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 200000;
    char *env_n = getenv("ARRAY_SIZE");
    if (env_n != NULL) N = atoi(env_n);
    if (argc > 1) N = atoi(argv[1]);

    if (N < 100000) {
        if (rank == 0) fprintf(stderr, "N must be >= 100000\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int remainder = N % size;
    int local_n = N / size + (rank < remainder ? 1 : 0);
    int start_index = 0;
    
    int prev_block;
    for (int i = 0; i < rank; i++) {
        prev_block = N / size + (i < remainder ? 1 : 0);
        start_index += prev_block;
    }

    double *a_local = malloc(local_n * sizeof(double));
    double *b_local = malloc(local_n * sizeof(double));
    double *sum_local = malloc(local_n * sizeof(double));
    double *diff_local = malloc(local_n * sizeof(double));
    double *prod_local = malloc(local_n * sizeof(double));
    double *div_local = malloc(local_n * sizeof(double));

    if (!a_local || !b_local || !sum_local || !diff_local || !prod_local || !div_local) {
        fprintf(stderr, "Process %d: Memory allocation failed\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < local_n; i++) {
        int global_index = start_index + i;
        a_local[i] = global_index + 1.0;
        b_local[i] = (global_index + 1.0) * 2.0;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    for (int i = 0; i < local_n; i++) {
        sum_local[i] = a_local[i] + b_local[i];
        diff_local[i] = a_local[i] - b_local[i];
        prod_local[i] = a_local[i] * b_local[i];
        div_local[i] = a_local[i] / b_local[i];
    }

    double local_time = MPI_Wtime() - start_time;
    double max_time;
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("ВРЕМЯ_ВЫПОЛНЕНИЯ: %f\n", max_time);
        printf("ПРОЦЕССОВ: %d\n", size);
    }

    free(a_local); free(b_local); free(sum_local); 
    free(diff_local); free(prod_local); free(div_local);
    MPI_Finalize();
    return 0;
}
