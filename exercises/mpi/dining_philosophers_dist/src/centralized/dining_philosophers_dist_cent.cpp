#include <queue>
#include <utility>
#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace std;

void philosopher(int rank, int num_philosophers) {
    int left = (rank + 1) % num_philosophers; // Palillo izquierdo
    int right = rank;                         // Palillo derecho
    MPI_Status status;

    while (true) {
        cout << "Filósofo " << rank << " está pensando..." << endl;
        sleep(rand() % 3 + 1); // Pensar por un tiempo aleatorio

        cout << "Filósofo " << rank << " tiene hambre." << endl;
        int request_msg[2] = {0, rank}; // REQUEST = 0
        MPI_Send(request_msg, 2, MPI_INT, num_philosophers, 0, MPI_COMM_WORLD);

        // Esperar confirmación para comer
        MPI_Recv(NULL, 0, MPI_INT, num_philosophers, 2, MPI_COMM_WORLD, &status); // GRANT = 2
        cout << "Filósofo " << rank << " está comiendo." << endl;
        sleep(rand() % 3 + 1); // Comer por un tiempo aleatorio

        // Liberar los palillos
        int release_msg[2] = {1, rank}; // RELEASE = 1
        MPI_Send(release_msg, 2, MPI_INT, num_philosophers, 0, MPI_COMM_WORLD);
        cout << "Filósofo " << rank << " ha terminado de comer y ha liberado los palillos." << endl;
    }
}

void waiter(int num_philosophers) {
    int *chopsticks = (int *)calloc(num_philosophers, sizeof(int)); // Estado de los palillos (0: libre, 1: ocupado)
    queue<pair<int, int>> request_queue; // Cola para solicitudes: (tipo, filósofo)
    MPI_Status status;

    while (true) {
        int msg[2];
        MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        int type = msg[0];       // Tipo de mensaje (REQUEST o RELEASE)
        int philosopher = msg[1];
        int left = (philosopher + 1) % num_philosophers;
        int right = philosopher;

        if (type == 0) { // REQUEST
            request_queue.push({type, philosopher}); // Agregar la solicitud a la cola
        } else if (type == 1) { // RELEASE
            chopsticks[left] = 0;
            chopsticks[right] = 0;
            cout << "Mesero: Filósofo " << philosopher << " ha liberado los palillos." << endl;
        }

        // Procesar la cola
        while (!request_queue.empty()) {
            auto [req_type, req_philosopher] = request_queue.front();
            int req_left = (req_philosopher + 1) % num_philosophers;
            int req_right = req_philosopher;

            if (chopsticks[req_left] == 0 && chopsticks[req_right] == 0) {
                // Palillos disponibles, otorgar permiso
                chopsticks[req_left] = 1;
                chopsticks[req_right] = 1;
                cout << "Mesero: Otorgando permiso para comer al filósofo " << req_philosopher << "." << endl;
                MPI_Send(NULL, 0, MPI_INT, req_philosopher, 2, MPI_COMM_WORLD); // GRANT
                request_queue.pop(); // Eliminar la solicitud procesada
            } else {
                // Si no hay palillos disponibles, salir del ciclo y esperar la próxima iteración
                break;
            }
        }
    }

    free(chopsticks);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int num_philosophers = 5; // Número de filósofos

    if (size != num_philosophers + 1) {
        if (rank == 0) {
            cerr << "Error: Debes iniciar " << num_philosophers + 1
                      << " procesos (1 mesero + " << num_philosophers << " filósofos)." << endl;
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if (rank == num_philosophers) {
        waiter(num_philosophers); // El último proceso actúa como mesero
    } else {
        philosopher(rank, num_philosophers); // Los demás procesos son filósofos
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
