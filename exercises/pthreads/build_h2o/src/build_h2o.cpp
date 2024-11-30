// Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>

using namespace std;//NOLINT
// Semáforos para sincronización
sem_t hydrogen_sem;
sem_t oxygen_sem;
pthread_mutex_t mutex;
pthread_cond_t condition;

// Contadores de átomos listos
int hydrogen_count = 0;
int oxygen_count = 0;

void bond(const string& atom) {
    // Simula el proceso de enlazar moléculas
    cout << atom << " is bonding..." << endl;
    usleep(100000);  // Simula un pequeño retraso
}

void* hydrogen(void* arg) {
    pthread_mutex_lock(&mutex);

    // Incrementar contador de hidrógeno
    hydrogen_count++;
    if (hydrogen_count >= 2 && oxygen_count >= 1) {
        // Si hay suficientes átomos, liberar semáforos para una molécula
        sem_post(&hydrogen_sem);
        sem_post(&hydrogen_sem);
        sem_post(&oxygen_sem);
        hydrogen_count -= 2;
        oxygen_count -= 1;
        // Notificar a todos los hilos esperando
        pthread_cond_broadcast(&condition);
    } else {
        // Esperar a completar una molécula
        pthread_cond_wait(&condition, &mutex);
    }

    pthread_mutex_unlock(&mutex);

    // Esperar para proceder a bond()
    sem_wait(&hydrogen_sem);
    bond("Hydrogen");
    return nullptr;
}

void* oxygen(void* arg) {
    pthread_mutex_lock(&mutex);

    // Incrementar contador de oxígeno
    oxygen_count++;
    if (hydrogen_count >= 2 && oxygen_count >= 1) {
        // Si hay suficientes átomos, liberar semáforos para una molécula
        sem_post(&hydrogen_sem);
        sem_post(&hydrogen_sem);
        sem_post(&oxygen_sem);
        hydrogen_count -= 2;
        oxygen_count -= 1;
        // Notificar a todos los hilos esperando
        pthread_cond_broadcast(&condition);
    } else {
        // Esperar a completar una molécula
        pthread_cond_wait(&condition, &mutex);
    }

    pthread_mutex_unlock(&mutex);

    // Esperar para proceder a bond()
    sem_wait(&oxygen_sem);
    bond("Oxygen");
    return nullptr;
}

int main() {
    // Inicialización de semáforos y mutex
    sem_init(&hydrogen_sem, 0, 0);
    sem_init(&oxygen_sem, 0, 0);
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&condition, nullptr);

    pthread_t thread;
    char input;

    cout << "Enter 'H' for Hydrogen, 'O' for Oxygen, or 'E' to Exit:" << endl;

    while (true) {
        cin >> input;
        if (input == 'H' || input == 'h') {
            // Crear hilo para hidrógeno
            pthread_create(&thread, nullptr, hydrogen, nullptr);
            pthread_detach(thread);
        } else if (input == 'O' || input == 'o') {
            // Crear hilo para oxígeno
            pthread_create(&thread, nullptr, oxygen, nullptr);
            pthread_detach(thread);
        } else if (input == 'E' || input == 'e') {
            // Salir del programa
            cout << "Exiting..." << endl;
            break;
        } else {
            cout << "Invalid input. Enter 'H', 'O', or 'E'." << endl;
        }
    }

    // Limpieza de semáforos y mutex
    sem_destroy(&hydrogen_sem);
    sem_destroy(&oxygen_sem);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return 0;
}
