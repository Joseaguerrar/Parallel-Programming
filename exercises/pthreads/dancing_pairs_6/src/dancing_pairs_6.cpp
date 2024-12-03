// Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <queue>
#include <cctype>

using namespace std; //NOLINT

/**
 * @struct DanceFloor
 * @brief Representa el salón de baile compartido para gestionar la sincronización de los bailarines.
 */
struct DanceFloor {
    pthread_mutex_t mtx;      ///< Mutex para proteger los recursos compartidos.
    pthread_cond_t cv_team;      ///< Variable para sincronizar equipos.
    int male_count;              ///< Número de hombres actualmente esperando.
    int female_count;            ///< Número de mujeres actualmente esperando.
    bool exit_flag;              ///< Bandera para indicar la terminación.

    /**
     * @brief Inicializa el DanceFloor con valores por defecto.
     * Se inicializan el mutex y la variable de condición.
     */
    DanceFloor() : male_count(0), female_count(0), exit_flag(false) {
        pthread_mutex_init(&mtx, nullptr);
        pthread_cond_init(&cv_team, nullptr);
    }

    /**
     * @brief Destruye el DanceFloor.
     * Limpia el mutex y la variable de condición.
     */
    ~DanceFloor() {
        pthread_mutex_destroy(&mtx);
        pthread_cond_destroy(&cv_team);
    }
};

/**
 * @brief Simula la acción de bailar.
 * Muestra un mensaje indicando que un equipo está bailando.
 */
void dance() {
    sleep(1);  // Simula el tiempo necesario para bailar.
    cout << "¡Un equipo de 2 hombres y 2 mujeres está bailando!" << endl;
}

/**
 * @brief Función de hilo para un bailarín hombre.
 * @param arg Puntero a la estructura compartida DanceFloor.
 * Incrementa el contador de hombres, espera a que se forme un equipo y baila cuando las condiciones se cumplen.
 */
void* male(void* arg) {
    auto* floor = static_cast<DanceFloor*>(arg);

    pthread_mutex_lock(&floor->mtx);
    floor->male_count++;

    while ((floor->male_count < 2 || floor->female_count < 2) &&
                                                            !floor->exit_flag) {
        pthread_cond_wait(&floor->cv_team, &floor->mtx);
    }

    if (floor->exit_flag) {  // Verifica si se activó la señal de salida
        pthread_mutex_unlock(&floor->mtx);
        return nullptr;
    }

    // Forma un equipo
    floor->male_count -= 2;  // Dos hombres por equipo
    floor->female_count -= 2;  // Dos mujeres por equipo
    pthread_cond_broadcast(&floor->cv_team);

    pthread_mutex_unlock(&floor->mtx);

    dance();
    return nullptr;
}

/**
 * @brief Función de hilo para una bailarina mujer.
 * @param arg Puntero a la estructura compartida DanceFloor.
 * Incrementa el contador de mujeres, espera a que se forme un equipo y baila cuando las condiciones se cumplen.
 */
void* female(void* arg) {
    auto* floor = static_cast<DanceFloor*>(arg);

    pthread_mutex_lock(&floor->mtx);
    floor->female_count++;

    while ((floor->male_count < 2 || floor->female_count < 2) &&
                                                            !floor->exit_flag) {
        pthread_cond_wait(&floor->cv_team, &floor->mtx);
    }

    if (floor->exit_flag) {  // Verifica si se activó la señal de salida
        pthread_mutex_unlock(&floor->mtx);
        return nullptr;
    }

    // Forma un equipo
    floor->male_count -= 2;  // Dos hombres por equipo
    floor->female_count -= 2;  // Dos mujeres por equipo
    pthread_cond_broadcast(&floor->cv_team);

    pthread_mutex_unlock(&floor->mtx);

    dance();
    return nullptr;
}

/**
 * @brief Función principal para gestionar los bailarines y la sincronización.
 * Lee la entrada, crea hilos para los bailarines y gestiona la terminación.
 * @return 0 en caso de ejecución exitosa, 1 si falla la creación de algún hilo.
 */
int main() {
    DanceFloor floor;
    ///< Instancia del salón de baile compartido.
    queue<pthread_t> threads;
    ///< Cola para gestionar las referencias a los hilos.
    char dancer;

    cout <<
     "Ingrese los bailarines (M para hombre, W para mujer, E para salir): " <<
      endl;

    while (true) {
        cin >> dancer;
        dancer = toupper(dancer);

        if (dancer == 'E') {
            pthread_mutex_lock(&floor.mtx);
            floor.exit_flag = true;
            pthread_cond_broadcast(&floor.cv_team);
            // Despierta a todos los hilos
            pthread_mutex_unlock(&floor.mtx);
            cout << "Saliendo..." << endl;
            break;
        }

        pthread_t tid;
        if (dancer == 'M') {
            if (pthread_create(&tid, nullptr, male, &floor) != 0) {
                cerr << "Error al crear el hilo para hombre." << endl;
                return 1;
            }
        } else if (dancer == 'W') {
            if (pthread_create(&tid, nullptr, female, &floor) != 0) {
                cerr << "Error al crear el hilo para mujer." << endl;
                return 1;
            }
        } else {
            cerr << "Entrada inválida. Ingrese M, W o E." << endl;
            continue;
        }

        threads.push(tid);
    }

    // Unir todos los hilos
    while (!threads.empty()) {
        pthread_join(threads.front(), nullptr);
        threads.pop();
    }

    return 0;
}
