#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Использование: $0 <количество_процессов> <размер_массива>"
    exit 1
fi

RANKS=$1
ARRAY_SIZE=$2
export ARRAY_SIZE

echo "=== Запуск тестов (100 итераций) ==="

run_test() {
    local prog=$1
    local ranks=$2
    local desc=$3
    
    echo "--- $desc ---"
    temp_file=$(mktemp)
    
    for i in $(seq 1 100); do
        output=$(mpirun -np $ranks ./$prog 2>&1)
        time_val=$(echo "$output" | awk '/ВРЕМЯ_ВЫПОЛНЕНИЯ/ {print $2}')
        procs=$(echo "$output" | awk '/ПРОЦЕССОВ/ {print $2}')
        printf "Запуск %3d: %8.6f сек (процессов: %s)\n" $i $time_val $procs
        echo "$time_val" >> "$temp_file"
    done

    avg_time=$(awk '{sum+=$1} END {print sum/NR}' "$temp_file")
    printf "\nСреднее время: %8.6f сек\n\n" $avg_time
    rm "$temp_file"
}

run_test "task3" 1 "Последовательная версия (MPI, 1 процесс)"
run_test "task3" $RANKS "Параллельная версия (MPI, $RANKS процессов)"
