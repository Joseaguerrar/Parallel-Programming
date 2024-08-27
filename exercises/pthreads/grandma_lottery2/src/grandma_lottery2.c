#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>  // Necesario para uintptr_t

void* buy_lottery(void*);

int main() {
    pthread_t nieto1, nieto2;
    int semilla1 = time(NULL);
    int semilla2 = time(NULL) + 1;

    // Creación de los hilos representando los nietos
    pthread_create(&nieto1, NULL, buy_lottery, &semilla1);
    pthread_create(&nieto2, NULL, buy_lottery, &semilla2);

    int num1;
    int num2;
    //Espera a que regresen con el valor recuperado
    pthread_join(nieto1, (void**)&num1);
    pthread_join(nieto2, (void**)&num2);

    // Devolvemos los números
    printf("Número de lotería del nieto 1: %i\n", num1);
    printf("Número de lotería del nieto 2: %i\n", num2);

    return EXIT_SUCCESS;
}

void* buy_lottery(void* arg) {
    int num_lottery = rand_r((unsigned int*)arg) % 100;
    return (void*)(uintptr_t) num_lottery;
    /*El número de lotería se convierte a uintptr_t,
    un tipo de entero sin signo lo suficientemente grande para almacenar punteros sin pérdida de información.*/
}
