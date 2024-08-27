// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// thread_private_data_t
typedef struct private_data {
  uint64_t thread_number;  // rank
  uint64_t thread_count;
  struct private_data* next;
} private_data_t;
//funciona como lista enlazada

/**
 * @brief ...
 */
void* greet(void* data);
int create_threads(uint64_t thread_count);

// procedure main(argc, argv[])
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  // create thread_count as result of converting argv[1] to integer
  // thread_count := integer(argv[1])
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  if (argc == 2) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) == 1) {
    } else {
      fprintf(stderr, "Error: invalid thread count\n");
      return 11;
    }
  }

  error = create_threads(thread_count);
  return error;
}  // end procedure


int create_threads(uint64_t thread_count) {
  int error = EXIT_SUCCESS;
  // for thread_number := 0 to thread_count do
  pthread_t* threads = (pthread_t*) malloc(thread_count * sizeof(pthread_t));//esta es para los hilos
  //Solicitamos la cantidad de espacio para un pthread por cada uno que solicito y se castea a un puntero tipo thread porque no nos sirve void*
  private_data_t* private_data = (private_data_t*)//esta es para el struct que almacena cada hilo
    calloc(thread_count, sizeof(private_data_t));
            //cantidad  *  //tamaño

    //calloc reserva la memoria y la devuelve inicializada con 0 malloc con "basura"
  if (threads && private_data) {//si están inicializados a algo que no sea NULL(0) ejecute
    for (uint64_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].thread_count = thread_count;
      // create_thread(greet, thread_number)
      error = pthread_create(&threads[thread_number], /*attr*/ NULL, greet
                            //dirección de memoria donde voy a guardar ese hilo

        , /*arg*/ &private_data[thread_number]);
                    //le mandamos los argumentos y atributos como estructura
      if (error == EXIT_SUCCESS) {
      } else {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;
        break;
      }
    }

    // print "Hello from main thread"
    printf("Hello from main thread\n");

    for (uint64_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      pthread_join(threads[thread_number], /*value_ptr*/ NULL);
      //Por cada thread creado hay que esperar a que "muera"
    }
    //Liberamos la memoria que pedimos
    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n"
      , thread_count);
    error = 22;
  }

  return error;
}

// procedure greet:
//siempre se recibe un puntero porque los hilos reciben un puntero puro es decir a void, cuando los creo, por eso se pone &
void* greet(void* data) {
  // assert(data);
  private_data_t* private_data = (private_data_t*) data;
  //Casteamos al tipo de dato, que en este caso es a la estructura
  // print "Hello from secondary thread"
  printf("Hello from secondary thread %" PRIu64 " of %" PRIu64 "\n"
                                        //formato de uint de 64 bits, el pr es necesario por el formato de printf
    , (*private_data).thread_number, private_data->thread_count);
       //el primer argumento         //el segundo argumento    //estas son las dos maneras de desreferenciar
  return NULL;
}  // end procedure