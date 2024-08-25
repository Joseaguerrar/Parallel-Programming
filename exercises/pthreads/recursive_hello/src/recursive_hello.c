// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>

/**
 * @brief ...
 */
void* greet(void* data);

// procedure main:
int main(void) {
    size_t number=2;
  // create_thread(greet)
  pthread_t thread;
  int error = pthread_create(&thread, /*attr*/ NULL, greet, &number);
  if (error == EXIT_SUCCESS) {
    // print "Hello from main thread"
    // usleep(1);  // indeterminism
    printf("Hello from main thread\n");
    pthread_join(thread, /*value_ptr*/ NULL);
  } else {
    fprintf(stderr, "Error: could not create secondary thread\n");
  }
  return error;
}  // end procedure

// procedure greet:
void* greet(void* numero_recibido) {
  size_t number= (size_t)numero_recibido;
  if (number==0)
  {
    printf("\nGoodbye from thread: %lu\n",number);
  } else if (number>0){
    printf("\nHello from thread: %lu\n",number);
        pthread_t new_thread;
        size_t new_number=number-1;
        int error=pthread_create(&new_thread,/*attr*/NULL,greet,&new_number);
        if (error==EXIT_SUCCESS)
        {
            //Esperar que el nuevo hilo termine su ejecución
            pthread_join(new_thread,NULL);
        }else
        {
            fprintf(stderr, "Error: could not create thread\n");
        }
        return NULL;
        //Fin del procedimiento 
  }
  return NULL;
}  // end procedure