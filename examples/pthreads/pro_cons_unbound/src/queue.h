// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4
// TODO(all): Implement a thread-safe queue

#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdbool.h>

/**
 * @brief Nodo de la cola.
 * 
 * Esta estructura representa un nodo en la cola. Cada nodo almacena un dato
 * y un puntero al siguiente nodo en la cola.
 */
typedef struct queue_node {
  size_t data;                
  // Dato almacenado en el nodo.

  struct queue_node* next;    
  // Puntero al siguiente nodo de la cola.
} queue_node_t;

/**
 * @brief Estructura de la cola.
 * 
 * Esta estructura representa una cola que es compartida entre múltiples hilos.
 * Contiene un mutex para garantizar el acceso seguro a la cola en un entorno 
 * concurrente.
 */
typedef struct {
  pthread_mutex_t can_access_queue;  
  // Mutex para controlar el acceso a la cola.

  queue_node_t* head;                
  // Puntero al primer nodo (cabeza) de la cola.

  queue_node_t* tail;                
  // Puntero al último nodo (cola) de la cola.
} queue_t;

/**
 * @brief Inicializa una cola.
 * 
 * Esta función inicializa una cola, incluyendo la creación de un mutex para el control
 * de acceso concurrente. Deja la cola vacía (sin nodos).
 * 
 * @param queue Puntero a la estructura de la cola que se va a inicializar.
 * @return int Devuelve 0 si la inicialización es exitosa, o un código de error si falla.
 * @remarks Esta subrutina NO es segura para hilos (thread-unsafe).
 */
int queue_init(queue_t* queue);

/**
 * @brief Destruye una cola.
 * 
 * Esta función destruye una cola, limpiando todos sus nodos y liberando los recursos
 * asociados al mutex.
 * 
 * @param queue Puntero a la estructura de la cola que se va a destruir.
 * @return int Devuelve 0 si la destrucción es exitosa, o un código de error si falla.
 */
int queue_destroy(queue_t* queue);

/**
 * @brief Verifica si la cola está vacía.
 * 
 * Esta función comprueba si la cola está vacía, utilizando el mutex para asegurar
 * el acceso seguro en un entorno concurrente.
 * 
 * @param queue Puntero a la estructura de la cola que se va a verificar.
 * @return bool Devuelve `true` si la cola está vacía, `false` en caso contrario.
 * @remarks Esta subrutina es segura para hilos (thread-safe).
 */
bool queue_is_empty(queue_t* queue);

/**
 * @brief Añade un dato a la cola.
 * 
 * Esta función encola un nuevo dato al final de la cola, actualizando los punteros 
 * de cabeza y cola según sea necesario. El acceso a la cola está protegido por un mutex.
 * 
 * @param queue Puntero a la estructura de la cola donde se va a encolar el dato.
 * @param data Dato que se va a añadir a la cola.
 * @return int Devuelve 0 si la operación es exitosa, o un código de error si falla.
 */
int queue_enqueue(queue_t* queue, const size_t data);

/**
 * @brief Elimina un dato de la cola.
 * 
 * Esta función desencola un dato de la cabeza de la cola, devolviendo el valor 
 * extraído mediante el parámetro `data`. El acceso a la cola está protegido por un mutex.
 * 
 * @param queue Puntero a la estructura de la cola de donde se va a desencolar el dato.
 * @param data Puntero donde se almacenará el dato desencolado.
 * @return int Devuelve 0 si la operación es exitosa, o un código de error si falla.
 */
int queue_dequeue(queue_t* queue, size_t* data);

/**
 * @brief Limpia una cola.
 * 
 * Esta función vacía completamente la cola, eliminando todos sus nodos.
 * 
 * @param queue Puntero a la estructura de la cola que se va a limpiar.
 */
void queue_clear(queue_t* queue);
#endif  // QUEUE_H