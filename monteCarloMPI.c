//
//
// mpicc monteCarloMPI.c -o monteCarloMPI
// mpirun --use-hwthread-cpus monteCarloMPI with virtual cores
// mpirun -np 2 monteCarloMPI
// mpirun --oversubscribe  -np 4 ./monteCarloMPI



#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#define SEED 35791246

int main(int argc, char* argv[])
{
    long iterations = 1000000000;
    int pid;                       
    double x,y;                     
    int i, count=0;                 
    double z;                       
    double pi;                      
    int nodenum;
    long iterPerNode;
    double start, end;
    int finalcount = 0;

    MPI_Init(&argc, &argv);//Ініціалізація середовища MPI, підготовка програми до виконання паралельних обчислень
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); // Визначає унікальний ідентифікатор поточного процесу (pid) в рамках комунікатора MPI_COMM_WORLD, який включає всі запущені процеси.
    MPI_Comm_size(MPI_COMM_WORLD, &nodenum); // Визначає загальну кількість процесів у комунікаторі MPI_COMM_WORLD і зберігає це число у змінній nodenum.
    MPI_Barrier(MPI_COMM_WORLD); // Блокує виконання всіх процесів в комунікаторі до того моменту, поки всі процеси не досягнуть цього бар'єру.
    int recieved[nodenum];
    iterPerNode = iterations/(nodenum);
    srand(SEED + pid);

    start = MPI_Wtime(); // Повертає час у секундах, що минув від певного моменту в минулому, зазвичай від початку епохи Unix.

    if(pid != 0)
    {
        for (i = 0; i < iterPerNode; ++i)
        {
            x= ((double)rand())/RAND_MAX;
            y =((double)rand())/RAND_MAX;
            z = sqrt(x*x+y*y);
            if (z<=1)
            {
                count++;
            }
        }

        for(i = 0; i < nodenum; ++i)
        {
            MPI_Send(&count,
                     1,
                     MPI_INT,
                     0,
                     1,
                     MPI_COMM_WORLD); // Відправляє кількість точок, що потрапили всередину чверті кола (змінна count), від поточного процесу до процесу з ідентифікатором 0 (головний процес).
        }
    }
    else if (pid == 0)
    {
        for(i = 0; i < nodenum; ++i)
        {
            MPI_Recv(&recieved[i],
                     nodenum,
                     MPI_INT,
                     MPI_ANY_SOURCE,
                     1,
                     MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE); // Отримує кількість точок, відправлену іншими процесами, у головний процес (з ідентифікатором 0). Використовується для збору результатів обчислень від усіх процесів.
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (pid == 0)
    {
        for(i = 0; i < nodenum; ++i)
        {
            finalcount += recieved[i];
        }
        end = MPI_Wtime();
        pi = ((double)finalcount/(double)iterations)*4.0;
        printf("Pi: %10f\nЧас виконання: %f секунд\nІтерації: %ld\n Кількість процесів: %d\n", pi, end - start, iterPerNode * (nodenum), nodenum);
    }

    MPI_Finalize(); // Завершує роботу з MPI, вивільняючи всі ресурси, які були виділені MPI. Після виклику цієї функції використання будь-яких функцій MPI стає неприпустимим.
    return 0;
}
