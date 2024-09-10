#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Para sleep()

#define job_count 10 //Cantidad de tareas

// Lista de semáforos, uno para cada tarea
sem_t job_dependencies[job_count];

typedef struct{
    int job_id;
    int *dependencies;
    int num_dependencies;
}   jobinfo;