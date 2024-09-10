#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Estructura para representar cada tarea y sus dependencias
typedef struct {
    const char* nombre;
    sem_t* dependencias; // Array de semáforos de las tareas de las que depende
    int num_dependencias; // Número de dependencias
    sem_t* mi_semaforo; // Semáforo propio que desbloqueará las tareas que dependen de esta
} Tarea;

// Nombres de las tareas
const char* task_names[TASK_COUNT] = {
    "Obra gris",
    "Plomería exterior",
    "Pintura exterior",
    "Acabados exteriores",
    "Plomería interior",
    "Techo",
    "Instalación eléctrica",
    "Pintura interior",
    "Piso",
    "Acabados interiores"
};

// Dependencias entre tareas (cada tarea puede depender de varias tareas)
int dependencies[TASK_COUNT][3] = {
    {},           // Tarea 0: No depende de ninguna
    {0},          // Tarea 1: Depende de Obra gris
    {1},          // Tarea 2: Depende de Plomería exterior
    {2},          // Tarea 3: Depende de Pintura exterior
    {1},          // Tarea 4: Depende de Plomería exterior
    {0},          // Tarea 5: Depende de Obra gris
    {5},          // Tarea 6: Depende de Techo
    {4, 6},       // Tarea 7: Depende de Plomería interior e Instalación eléctrica
    {7},          // Tarea 8: Depende de Pintura interior
    {8}           // Tarea 9: Depende de Piso
};

// Función para simular el trabajo de cada albañil (hilo de trabajo)
void* builder(void* arg);

int main(int argc, char* argv[]) {
    pthread_t builders[TASK_COUNT];
    TaskInfo task_info[TASK_COUNT];

    // Inicializar los semáforos para cada tarea
    for (int i = 0; i < TASK_COUNT; i++) {
        sem_init(&task_dependencies[i], 0, 0);
    }

    // Crear las dependencias para cada tarea
    for (int i = 0; i < TASK_COUNT; i++) {
        task_info[i].task_id = i;
        task_info[i].num_dependencies = sizeof(dependencies[i]) / sizeof(int);
        task_info[i].dependencies = malloc(task_info[i].num_dependencies * sizeof(int));

        // Asignar dependencias
        for (int j = 0; j < task_info[i].num_dependencies; j++) {
            task_info[i].dependencies[j] = dependencies[i][j];
        }
    }

    // Crear los hilos de albañiles para cada tarea
    for (int i = 0; i < TASK_COUNT; i++) {
        pthread_create(&builders[i], NULL, builder, (void*)&task_info[i]);
    }

    // La primera tarea (Obra gris) no tiene dependencias, así que puede empezar inmediatamente
    sem_post(&task_dependencies[0]);

    // Esperar a que todos los albañiles terminen sus tareas
    for (int i = 0; i < TASK_COUNT; i++) {
        pthread_join(builders[i], NULL);
    }

    // Destruir semáforos y liberar memoria
    for (int i = 0; i < TASK_COUNT; i++) {
        sem_destroy(&task_dependencies[i]);
        free(task_info[i].dependencies);
    }

    printf("El trabajo se ha completado...\n");
    return 0;
}

void* builder(void* arg) {
    TaskInfo* task_info = (TaskInfo*) arg;
    int task_id = task_info->task_id;

    // Esperar hasta que todas las dependencias se completen
    for (int i = 0; i < task_info->num_dependencies; i++) {
        int dep_id = task_info->dependencies[i];
        sem_wait(&task_dependencies[dep_id]);
    }

    // Iniciar la tarea
    printf("El albañil ha comenzado su tarea: %s\n", task_names[task_id]);

    // Simular el trabajo del albañil con una espera aleatoria
    usleep(25);

    // Finalizar la tarea
    printf("El albañil ha terminado su tarea: %s\n", task_names[task_id]);

    // Marcar la tarea como completada
    sem_post(&task_dependencies[task_id]);

    pthread_exit(NULL);
}
