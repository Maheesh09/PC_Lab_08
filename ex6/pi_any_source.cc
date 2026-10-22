// Exercise 6 - Exercise 3 (Monte Carlo Pi) with the receive SOURCE changed to MPI_ANY_SOURCE
// Rank 0 now accepts results in whatever order they arrive and uses status.MPI_SOURCE
// to see who sent each one.
// Build: mpicxx -O2 pi_any_source.cc -o pi_any_source    Run: mpirun -np 4 ./pi_any_source
#include <mpi.h>
#include <cstdio>
#include <cstdlib>

#define SEED 35791246

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long niter = 10000000;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    long my_iter = niter / size + (rank < niter % size ? 1 : 0);
    srand(SEED + rank);

    long count = 0;
    for (long i = 0; i < my_iter; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) count++;
    }

    if (rank != 0) {
        MPI_Send(&count, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        long total = count;
        MPI_Status status;
        printf("Arrival order:");
        for (int r = 1; r < size; r++) {
            long c;
            // CHANGED: take the next result from ANY worker (first come, first served)
            MPI_Recv(&c, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf(" %d", status.MPI_SOURCE);           // who actually sent it
            total += c;
        }
        printf("\n");
        double pi = 4.0 * total / niter;
        double elapsed = MPI_Wtime() - start;
        printf("# of trials = %ld, estimate of pi is %.8f\n", niter, pi);
        printf("Processes = %d   Time = %.6f s\n", size, elapsed);
    }

    MPI_Finalize();
    return 0;
}
