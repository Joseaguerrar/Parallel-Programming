#include <iostream>
#include <pthread.h>
#include <queue>
#include <unistd.h>
#include <cctype>

using namespace std;

struct DanceFloor {
    pthread_mutex_t mtx;
    pthread_cond_t cv_team;
    int male_count;
    int female_count;
    bool exit_flag;

    DanceFloor() : male_count(0), female_count(0), exit_flag(false) {
        pthread_mutex_init(&mtx, nullptr);
        pthread_cond_init(&cv_team, nullptr);
    }

    ~DanceFloor() {
        pthread_mutex_destroy(&mtx);
        pthread_cond_destroy(&cv_team);
    }
};

void dance() {
    sleep(1);
    cout << "A team of 2 males and 2 females is dancing!" << endl;
}

void* male(void* arg) {
    auto* floor = static_cast<DanceFloor*>(arg);

    pthread_mutex_lock(&floor->mtx);
    floor->male_count++;

    while ((floor->male_count < 2 || floor->female_count < 2) && !floor->exit_flag) {
        pthread_cond_wait(&floor->cv_team, &floor->mtx);
    }

    if (floor->exit_flag) {  // Verifica si se dio la señal de parada
        pthread_mutex_unlock(&floor->mtx);
        return nullptr;
    }

    // Formar equipo
    floor->male_count -= 2; // Dos hombres por equipo
    floor->female_count -= 2; // Dos mujeres por equipo
    pthread_cond_broadcast(&floor->cv_team);

    pthread_mutex_unlock(&floor->mtx);

    dance();
    return nullptr;
}

void* female(void* arg) {
    auto* floor = static_cast<DanceFloor*>(arg);

    pthread_mutex_lock(&floor->mtx);
    floor->female_count++;

    while ((floor->male_count < 2 || floor->female_count < 2) && !floor->exit_flag) {
        pthread_cond_wait(&floor->cv_team, &floor->mtx);
    }

    if (floor->exit_flag) {  // Verifica si se dio la señal de parada
        pthread_mutex_unlock(&floor->mtx);
        return nullptr;
    }

    // Formar equipo
    floor->male_count -= 2; // Dos hombres por equipo
    floor->female_count -= 2; // Dos mujeres por equipo
    pthread_cond_broadcast(&floor->cv_team);

    pthread_mutex_unlock(&floor->mtx);

    dance();
    return nullptr;
}

int main() {
    DanceFloor floor;
    queue<pthread_t> threads;
    char dancer;

    cout << "Enter dancers (M for male, W for female, E to exit): " << endl;

    while (true) {
        cin >> dancer;
        dancer = toupper(dancer);

        if (dancer == 'E') {
            pthread_mutex_lock(&floor.mtx);
            floor.exit_flag = true;
            pthread_cond_broadcast(&floor.cv_team);  // Despierta a todos los hilos
            pthread_mutex_unlock(&floor.mtx);
            cout << "Exiting..." << endl;
            break;
        }

        pthread_t tid;
        if (dancer == 'M') {
            if (pthread_create(&tid, nullptr, male, &floor) != 0) {
                cerr << "Error creating male thread." << endl;
                return 1;
            }
        } else if (dancer == 'W') {
            if (pthread_create(&tid, nullptr, female, &floor) != 0) {
                cerr << "Error creating female thread." << endl;
                return 1;
            }
        } else {
            cerr << "Invalid input. Enter M, W, or E." << endl;
            continue;
        }

        threads.push(tid);
    }

    while (!threads.empty()) {
        pthread_join(threads.front(), nullptr);
        threads.pop();
    }

    return 0;
}
