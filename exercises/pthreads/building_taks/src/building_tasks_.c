#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Para sleep()

#define TASK_COUNT 10 //Cantidad de tareas

// Lista de semáforos, uno para cada tarea
sem_t task_dependencies[TASK_COUNT];

// Estructura para representar cada tarea
typedef struct {
    int task_id;
    int *dependencies;
    int num_dependencies;
} TaskInfo;

// Dependencias entre tareas (grafo de tareas)
int task_deps[TASK_COUNT][2] = {
    {0, 0},    // 0: Obra gris no depende de nada
    {0, 1},    // 1: Plomería exterior depende de obra gris
    {1, 1},    // 2: Pintura exterior depende de plomería exterior
    {2, 1},    // 3: Acabados exteriores dependen de pintura exterior
    {1, 1},    // 4: Plomería interior depende de plomería exterior
    {0, 1},    // 5: Techo depende de obra gris
    {5, 1},    // 6: Instalación eléctrica depende de techo
    {4, 2},    // 7: Pintura interior depende de plomería interior e instalación eléctrica
    {7, 1},    // 8: Piso depende de pintura interior
    {8, 1}     // 9: Acabados interiores dependen de piso
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
        task_info[i].num_dependencies = task_deps[i][1];
        task_info[i].dependencies = malloc(task_info[i].num_dependencies * sizeof(int));

        // Asignar dependencias
        for (int j = 0; j < task_info[i].num_dependencies; j++) {
            task_info[i].dependencies[j] = task_deps[i][j];
        }
    }

    //  Crear los hilos de los albañiles para cada tarea

    for (int i = 0; i < TASK_COUNT; i++)
    {
        pthread_create(&builders[i], NULL, builder, (void*)&task_info[i]);
    }

    sem_post(&task_dependencies[0]);

    //  Esperar a que todos los albañiles terminen sus tareas
    for (int i = 0; i < TASK_COUNT; i++)
    {
        pthread_join(builders[i], NULL);
    }

    //  Destruir semáforos y liberar memoria

        for (int i = 0; i < TASK_COUNT; i++)
        {
            sem_destroy(&task_dependencies[i]);
            free(task_info[i].dependencies);
        }
        
    printf("El trabajo se ha completado...\n");
    return 0;
    
}

void* builder(void* arg){
    TaskInfo* task_info=(TaskInfo*) arg;

    int task_id= task_info->task_id;
    for (int i = 0; i < task_info->num_dependencies; i++)
    {
        int dep_id=task_info->dependencies[i];
        sem_wait(&task_dependencies[i]);
    }
    
    //Iniciar la tarea
    printf("El albañil empezó su tarea\n", task_id);

    //Simular la espera por el trabajo
    sleep(randr()% 3 +1);

    //Finalizar la tarea
    printf("El albañil terminó su trabajo\n", task_id);

    //Marca el trabajo como completo
    sem_post(&task_dependencies[task_id]);

    pthread_exit(NULL);

}