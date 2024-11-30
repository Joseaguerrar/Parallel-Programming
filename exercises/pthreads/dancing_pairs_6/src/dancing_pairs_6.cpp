#include <iostream>
#include <pthread.h>
#include <queue>
#include <unistd.h>
#include <cctype>

using namespace std;

// Estructura para datos compartidos
struct DanceFloor {
    pthread_mutex_t mtx;
    pthread_cond_t cv_male;
    pthread_cond_t cv_female;
    pthread_cond_t cv_team;
    int male_count;
    int female_count;
    int ready_to_dance;

    DanceFloor() : male_count(0), female_count(0), ready_to_dance(0) {
        pthread_mutex_init(&mtx, nullptr);
        pthread_cond_init(&cv_male, nullptr);
        pthread_cond_init(&cv_female, nullptr);
        pthread_cond_init(&cv_team, nullptr);
    }

    ~DanceFloor() {
        pthread_mutex_destroy(&mtx);
        pthread_cond_destroy(&cv_male);
        pthread_cond_destroy(&cv_female);
        pthread_cond_destroy(&cv_team);
    }
};

void dance() {
    sleep(1); // Simula el baile
    cout << "A team of 2 males and 2 females is dancing!" << endl;
}

void* male(void* arg) {
    auto* floor = static_cast<DanceFloor*>(arg);

    pthread_mutex_lock(&floor->mtx);
    floor->male_count++;
    pthread_cond_broadcast(&floor->cv_team);

    while (floor->male_count < 2 || floor->female_count < 2) {
        pthread_cond_wait(&floor->cv_male, &floor->mtx);
    }

    if (floor->ready_to_dance < 4) {
        floor->ready_to_dance++;
        floor->male_count--;
    }

    if (floor->ready_to_dance == 4) {
        floor->ready_to_dance = 0;
        pthread_cond_broadcast(&floor->cv_team);
    }

    pthread_mutex_unlock(&floor->mtx);
    dance();
    return nullptr;
}

void* female(void* arg) {
    auto* floor = static_cast<DanceFloor*>(arg);

    pthread_mutex_lock(&floor->mtx);
    floor->female_count++;
    pthread_cond_broadcast(&floor->cv_team);

    while (floor->male_count < 2 || floor->female_count < 2) {
        pthread_cond_wait(&floor->cv_female, &floor->mtx);
    }

    if (floor->ready_to_dance < 4) {
        floor->ready_to_dance++;
        floor->female_count--;
    }

    if (floor->ready_to_dance == 4) {
        floor->ready_to_dance = 0;
        pthread_cond_broadcast(&floor->cv_team);
    }

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
        } else if (dancer == 'E') {
            cout << "Exiting..." << endl;
            break;
        } else {
            cerr << "Invalid input. Enter M, W, or E." << endl;
            continue;
        }

        threads.push(tid);
    }

    // Esperar a que todos los hilos terminen
    while (!threads.empty()) {
        if (pthread_join(threads.front(), nullptr) != 0) {
            cerr << "Error joining thread." << endl;
            return 1;
        }
        threads.pop();
    }

    return 0;
}
