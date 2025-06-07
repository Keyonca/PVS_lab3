#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size_proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size_proc);
    
    const char *env_size = getenv("ARRAY_SIZE");
    int array_size = env_size ? atoi(env_size) : 200000;
    
    // Распределяем работу между процессами
    int chunk_size = array_size / size_proc;
    int remainder = array_size % size_proc;
    
    int start = rank * chunk_size + (rank < remainder ? rank : remainder);
    int end = start + chunk_size + (rank < remainder ? 1 : 0);
    
    // Каждый процесс вычисляет свою часть суммы
    long long local_sum = 0;
    for (int i = start; i < end; i++) {
        local_sum += (i + 1);  // Эквивалентно array[i] = i + 1
    }
    
    double start_time = MPI_Wtime();
    
    // Собираем результаты на процессе 0
    long long global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    double end_time = MPI_Wtime();
    
    if (rank == 0) {
        printf("Сумма массива: %lld\n", global_sum);
        printf("Время выполнения параллельной программы: %f секунд с %d процессами\n",
               end_time - start_time, size_proc);
    }
    
    MPI_Finalize();
    return 0;
}
