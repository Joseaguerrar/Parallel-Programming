#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

void* buy_lottery(void*);

int main(){
    pthread_t nieto1,nieto2;
    int semilla1=1, semilla2=2;

    //creación de los hilos representando los nietos
    pthread_create(&nieto1,NULL,buy_lottery,&semilla1);
    pthread_create(&nieto2,NULL,buy_lottery,&semilla2);

    int* num1;
    int* num2;

    pthread_join(&nieto1,(void**)&num1);
    pthread_join(&nieto2,(void**)&num2);
    //Si no se hace el casting el compilador tira warning, además así almacenamos la dirección de los números

    //Devolvemos los números
    printf("Número de loteria del nieto 1: %i", *num1);
    printf("Número de loteria del nieto 2: %i", *num2);

    free(num1);
    free(num2);


    return EXIT_SUCCESS;
}



void* buy_lottery(void* arg){
    __uint16_t* num_lottery= malloc(sizeof(__uint16_t));
    //reservamos espacio en memoria para el entero
    if(num_lottery==NULL){
        //si es nulo, hubo error
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
        //devolvemos salida erronea
    }
    *num_lottery=rand_r(( __uint16_t*)arg)%100;
    //le decimos que el valor que va a guardar es un random positivo de 0 a 99

    /*rand_r es una versión de rand que es reentrante, lo cual es importante en un entorno multihilo. 
    Estás pasando un puntero al argumento arg, lo que permite que rand_r genere un número pseudoaleatorio
    basado en la semilla proporcionada. Definición obtenida de chatgpt para ver al diferencia entre rand y rand_r*/
    return (void*)num_lottery;
    //lo retornamos tratandolo como void*
}