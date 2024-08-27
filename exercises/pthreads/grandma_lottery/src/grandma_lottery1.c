#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

void* buy_lottery(void*);

int main(){





    return EXIT_SUCCESS;
}



void* buy_lottery(void* arg){
    int* num_lottery= malloc(sizeof(int));
    //reservamos espacio en memoria para el entero
    if(num_lottery==NULL){
        //si es nulo, hubo error
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
        //devolvemos salida erronea
    }
    *num_lottery=rand_r((unsigned int*)arg)%100;
    //le decimos que el valor que va a guardar es un random positivo de 0 a 99
    return (void*)num_lottery;
    //lo retornamos tratandolo como void*
}