Repositorio para ejemplos y tareas del curso Programación paralela y concurrente

Glossary:

1.Programación serial: es la programación en la que se ejecutan las instrucciones una a una, en el orden en que se encuentran escritas en el código fuente.

2.Programación concurrente: es la programación en la que se ejecutan varias instrucciones al mismo tiempo, pero no necesariamente en el mismo orden en que se encuentran escritas en el código fuente.

3.Programación paralela: es la programación en la que se ejecutan varias instrucciones al mismo tiempo, y en el mismo orden en que se encuentran escritas en el código fuente de usando múltiples procesadores o hilos de ejecución.

4.Concurrencia de tareas:

5.Paralelismo de datos:

6.Hilo de ejecución: un arreglo de valores que el SO carga en los registros del CPU y permiten ejecutar código en ella. Contiene el program counter, el stack pointer y los demas regisros que le permitiran ejecutar instrucciones que le corresponden.

7.Indeterminismo: varios hilos o procesos pueden ejecutarse simultáneamente y competir por recursos como memoria o acceso a archivos. Dependiendo del orden en que los hilos ejecutan sus instrucciones, el estado final del programa puede cambiar, lo que puede resultar en resultados inesperados o comportamientos incorrectos si no se gestionan adecuadamente las condiciones de carrera.

8.Memoria privada y compartida: 
  Privada: es la memoria que pertenece exclusivamente a un hilo o proceso particular. Ningún otro hilo o proceso tiene acceso a esta memoria, lo que significa que los datos almacenados en ella son privados y no pueden ser modificados ni leídos por otros hilos. 
  Compartida: es la memoria a la que pueden acceder y modificar múltiples hilos o procesos. Los datos en la memoria compartida son accesibles por todos los hilos, lo que facilita la comunicación y la colaboración entre ellos.

9.Espera activa: o "busy waiting", un hilo o proceso verifica repetidamente una condición para determinar si puede continuar con su ejecución. Durante la espera activa, el hilo no cede el control del procesador y sigue ocupando recursos del sistema, ejecutando un bucle que verifica la condición sin realizar ningún trabajo útil.

10.Condición de carrera: ocurre cuando dos o más hilos o procesos acceden a recursos compartidos (como variables o datos en memoria) y tratan de modificar esos recursos al mismo tiempo. La salida o el estado final del programa depende del orden en que se ejecuten las operaciones de los hilos, lo que puede llevar a resultados impredecibles y errores difíciles de depurar.

11.Control de concurrencia: conjunto de técnicas y mecanismos utilizados para coordinar la ejecución de múltiples hilos o procesos que comparten recursos. Para garantizar que los recursos compartidos, como memoria, archivos, o dispositivos, sean accedidos de manera segura y eficiente, evitando problemas como condiciones de carrera, bloqueos y estados inconsistentes.

12.Seguridad condicional: la garantía de que las condiciones necesarias para la correcta ejecución de hilos o procesos se mantengan durante el acceso a recursos compartidos. Es una propiedad que asegura que las operaciones realizadas por múltiples hilos no violen las restricciones lógicas o condiciones del sistema, evitando así estados inconsistentes y errores.

13.Exclusión mutua: principio que garantiza que solo un hilo o proceso pueda acceder a un recurso compartido o a una sección crítica de código en un momento dado. El objetivo de la exclusión mutua es prevenir condiciones de carrera, asegurando que las operaciones sobre los recursos compartidos se realicen de manera segura y coherente.

14.Semáforo: mecanismo de sincronización utilizado para controlar el acceso de múltiples hilos o procesos a recursos compartidos. Los semáforos ayudan a evitar condiciones de carrera y garantizan que los recursos críticos se utilicen de manera segura y eficiente.

15.Barrera:

16.Variable de condición:

17.Candado de lectura y escritura:

18.Descomposición:

19.Mapeo:

20.Incremento de velocidad:

21.Comunicación punto a punto entre procesos:

22.Comunicación colectiva entre procesos:

23.Reducción:

