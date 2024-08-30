El procedimiento create_threads() crea un equipo de hilos, todos ejecutando el procedimiento routine() concurrentemente, y retorna un arreglo de valores que representan hilos de ejecución. De forma análoga, el procedimiento join_threads() recibe el arreglo de hilos y espera a que todos ellos finalicen su ejecución.

Contar con estos dos procedimientos reutilizables es sumamente útil para la implementación de equipos de hilos en el paralelismo de datos.
Modifique la subrutina create_threads() para que cree en la memoria dinámica e inicialice un arreglo de registros de datos privados. Cada registro tiene cuatro valores: el número de hilo (obtenido del índice de un ciclo), la cantidad de hilos en el equipo de hilos, el puntero a los datos compartidos (data, recibido por parámetro), y el identificador del hilo (pthread_t) inicializado con pthread_create(). Envíe la dirección del registro privado a cada hilo al crearlo con pthread_create(). Finalmente create_threads() retorna la dirección de este arreglo de registros privados en lugar del arreglo de identificadores de hilos (pthread_t).

Modifique a join_threads() para que reciba la cantidad de hilos y la dirección al arreglo de registros privados (en lugar del arreglo de identificadores de hilos). Luego esperará por cada hilo con pthread_join() usando el identificador del hilo en los datos privados. Finalmente libera el arreglo de datos privados.

Con estos cambios, los hilos podrán saber su número de hilo (thread_number) y la cantidad de hilos que hay en el equipo (thread_count) accediendo a los campos del registro privado.

Aplique estas subrutinas a un ejemplo o ejercicio que utilice equipos de hilos, como Ejemplo 2 o [team_shot_put].