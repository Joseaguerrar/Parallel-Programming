##
Ejemplo 5. Hola mundo ordenado con espera activa [hello_order_busywait]
Haga que los threads saluden siempre en orden. Es decir, si se crean w threads, la salida sea siempre en orden

Hello from thread 0 of w
Hello from thread 1 of w
Hello from thread 2 of w
...
Hello from thread w of w
Utilice espera activa como mecanismo de sincronización (control de concurrencia).

Ejecute varias veces su solución de hello_order_busywait con la cantidad máxima de threads que el su sistema operativo le permite. ¿Es consistente la duración de su solución con espera activa?
#
R/ La duración se ejecuta de manera inconsistente, y lo probamos con 16 hilos

Describa el comportamiento del programa y explique la razón de este comportamiento en el archivo readme.md dentro de su carpeta hello_order_busywait. Indique en qué condiciones sería apremiante utilizar espera activa, o en caso de no haberlas, sugiera soluciones alternativas.
#
R/ No es consistente ya que el hilo que se está ejecutando actualmente debe comprobrar activamente si se cumple la condición y por lo tanto fuerza a la CPU a que gaste recursos si no se maneja. Algunas alternativas puede ser poner un tiempo de espera conocido como (sleep), que ponga el hilo en espera/pausa y no gaste recursos de manera activa...

La espera activa (busy waiting) es un ciclo que hace a un hilo de ejecución esperar repetitivamente hasta que una condición se haga falsa. Por ejemplo, si la variable next_thread indica el número del próximo thread que puede realizar una tarea que debe ser serializada en orden, el código

// Wait until it is my turn
while (next_thread < my_thread_id) {
  // busy-wait
}

// Do the ordered-task here
task();

// Allow subsequent thread to do the task
++next_thread;

###
Espera activa con retraso. ¿Se corrige el problema de la espera activa si en lugar de tener un ciclo vacío, se espera un cierto tiempo? 
 Copie su carpeta ejemplos/pthreads/hello_order_busywait a ejercicios/pthreads/delayed_busy_wait. Permita que el usuario pueda invocar su programa dos argumentos de línea de comandos: la cantidad de hilos a crear, y la cantidad de microsegundos a esperar cuando no es el turno del hilo de ejecución.

Espera activa con retraso constante. Si no es el turno de un hilo, éste espera una cantidad constante de microsegundos, algo como:

// Constant delayed busy waiting: wait until it is my turn
while (next_thread < my_thread_id) {
  usleep(delay);
}
Recuerde probar la calidad de su código (sanitizers, linter). Luego ejecute al menos tres veces su solución (sin sanitizers) con la cantidad máxima de hilos de ejecución que su sistema operativo le permite crear y una espera de 50µs. Escriba en el readme.md del ejercicio la mayor de las duraciones que obtuvo de sus corridas. ¿Hubo una mejora de la espera constante respecto a la espera activa pura?
#
R/En nuestro caso no se corrigió el problema, sigue inconsistente. Execution time: 0.001285062s

Espera activa con retraso pseudoaleatorio. Altere su solución al ejercicio para que en lugar de esperar exactamente la cantidad de microsegundos indicada por el usuario, espere por una cantidad pseudoaleatoria de microsegundos cuyo límite es el número indicado por el usuario en el segundo argumento de línea de comandos. Sugerencia: puede usar compilación condicional para implementar esta variación. La espera varía en cada iteración del ciclo de espera activa, algo como:

// Random delayed busy waiting: wait until it is my turn
while (next_thread < my_thread_id) {
	const unsigned my_delay = rand_r(&my_seed) % max_delay;
  usleep(my_delay);
}
Ejecute al menos tres veces su solución pseudoaleatoria con la cantidad máxima de hilos y un máximo de espera de 50µs. Tome la mayor de las duraciones. ¿Hubo una mejora de la espera pseudoaleatoria respecto a la espera constante?

Comparación de las esperas. ¿Mejora el tiempo de ejecución de los dos tipos de esperas (constante y pseudoaleatoria) si disminuye o incrementa la espera máxima de los hilos? Haga al menos un par de ejecuciones con al menos los siguientes tiempos de espera:

1µs

5µs

25µs

50µs

75µs

100µs

Sugerencia: si hay varios compañeros(as) trabajando el mismo ejercicio en el laboratorio, escojan tiempos diferentes y compartan los resultados. Pueden crear una gráfica en un documento compartido. Agreguen más tiempos de ejecución si hay más de 6 integrantes.

Cree una gráfica donde el eje-x son las duraciones dadas por argumento al programa. El eje-y son los tiempos de ejecución de los programas. El gráfico tendrá dos series, una para la espera constante y otra para la espera pseudoaleatoria.

Agregue la gráfica al readme.md del ejercicio y una discusión de a lo sumo dos párrafos. Indique cuál fue el mejor tiempo máximo de espera obtenido y los efectos de disminuir o incrementarlo. Conjeture y trate de explicar por qué ocurre este comportamiento. Finalmente indique si la espera activa con retraso es una solución óptima, y en caso negativo, provea una idea que pueda lograr este ideal.
