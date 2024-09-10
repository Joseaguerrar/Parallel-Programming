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

int main() {
    // Inicializar semáforos
    sem_t sem_obra_gris, sem_plomeria_ext, sem_techo, sem_pintura_ext,
    sem_pintura_int, sem_piso, sem_acabados_ext, sem_acabados_int,
    sem_instalacion_electrica, sem_plomeria_int;
    
    sem_init(&sem_obra_gris, 0, 1);  // Obra gris puede empezar inmediatamente
    sem_init(&sem_plomeria_ext, 0, 0);
    sem_init(&sem_techo, 0, 0);
    sem_init(&sem_pintura_ext, 0, 0);
    sem_init(&sem_pintura_int, 0, 0);
    sem_init(&sem_piso, 0, 0);
    sem_init(&sem_acabados_ext, 0, 0);
    sem_init(&sem_acabados_int, 0, 0);
    sem_init(&sem_instalacion_electrica, 0, 0);
    sem_init(&sem_plomeria_int, 0, 0);

    // Crear las tareas y definir sus dependencias
    Tarea tareas[] = {
        {"Obra gris", NULL, 0, &sem_obra_gris},
        {"Plomería exterior", (sem_t[]){sem_obra_gris}, 1, &sem_plomeria_ext},
        {"Techo", (sem_t[]){sem_obra_gris}, 1, &sem_techo},
        {"Pintura exterior", (sem_t[]){sem_plomeria_ext}, 1, &sem_pintura_ext},
        {"Acabados exteriores", (sem_t[]){sem_pintura_ext}, 1, &sem_acabados_ext},
        {"Instalación eléctrica", (sem_t[]){sem_techo}, 1, &sem_instalacion_electrica},
        {"Plomería interior", (sem_t[]){sem_plomeria_ext}, 1, &sem_plomeria_int},
        {"Pintura interior", (sem_t[]){sem_plomeria_int, sem_instalacion_electrica}, 2, &sem_pintura_int},
        {"Piso", (sem_t[]){sem_pintura_int}, 1, &sem_piso},
        {"Acabados interiores", (sem_t[]){sem_piso}, 1, &sem_acabados_int}
    };

    // Crear hilos para cada tarea
    pthread_t hilos[sizeof(tareas) / sizeof(Tarea)];
    
    for (int i = 0; i < sizeof(tareas) / sizeof(Tarea); i++) {
        pthread_create(&hilos[i], NULL, ejecutar_tarea, &tareas[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < sizeof(tareas) / sizeof(Tarea); i++) {
        pthread_join(hilos[i], NULL);
    }

    // Destruir semáforos
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

    return 0;
}

void* ejecutar_tarea(void*){

}
