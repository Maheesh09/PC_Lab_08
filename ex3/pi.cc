// Exercise 3 - Monte Carlo estimate of Pi with MPI (10,000,000 points in total)
// Based on the Dartmouth serial example: throw random points into the unit square,
// count how many fall inside the quarter circle  ->  pi = 4 * inside / total
// Build: mpicxx -O2 pi.cc -o pi        Run: mpirun -np 4 ./pi
#include <mpi.h>
#include <cstdio>
#include <cstdlib>

#define SEED 35791246

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long niter = 10000000;              // total points (all processes together)

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    // 1. My share of the points (first niter % size ranks take one extra)
    long my_iter = niter / size + (rank < niter % size ? 1 : 0);

    // 2. Different seed on every process, otherwise all would throw the SAME points
    srand(SEED + rank);

    long count = 0;                           // points inside the quarter circle
    for (long i = 0; i < my_iter; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) count++;
    }

    // 3. Workers send their count to rank 0, rank 0 receives them in rank order
    if (rank != 0) {
        MPI_Send(&count, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        long total = count;
        for (int r = 1; r < size; r++) {
            long c;
            MPI_Recv(&c, 1, MPI_LONG, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += c;
        }
        double pi = 4.0 * total / niter;
        double elapsed = MPI_Wtime() - start;
        printf("# of trials = %ld, estimate of pi is %.8f\n", niter, pi);
        printf("Processes = %d   Time = %.6f s\n", size, elapsed);
    }

    MPI_Finalize();
    return 0;
}
