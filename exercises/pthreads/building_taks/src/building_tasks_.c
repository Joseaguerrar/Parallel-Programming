#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Mutex para controlar el acceso exclusivo a la salida estándar
pthread_mutex_t mutex;

// Definición de los semáforos de cada tarea
sem_t sem_obra_gris;
sem_t sem_plomeria_ext;
sem_t sem_techo;
sem_t sem_pintura_ext;
sem_t sem_acabados_ext;
sem_t sem_instalacion_electrica;
sem_t sem_plomeria_int;
sem_t sem_pintura_int;
sem_t sem_piso;
sem_t sem_acabados_int;

// Estructura que representa cada tarea
typedef struct {
    const char* nombre;
    sem_t* mi_semaforo;
    sem_t* dependencias[2];  // Pueden tener hasta 2 dependencias
    int num_dependencias;
    int num_post;  // Número de veces que se debe hacer post
} Tarea;

// Función que simula el trabajo de un albañil
/**
 * @brief Función que simula el trabajo de un albañil, esperando las dependencias 
 *        y ejecutando su tarea una vez estén listas.
 *
 * @param arg Puntero genérico que será casteado a la estructura Tarea, 
 *            que contiene toda la información sobre la tarea.
 * 
 * La función espera a que todas las dependencias de la tarea se completen
 * (utilizando semáforos), luego simula el inicio y la ejecución de la tarea 
 * con un bloqueo opcional mediante mutex para controlar el acceso concurrente.
 * Al finalizar la tarea, desbloquea las tareas que dependen de esta.
 * 
 * @return NULL 
 */
void* ejecutar_tarea(void* arg) {
    Tarea* tarea = (Tarea*)arg;

    // Esperar a que todas las dependencias se completen
    for (int i = 0; i < tarea->num_dependencias; i++) {
        sem_wait(tarea->dependencias[i]);
    }

    /**
     * Iniciar la tarea. El mutex se utiliza para asegurar que las salidas
     * de impresión (inicio y fin de la tarea) se ejecuten en orden.
     * 
     * Si se comenta el uso del mutex, las tareas seguirán respetando las
     * prioridades y dependencias, pero el orden de inicio y finalización
     * en la salida puede variar debido a la concurrencia.
     */
    pthread_mutex_lock(&mutex);  // Bloqueo para asegurar salida controlada
    printf("%s ha comenzado.\n", tarea->nombre);
    sleep(1);  // Simular trabajo con una espera de 1 segundo
    printf("%s ha terminado.\n", tarea->nombre);
    pthread_mutex_unlock(&mutex);  // Liberar el mutex

    // Desbloquear las tareas dependientes
    for (int i = 0; i < tarea->num_post; i++) {
        sem_post(tarea->mi_semaforo);
    }

    return NULL;
}
int main() {
    // Inicializar los semáforos con el valor inicial de 0 (bloqueado)
    sem_init(&sem_obra_gris, 0, 1);  // La primera tarea (obra gris) puede empezar inmediatamente
    sem_init(&sem_plomeria_ext, 0, 0);
    sem_init(&sem_techo, 0, 0);
    sem_init(&sem_pintura_ext, 0, 0);
    sem_init(&sem_acabados_ext, 0, 0);
    sem_init(&sem_instalacion_electrica, 0, 0);
    sem_init(&sem_plomeria_int, 0, 0);
    sem_init(&sem_pintura_int, 0, 0);
    sem_init(&sem_piso, 0, 0);
    sem_init(&sem_acabados_int, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // Crear las tareas con sus respectivas dependencias y número de posts
    Tarea tareas[] = {
        {"Obra gris", &sem_obra_gris, {NULL}, 0, 2},
        // Desbloquea 2: Plomería exterior y Techo
        {"Plomería exterior", &sem_plomeria_ext, {&sem_obra_gris}, 1, 2},
        // Desbloquea 2: Plomería interior y Pintura exterior
        {"Techo", &sem_techo, {&sem_obra_gris}, 1, 1},
        // Desbloquea 1: Instalación eléctrica
        {"Pintura exterior", &sem_pintura_ext, {&sem_plomeria_ext}, 1, 1},
        // Desbloquea 1: Acabados exteriores
        {"Acabados exteriores", &sem_acabados_ext, {&sem_pintura_ext}, 1, 0},
        // No desbloquea a nadie
        {"Instalación eléctrica", &sem_instalacion_electrica,
        {&sem_techo}, 1, 1}, // Desbloquea 1: Pintura interior
        {"Plomería interior", &sem_plomeria_int, {&sem_plomeria_ext}, 1, 1},
        // Desbloquea 1: Pintura interior
        {"Pintura interior", &sem_pintura_int, {&sem_plomeria_int,
        &sem_instalacion_electrica}, 2, 1}, // Desbloquea 1: Piso
        {"Piso", &sem_piso, {&sem_pintura_int}, 1, 1},
        // Desbloquea 1: Acabados interiores
        {"Acabados interiores", &sem_acabados_int, {&sem_piso}, 1, 0}
        // No desbloquea a nadie
    };

    // Crear hilos para cada tarea
    pthread_t hilos[sizeof(tareas) / sizeof(Tarea)];

    for (size_t i = 0; i < sizeof(tareas) / sizeof(Tarea); i++) {
        pthread_create(&hilos[i], NULL, ejecutar_tarea, &tareas[i]);
    }

    // Esperar a que todos los hilos terminen
    for (size_t i = 0; i < sizeof(tareas) / sizeof(Tarea); i++) {
        pthread_join(hilos[i], NULL);
    }

    // Destruir semáforos y mutex
    sem_destroy(&sem_obra_gris);
    sem_destroy(&sem_plomeria_ext);
    sem_destroy(&sem_techo);
    sem_destroy(&sem_pintura_ext);
    sem_destroy(&sem_acabados_ext);
    sem_destroy(&sem_instalacion_electrica);
    sem_destroy(&sem_plomeria_int);
    sem_destroy(&sem_pintura_int);
    sem_destroy(&sem_piso);
    sem_destroy(&sem_acabados_int);
    pthread_mutex_destroy(&mutex);

    return 0;
}
