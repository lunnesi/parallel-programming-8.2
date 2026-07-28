#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


double f(double x, double y) {
    return cos(x * x + y * y);
}

int main(int argc, char** argv) {
    int rank, size;
    //параметр области a (радиус) и число разбиений N
    double a = 1.0;
    long long N = 4000; 
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time = MPI_Wtime();

    //границы описанного квадрата для области D
    double x_min = -a, x_max = a;
    double y_min = -a, y_max = a;
    
    double dx = (x_max - x_min) / N;
    double dy = (y_max - y_min) / N;
    double cell_area = dx * dy;

    double local_sum = 0.0;

    //распределение итераций по процессам (по строкам x)
    for (long long i = rank; i < N; i += size) {
        double x = x_min + (i + 0.5) * dx;
        for (long long j = 0; j < N; j++) {
            double y = y_min + (j + 0.5) * dy;
            
            //проверка попадания точки в область D
            if (x * x + y * y <= a * a) {
                local_sum += f(x, y);
            }
        }
    }

    double total_sum = 0.0;

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        double result = total_sum * cell_area;
        printf("Calculated Integral: %.10f\n", result);
        printf("Execution Time: %.4f seconds\n", end_time - start_time);
        printf("Processes used: %d\n", size);
    }

    MPI_Finalize();
    return 0;
}
