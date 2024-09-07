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

int queue_enqueue(queue_t* queue, const size_t data) {
  assert(queue);
  int error = EXIT_SUCCESS;

  queue_node_t* new_node = (queue_node_t*) calloc(1, sizeof(queue_node_t));
  if (new_node) {
    new_node->data = data;

    pthread_mutex_lock(&queue->can_access_queue);
    if (queue->tail) {
      queue->tail = queue->tail->next = new_node;
    } else {
      queue->head = queue->tail = new_node;
    }
    pthread_mutex_unlock(&queue->can_access_queue);
  } else {
    error = EXIT_FAILURE;
  }

  return error;
}

int queue_dequeue(queue_t* queue, size_t* data) {
  assert(queue);
  int error = 0;

  pthread_mutex_lock(&queue->can_access_queue);
  if (!queue_is_empty_unsafe(queue)) {
    if (data) {
      *data = queue->head->data;
    }
    queue_remove_first_unsafe(queue);
  } else {
    error = EXIT_FAILURE;
  }
  pthread_mutex_unlock(&queue->can_access_queue);

  return error;
}

void queue_remove_first_unsafe(queue_t* queue) {
  assert(queue);
  assert(!queue_is_empty_unsafe(queue));
  queue_node_t* node = queue->head;
  queue->head = queue->head->next;
  free(node);
  if (queue->head == NULL) {
    queue->tail = NULL;
  }
}

void queue_clear(queue_t* queue) {
  assert(queue);
  pthread_mutex_lock(&queue->can_access_queue);
  while (!queue_is_empty_unsafe(queue)) {
    queue_remove_first_unsafe(queue);
  }
  pthread_mutex_unlock(&queue->can_access_queue);
}