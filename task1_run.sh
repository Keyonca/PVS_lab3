#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Использование: $0 <число_рангов> <кол-во_элементов>"
    echo "Пример: $0 8 250000"
    exit 1
fi

RANKS=$1
ARRAY_SIZE=$2
export ARRAY_SIZE

EXEC="./task1_par"

if [ ! -x "$EXEC" ]; then
    echo "Ошибка: $EXEC не найден или не исполняемый"
    exit 2
fi

echo "=== Запуск $EXEC с $RANKS рангов и $ARRAY_SIZE элементов (100 раз) ==="

# Используем awk для вычислений с плавающей точкой
awk_script=
BEGIN { total = 0 }
{
    printf "Run #%3d: %.12f сек\n", NR, $0
    total += $0
}
END {
    avg = total / NR
    printf "-----------------------------------------------\n"
    printf "Среднее время для %d рангов и %d элементов: %.12f сек\n", ranks, array_size, avg
}

# Собираем все времена в массив
times=()
for i in $(seq 1 100); do
    output=$(mpirun -np "$RANKS" "$EXEC" 2>&1)
    time_result=$(echo "$output" | awk -F':' '/ELAPSED_TIME/ {print $2}')
    
    if [ -z "$time_result" ]; then
        echo "Ошибка на запуске $i: время не получено"
        echo "Полный вывод программы:"
        echo "$output"
        exit 3
    fi

    times+=("$time_result")
done

# Передаем все времена в awk для обработки
printf "%s\n" "${times[@]}" | awk -v ranks="$RANKS" -v array_size="$ARRAY_SIZE" "$awk_script"
