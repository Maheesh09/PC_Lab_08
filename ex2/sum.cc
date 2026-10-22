// Exercise 2 - add up 1 .. 10,000,000 in parallel with MPI (point-to-point)
// Build: mpicxx -O2 sum.cc -o sum      Run: mpirun -np 4 ./sum
#include <mpi.h>
#include <cstdio>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const long long N = 10000000;

    MPI_Barrier(MPI_COMM_WORLD);              // start the clock together
    double start = MPI_Wtime();

    // 1. Work out MY range of numbers.  (first N % size ranks take one extra)
    long long chunk = N / size;
    long long extra = N % size;
    long long first = rank * chunk + (rank < extra ? rank : extra) + 1;
    long long last  = first + chunk - 1 + (rank < extra ? 1 : 0);

    // 2. Add my numbers (no array is needed - the numbers are just first..last)
    long long local_sum = 0;
    for (long long i = first; i <= last; i++)
        local_sum += i;

    // 3. Workers send their partial sum to rank 0; rank 0 adds them up
    if (rank != 0) {
        MPI_Send(&local_sum, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        long long total = local_sum;
        for (int r = 1; r < size; r++) {
            long long part;
            MPI_Recv(&part, 1, MPI_LONG_LONG, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += part;
        }
        double elapsed = MPI_Wtime() - start;

        long long expected = N * (N + 1) / 2;
        printf("Sum 1..%lld = %lld  (expected %lld) %s\n",
               N, total, expected, total == expected ? "CORRECT" : "WRONG");
        printf("Processes = %d   Time = %.6f s\n", size, elapsed);
    }

    MPI_Finalize();
    return 0;
}
