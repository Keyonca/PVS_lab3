#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

void fill_matrix(double *matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i * size + j] = (double)rand() / RAND_MAX * 100.0;
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size_mpi;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size_mpi);

    int size = 1000;
    if (getenv("ARRAY_SIZE")) {
        size = atoi(getenv("ARRAY_SIZE"));
    }

    srand(time(NULL) + rank);

    double *A = NULL;
    double *B = NULL;
    double *C = NULL;

    if (rank == 0) {
        A = (double *)malloc(size * size * sizeof(double));
        B = (double *)malloc(size * size * sizeof(double));
        C = (double *)malloc(size * size * sizeof(double));
        fill_matrix(A, size);
        fill_matrix(B, size);
    }

    int local_size = size / size_mpi;
    double *local_A = (double *)malloc(local_size * size * sizeof(double));
    double *local_B = (double *)malloc(local_size * size * sizeof(double));
    double *local_C = (double *)malloc(local_size * size * sizeof(double));

    MPI_Scatter(A, local_size * size, MPI_DOUBLE, local_A, local_size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, local_size * size, MPI_DOUBLE, local_B, local_size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Операции над матрицами
    for (int i = 0; i < local_size; i++) {
        for (int j = 0; j < size; j++) {
            int idx = i * size + j;
            local_C[idx] = local_A[idx] + local_B[idx];  // Сложение
            local_C[idx] = local_A[idx] - local_B[idx];  // Вычитание
            local_C[idx] = local_A[idx] * local_B[idx];  // Умножение
            if (local_B[idx] != 0) {
                local_C[idx] = local_A[idx] / local_B[idx];  // Деление
            }
        }
    }

    MPI_Gather(local_C, local_size * size, MPI_DOUBLE, C, local_size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    if (rank == 0) {
        printf("ВРЕМЯ_ВЫПОЛНЕНИЯ: %.6f\n", time_taken);
        printf("ПРОЦЕССОВ: %d\n", size_mpi);
        free(A);
        free(B);
        free(C);
    }

    free(local_A);
    free(local_B);
    free(local_C);

    MPI_Finalize();
    return 0;
}
