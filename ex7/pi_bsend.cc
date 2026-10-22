// Exercise 7 - Exercise 6 rewritten with Buffered Send (MPI_Bsend) on the workers
// Build: mpicxx -O2 pi_bsend.cc -o pi_bsend    Run: mpirun -np 4 ./pi_bsend
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
        // CHANGED: attach a buffer, Bsend into it, detach
        int bufsize = sizeof(long) + MPI_BSEND_OVERHEAD;     // room for 1 message
        char* buffer = (char*)malloc(bufsize);
        MPI_Buffer_attach(buffer, bufsize);

        MPI_Bsend(&count, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD); // returns as soon as copied

        MPI_Buffer_detach(&buffer, &bufsize);                 // waits until it is delivered
        free(buffer);
    } else {
        long total = count;
        MPI_Status status;
        printf("Arrival order:");
        for (int r = 1; r < size; r++) {
            long c;
            MPI_Recv(&c, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf(" %d", status.MPI_SOURCE);
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
