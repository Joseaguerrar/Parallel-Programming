# Ejercicio de Comunicación Circular en MPI

Este ejercicio implementa una comunicación en anillo usando MPI (Message Passing Interface) donde varios procesos colaboran para calcular una suma acumulativa. Cada proceso genera un número aleatorio y lo añade a una suma recibida de su proceso anterior en el anillo, enviando el resultado al siguiente proceso. Al finalizar, cada proceso imprime su número aleatorio y la suma acumulada, junto con su identificador y el nombre de la máquina en la que se ejecuta.

## Descripción del Ejercicio

El programa realiza los siguientes pasos:
1. Inicializa el entorno MPI.
2. Cada proceso genera un número aleatorio entre 0 y 100.
3. La comunicación se organiza en un anillo, donde:
   - El proceso 0 inicia la suma con su número aleatorio.
   - Cada proceso recibe una suma acumulada, añade su número aleatorio, y envía el nuevo total al siguiente proceso en el anillo.
   - Este proceso se repite hasta que todos los procesos hayan recibido, actualizado y enviado la suma acumulada.
4. Cada proceso imprime su identificador, su número aleatorio, la suma acumulada y el nombre del host al finalizar.

### Requisitos

- MPI debe estar instalado en el sistema.
- Una compilación y entorno de ejecución de C++ compatibles con MPI (por ejemplo, `mpic++` y `mpiexec`).

## Compilación

Para compilar el programa:
```bash
make

Para ejecutar el programa:
mpiexec -n 4 ./bin/ring
