// Exercise 5.1 - source and destination do NOT match (modified message1.cc)
// Rank 0 sends to rank 1, but rank 1 waits for a message from rank 2.
// Build: mpicxx mismatch.cc -o mismatch
// Run:   mpirun -np 3 ./mismatch     -> hangs forever (press Ctrl+C)
//        mpirun -np 2 ./mismatch     -> error: rank 2 does not exist
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number;
    if (rank == 0) {
        number = 42;
        MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);       // destination = 1
        std::cout << "Process 0 sent " << number << " to process 1" << std::endl;
    } else if (rank == 1) {
        std::cout << "Process 1 waiting for a message from process 2..." << std::endl;
        MPI_Recv(&number, 1, MPI_INT, 2, 0, MPI_COMM_WORLD,        // source = 2  (WRONG)
                 MPI_STATUS_IGNORE);
        std::cout << "Process 1 received " << number << std::endl; // never printed
    }

    MPI_Finalize();
    return 0;
}
