#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Использование: $0 <число_процессов> <размер_матрицы>"
    exit 1
fi

RANKS=$1
MATRIX_SIZE=$2

echo "=== Запуск тестов (100 итераций) ==="

run_test() {
    local prog=$1
    local ranks=$2
    local args=$3
    local desc=$4
    
    echo "--- $desc ---"

    temp_file=$(mktemp)
    
    for i in $(seq 1 100); do
        output=$(mpirun -np $ranks ./$prog $args 2>&1)
        time_val=$(echo "$output" | awk -F': | сек' '/ВРЕМЯ_ВЫПОЛНЕНИЯ/ {print $(NF-1)}')
        procs=$(echo "$output" | awk '/ПРОЦЕССОВ/ {print $2}')
        size=$(echo "$output" | awk '/РАЗМЕР_МАТРИЦЫ/ {print $2}')
        
        printf "Запуск %3d: %.6f сек | Процессов: %s | Размер: %s\n" $i $time_val $procs $size
        echo "$time_val" >> "$temp_file"
    done

    avg_time=$(awk '{sum+=$1} END {printf "%.6f", sum/NR}' "$temp_file")
    min_time=$(awk 'NR==1 || $1<min {min=$1} END {printf "%.6f", min}' "$temp_file")
    max_time=$(awk 'NR==1 || $1>max {max=$1} END {printf "%.6f", max}' "$temp_file")
    
    printf "\nРезультаты для %s:\n" "$desc"
    printf "  Среднее время: %.6f сек\n" $avg_time
    printf "  Минимальное время: %.6f сек\n" $min_time
    printf "  Максимальное время: %.6f сек\n\n" $max_time
    
    rm "$temp_file"
}

# Запуск последовательной версии (1 процесс)
run_test "task4_seq" 1 "$MATRIX_SIZE" "Последовательная версия (MPI, 1 процесс)"
