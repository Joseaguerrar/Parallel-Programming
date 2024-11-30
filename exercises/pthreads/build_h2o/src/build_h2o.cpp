// Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>

using namespace std; // NOLINT

/// @brief Semáforo que controla la disponibilidad de átomos de hidrógeno.
sem_t hydrogen_sem;
/// @brief Semáforo que controla la disponibilidad de átomos de oxígeno.
sem_t oxygen_sem;
/// @brief Mutex para proteger el acceso a los contadores de átomos.
pthread_mutex_t mutex;
/// @brief Condición para sincronizar hilos cuando se completa una molécula.
pthread_cond_t condition;

/// @brief Contador de átomos de hidrógeno disponibles.
int hydrogen_count = 0;
/// @brief Contador de átomos de oxígeno disponibles.
int oxygen_count = 0;

/// @brief Simula el enlace de un átomo en la formación de una molécula.
/// @param atom Tipo de átomo que se enlaza (Hydrogen u Oxygen).
void bond(const string& atom) {
    cout << atom << " is bonding..." << endl;
    usleep(100000);  // Simula un pequeño retraso
}

/// @brief Lógica del hilo que representa un átomo de hidrógeno.
/// @param arg Argumento del hilo (no utilizado).
/// @return Siempre retorna nullptr.
void* hydrogen(void* arg) {
    (void)arg;  // Ignorar el parámetro no utilizado para evitar advertencias
    pthread_mutex_lock(&mutex);

    hydrogen_count++;
    if (hydrogen_count >= 2 && oxygen_count >= 1) {
        sem_post(&hydrogen_sem);
        sem_post(&hydrogen_sem);
        sem_post(&oxygen_sem);
        hydrogen_count -= 2;
        oxygen_count -= 1;
        pthread_cond_broadcast(&condition);
    } else {
        pthread_cond_wait(&condition, &mutex);
    }

    pthread_mutex_unlock(&mutex);

    sem_wait(&hydrogen_sem);
    bond("Hydrogen");
    return nullptr;
}

/// @brief Lógica del hilo que representa un átomo de oxígeno.
/// @param arg Argumento del hilo (no utilizado).
/// @return Siempre retorna nullptr.
void* oxygen(void* arg) {
    (void)arg;  // Ignorar el parámetro no utilizado para evitar advertencias
    pthread_mutex_lock(&mutex);

    oxygen_count++;
    if (hydrogen_count >= 2 && oxygen_count >= 1) {
        sem_post(&hydrogen_sem);
        sem_post(&hydrogen_sem);
        sem_post(&oxygen_sem);
        hydrogen_count -= 2;
        oxygen_count -= 1;
        pthread_cond_broadcast(&condition);
    } else {
        pthread_cond_wait(&condition, &mutex);
    }

    pthread_mutex_unlock(&mutex);

    sem_wait(&oxygen_sem);
    bond("Oxygen");
    return nullptr;
}

/// @brief Punto de entrada principal del programa.
///
/// Permite al usuario ingresar 'H' para hidrógeno, 'O' para oxígeno,
/// o 'E' para salir del programa. Crea hilos para cada átomo ingresado
/// y sincroniza la formación de moléculas de agua.
/// @return Retorna 0 al finalizar el programa.
int main() {
    // Inicialización de semáforos y mutex
    sem_init(&hydrogen_sem, 0, 0);
    sem_init(&oxygen_sem, 0, 0);
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&condition, nullptr);

    pthread_t threads[100];  // Para rastrear hilos
    int thread_count = 0;
    char input;

    cout << "Enter 'H' for Hydrogen, 'O' for Oxygen, or 'E' to Exit:" << endl;

    while (true) {
        cin >> input;
        if (input == 'H' || input == 'h') {
            pthread_create(&threads[thread_count++], nullptr,
                                                             hydrogen, nullptr);
        } else if (input == 'O' || input == 'o') {
            pthread_create(&threads[thread_count++], nullptr, oxygen, nullptr);
        } else if (input == 'E' || input == 'e') {
            cout << "Exiting..." << endl;
            break;
        } else {
            cout << "Invalid input. Enter 'H', 'O', or 'E'." << endl;
        }
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], nullptr);
    }

    // Limpieza de semáforos y mutex
    sem_destroy(&hydrogen_sem);
    sem_destroy(&oxygen_sem);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return 0;
}
