#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void init_matrix(double **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = (double)rand() / RAND_MAX * 100.0;
        }
    }
}

void matrix_operations(double **a, double **b, double **add, double **sub, double **mul, double **div, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            add[i][j] = a[i][j] + b[i][j];
            sub[i][j] = a[i][j] - b[i][j];
            mul[i][j] = a[i][j] * b[i][j];
            div[i][j] = (b[i][j] != 0) ? (a[i][j] / b[i][j]) : 0;
        }
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size_proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size_proc);

    if (size_proc != 1) {
        if (rank == 0) {
            printf("Ошибка: Последовательная версия должна запускаться с 1 процессом!\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (argc < 2) {
        if (rank == 0) {
            printf("Использование: %s <размер_матрицы>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int size = atoi(argv[1]);
    srand(time(NULL) + rank);

    double start_time = get_time();

    double **a = (double **)malloc(size * sizeof(double *));
    double **b = (double **)malloc(size * sizeof(double *));
    double **add = (double **)malloc(size * sizeof(double *));
    double **sub = (double **)malloc(size * sizeof(double *));
    double **mul = (double **)malloc(size * sizeof(double *));
    double **div = (double **)malloc(size * sizeof(double *));

    for (int i = 0; i < size; i++) {
        a[i] = (double *)malloc(size * sizeof(double));
        b[i] = (double *)malloc(size * sizeof(double));
        add[i] = (double *)malloc(size * sizeof(double));
        sub[i] = (double *)malloc(size * sizeof(double));
        mul[i] = (double *)malloc(size * sizeof(double));
        div[i] = (double *)malloc(size * sizeof(double));
    }

    init_matrix(a, size);
    init_matrix(b, size);
    matrix_operations(a, b, add, sub, mul, div, size);
  
    for (int i = 0; i < size; i++) {
        free(a[i]);
        free(b[i]);
        free(add[i]);
        free(sub[i]);
        free(mul[i]);
        free(div[i]);
    }
    free(a);
    free(b);
    free(add);
    free(sub);
    free(mul);
    free(div);

    double end_time = get_time();

    if (rank == 0) {
        printf("ПРОЦЕССОВ: 1\n");
        printf("РАЗМЕР_МАТРИЦЫ: %d\n", size);
        printf("ВРЕМЯ_ВЫПОЛНЕНИЯ: %.6f сек\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
