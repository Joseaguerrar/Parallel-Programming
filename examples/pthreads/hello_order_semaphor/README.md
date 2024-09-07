Semáforos
La definición original de semáforo por Dijkstra puede variar ligeramente de algunas implementaciones. Para Dijkstra un semáforo es un entero con signo, con tres características:

Cuando se crea un semáforo, éste se inicializa con un entero cualquiera (negativo, cero, o positivo). Pero después de inicializado las únicas dos operaciones que están permitidas es incrementar en uno (signal) y decrementar en uno (wait) al semáforo. No se puede leer el valor actual del semáforo.

Cuando un hilo decrementa un semáforo, si el resultado es negativo, el hilo es bloqueado y no puede continuar hasta que otro hilo incremente el semáforo.

Cuando un hilo incrementa un semáforo, si hay otros threads esperando, uno de ellos será desbloqueado. Tanto el hilo que incrementa el semáforo como el que fue desbloqueado siguen ejecutándose concurrentemente. Si hay varios hilos esperando, no hay forma de saber cuál de ellos será el desbloqueado por el scheduler del sistema operativo. El programador no tiene forma de saber si al incrementar un semáforo, se desbloqueará o no un hilo en espera, dado que no se puede leer el valor actual del semáforo por la regla 1.

El valor actual de un semáforo indica lo siguiente. Si el valor es positivo indica la cantidad de hilos que pueden decrementar el semáforo sin bloquearse. Si es negativo indica la cantidad de hilos que están bloqueados esperando actualmente por el semáforo. Si el valor es cero, indica que no hay hilos esperando por el semáforo, pero si un thread trata de decrementarlo, será bloqueado.

Algunas implementaciones, por ejemplo POSIX, permiten probar la espera (sem_trywait). Esta función nunca bloquea al hilo que la invoca. Si se trata de esperar un semáforo que tiene un valor positivo, se decrementará el semáforo y el hilo continuará ejecutándose como una invocación normal a wait(). Pero si se trata de esperar por un semáforo que quedaría en un valor negativo, la función sem_trywait() no decrementa al semáforo ni bloquea al hilo, sino que retorna de inmediato indicando un código de error (por ejemplo, -1). Dado que el hilo mantiene su ejecución, quien programa puede decidir condicionando (if) el valor de retorno del sem_trywait() y tomar distintas acciones que realice el hilo cuando se pudo o no esperar por el semáforo.

En los siguientes ejemplos se seguirá la definición original de Dijkstra, que no permite probar la espera. Se usará pseudocódigo con la siguiente sintaxis para los hilos y semáforos, con el fin de centrar la atención en estos temas y no en detalles de implementación de una tecnología particular (ej.: Pthreads):

sem := create_semaphore(3) // Create a semaphore with initial value 3
signal(sem)                // Increment and wake a waiting thread if any
wait(sem)                  // Decrement and block if the result is negative