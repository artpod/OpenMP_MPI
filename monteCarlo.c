#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

double getRandomNumber(unsigned int *seed) { // Функція приймає вказівник на зерно генерації випадкових чисел і використовує rand_r() для генерації
    return (double) rand_r(seed) * 2 / (double) (RAND_MAX) - 1; // випадкового числа у діапазоні [-1, 1].
}

long double SequentialPi(long long iterations) { // Функція виконує послідовний алгоритм Монте-Карло для обчислення числа π. Вона 
    long long numberInCircle = 0;                // генерує випадкові точки в квадраті зі стороною 2 (від -1 до 1 по обом осям) і підраховує, 
    unsigned int seed = (unsigned int) time(NULL); // скільки з цих точок потрапило в одиничне коло. Число π визначається як 4 * (кількість точок у колі / загальна кількість точок).

    for (long long int i = 0; i < iterations; i++) {
        double x = getRandomNumber(&seed);
        double y = getRandomNumber(&seed);
        double distanceSquared = x*x + y*y;

        if (distanceSquared <= 1)
            numberInCircle++;
    }

    return 4 *(double) numberInCircle / ((double) iterations);
}

long double ParallelPi(long long iterations) { // Ця функція використовує OpenMP для паралельного виконання алгоритму Монте-Карло.
    long long numberInCircle = 0;
#pragma omp parallel num_threads(8) // ця директива створює паралельну область, де кожна нитка має своє зерно для генерації випадкових чисел,
    {                               // що забезпечує незалежність генерованих послідовностей.
        /* генеруємо зерно для кожного потоку різним */
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) omp_get_thread_num();
#pragma omp for reduction(+: numberInCircle) // паралельно виконує цикл і використовує редукцію для сумування кількості точок, що потрапили в коло, з усіх ниток.
        for (long long int i = 0; i < iterations; i++) {
            double x = getRandomNumber(&seed);
            double y = getRandomNumber(&seed);
            double distanceSquared = x*x + y*y;

            if (distanceSquared <= 1)
                numberInCircle++;
        }
    }
    return 4 * (double) numberInCircle/((double) iterations);
}

// gcc-11 -fopenmp monteCarlo.c -o monteCarlo
// ./monteCarlo

int main() { // тут використовуються функції gettimeofday(&start, NULL) і gettimeofday(&end, NULL) для вимірювання часу виконання послідовного і паралельного 
    struct timeval start, end; // обчислення числа π. Різниця між end і start дає час виконання в секундах.

    long long iterations = 1000000000;

    printf("\nПослідовне обчислення: \n");
    gettimeofday(&start, NULL);
    long double sequentialPi = SequentialPi(iterations);
    gettimeofday(&end, NULL);
    printf("Час виконання %f секунд\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("\nПаралельне обчислення: \n");
    gettimeofday(&start, NULL);
    long double parallelPi = ParallelPi(iterations);
    gettimeofday(&end, NULL);
    printf("Час виконання %f секунд\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Послідовне: Pi = %.10Lf \n", sequentialPi);
    printf("Паралельне: Pi = %.10Lf \n", parallelPi);

    return 0;
}