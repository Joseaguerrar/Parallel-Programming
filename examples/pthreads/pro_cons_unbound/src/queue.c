// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#include <assert.h>
#include <stdlib.h>

#include "queue.h"

/**
 * @brief Inicializa una cola.
 * 
 * Esta función inicializa los componentes de una cola, incluyendo el mutex que asegura
 * el acceso concurrente. La cola comienza vacía (sin nodos en la cabeza ni en la cola).
 * 
 * @param queue Puntero a la estructura de la cola que se va a inicializar.
 * @return int Devuelve 0 si la inicialización es exitosa, o un código de error si falla.
 */
int queue_init(queue_t* queue) {
  assert(queue);
  int error = pthread_mutex_init(&queue->can_access_queue, /*attr*/ NULL);
  queue->head = NULL;
  queue->tail = NULL;
  return error;
}

/**
 * @brief Destruye una cola.
 * 
 * Esta función limpia el contenido de la cola y destruye el mutex utilizado para
 * la sincronización de acceso concurrente.
 * 
 * @param queue Puntero a la estructura de la cola que se va a destruir.
 * @return int Devuelve 0 si la destrucción es exitosa, o un código de error si falla.
 */
int queue_destroy(queue_t* queue) {
  queue_clear(queue);
  return pthread_mutex_destroy(&queue->can_access_queue);
}

/**
 * @brief Verifica si una cola está vacía de manera segura.
 * 
 * Esta función comprueba si la cola está vacía, utilizando un mutex para asegurar
 * que el acceso sea seguro en un entorno de múltiples hilos.
 * 
 * @param queue Puntero a la estructura de la cola que se va a verificar.
 * @return bool Devuelve `true` si la cola está vacía, `false` en caso contrario.
 */
bool queue_is_empty(queue_t* queue) {
  assert(queue);
  pthread_mutex_lock(&queue->can_access_queue);
  bool result = queue->head == NULL;
  pthread_mutex_unlock(&queue->can_access_queue);
  return result;
}

/**
 * @brief Verifica si una cola está vacía sin bloqueo (inseguro).
 * 
 * Esta función comprueba si la cola está vacía sin utilizar un mutex, lo que la hace
 * insegura para su uso en entornos concurrentes. Debe usarse solo cuando se asegura
 * que no hay concurrencia.
 * 
 * @param queue Puntero a la estructura de la cola que se va a verificar.
 * @return bool Devuelve `true` si la cola está vacía, `false` en caso contrario.
 */
bool queue_is_empty_unsafe(queue_t* queue) {
  assert(queue);
  return queue->head == NULL;
}

/**
 * @brief Añade un dato a la cola.
 * 
 * Esta función encola un nuevo nodo con el dato proporcionado al final de la cola. 
 * La operación es segura para hilos, utilizando un mutex para proteger el acceso a la cola.
 * 
 * @param queue Puntero a la estructura de la cola.
 * @param data Dato que se va a añadir a la cola.
 * @return int Devuelve EXIT_SUCCESS si la operación es exitosa, o EXIT_FAILURE si ocurre un error.
 */
int queue_enqueue(queue_t* queue, const size_t data) {
  assert(queue);
  int error = EXIT_SUCCESS;

  queue_node_t* new_node = (queue_node_t*) calloc(1, sizeof(queue_node_t));
  // Reserva memoria para un nuevo nodo.

  if (new_node) {
    new_node->data = data;
    // Asigna el dato al nuevo nodo.

    pthread_mutex_lock(&queue->can_access_queue);
    // Bloquea el acceso a la cola para evitar condiciones de carrera.

    if (queue->tail) {
      queue->tail = queue->tail->next = new_node;
      // Si la cola no está vacía, el nuevo nodo se convierte en el último.
    } else {
      queue->head = queue->tail = new_node;
      // Si la cola está vacía, el nuevo nodo es tanto la cabeza como la cola.
    }

    pthread_mutex_unlock(&queue->can_access_queue);
    // Desbloquea el acceso a la cola.
  } else {
    error = EXIT_FAILURE;
    // Si no se pudo asignar memoria para el nuevo nodo, devuelve un error.
  }

  return error;
}

/**
 * @brief Elimina un dato de la cola.
 * 
 * Esta función desencola el primer nodo de la cola, devolviendo su dato a través del
 * parámetro `data`. La operación es segura para hilos, utilizando un mutex para proteger el acceso.
 * 
 * @param queue Puntero a la estructura de la cola.
 * @param data Puntero donde se almacenará el dato desencolado.
 * @return int Devuelve EXIT_SUCCESS si la operación es exitosa, o EXIT_FAILURE si la cola está vacía.
 */
int queue_dequeue(queue_t* queue, size_t* data) {
  assert(queue);
  int error = 0;

  pthread_mutex_lock(&queue->can_access_queue);
  // Bloquea el acceso a la cola para evitar condiciones de carrera.

  if (!queue_is_empty_unsafe(queue)) {
    // Verifica si la cola no está vacía sin usar mutex (condición insegura).

    if (data) {
      *data = queue->head->data;
      // Almacena el dato desencolado en el puntero `data`.
    }
    queue_remove_first_unsafe(queue);
    // Elimina el primer nodo de la cola.
  } else {
    error = EXIT_FAILURE;
    // Devuelve un error si la cola está vacía.
  }

  pthread_mutex_unlock(&queue->can_access_queue);
  // Desbloquea el acceso a la cola.

  return error;
}

/**
 * @brief Elimina el primer nodo de la cola (sin bloqueo).
 * 
 * Esta función elimina el primer nodo de la cola sin usar mutex. Es insegura
 * para el uso concurrente, por lo que debe ser llamada solo cuando se asegure
 * que no hay acceso concurrente.
 * 
 * @param queue Puntero a la estructura de la cola.
 */
void queue_remove_first_unsafe(queue_t* queue) {
  assert(queue);
  assert(!queue_is_empty_unsafe(queue));
  // Verifica que la cola no esté vacía.

  queue_node_t* node = queue->head;
  queue->head = queue->head->next;
  // Mueve la cabeza al siguiente nodo.

  free(node);
  // Libera la memoria del nodo eliminado.

  if (queue->head == NULL) {
    queue->tail = NULL;
    // Si la cola está vacía, actualiza el puntero de la cola.
  }
}

/**
 * @brief Vacía completamente una cola.
 * 
 * Esta función elimina todos los nodos de la cola, limpiando su contenido.
 * La operación es segura para hilos, utilizando un mutex para proteger el acceso.
 * 
 * @param queue Puntero a la estructura de la cola que se va a limpiar.
 */
void queue_clear(queue_t* queue) {
  assert(queue);
  pthread_mutex_lock(&queue->can_access_queue);
  // Bloquea el acceso a la cola para evitar condiciones de carrera.

  while (!queue_is_empty_unsafe(queue)) {
    queue_remove_first_unsafe(queue);
    // Elimina todos los nodos de la cola uno por uno.
  }

  pthread_mutex_unlock(&queue->can_access_queue);
  // Desbloquea el acceso a la cola.
}
