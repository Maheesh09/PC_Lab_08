// Exercise 5.2 - message2.cc rewritten with Buffered Send (MPI_Bsend)
// Each message uses its OWN variable (numbers[0], numbers[1], numbers[2]),
// so the data being sent is never overwritten.
// Build: mpicxx message2_bsend.cc -o message2_bsend     Run: mpirun -np 2 ./message2_bsend
#include <mpi.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int COUNT = 3;

    if (rank == 0) {
        int numbers[COUNT];                                  // one variable per message

        // 1. Attach a buffer big enough for 3 ints + MPI's bookkeeping per message
        int bufsize = COUNT * (sizeof(int) + MPI_BSEND_OVERHEAD);
        char* buffer = (char*)malloc(bufsize);
        MPI_Buffer_attach(buffer, bufsize);

        // 2. Bsend copies each value into the buffer and returns immediately
        for (int i = 0; i < COUNT; i++) {
            numbers[i] = i * 10;
            MPI_Bsend(&numbers[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            std::cout << "Process 0 sent " << numbers[i] << std::endl;
        }

        // 3. Detach waits until all buffered messages have been delivered
        MPI_Buffer_detach(&buffer, &bufsize);
        free(buffer);
    } else if (rank == 1) {
        int received[COUNT];
        for (int i = 0; i < COUNT; i++) {
            MPI_Recv(&received[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Process 1 received " << received[i] << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
