# Simulación de Transferencia de Calor en una Lámina Rectangular

## Descripción

Este proyecto implementa una simulación de transferencia de calor en una lámina rectangular de material homogéneo. La lámina está dividida en una matriz de celdas, donde cada celda representa una porción del material con una temperatura específica. La simulación calcula cómo la temperatura de cada celda cambia con el tiempo debido a la transferencia de calor entre las celdas vecinas.

La simulación busca alcanzar un equilibrio térmico, es decir, un estado en el que las temperaturas de todas las celdas internas se estabilizan y no cambian significativamente con el tiempo.

### Contexto

En un entorno experimental, se inyecta calor de manera constante a través de los bordes de la lámina. El propósito de la simulación es predecir el comportamiento térmico de la lámina hasta que se alcance el equilibrio térmico.

### Modelo de Transferencia de Calor

La simulación está basada en un modelo físico que considera la conductividad térmica del material, actualizando las temperaturas de las celdas en función de las celdas vecinas. Se toma en cuenta la difusividad térmica y el tiempo que tarda el calor en difundirse.

### Objetivo

El objetivo es continuar actualizando las temperaturas de las celdas hasta que todas las celdas internas presenten cambios de temperatura menores a un umbral (epsilon), indicando que el equilibrio térmico ha sido alcanzado.

## Características del Programa

- **Flexibilidad**: Permite trabajar con láminas de diferentes tamaños y materiales, con parámetros de entrada configurables.
- **Resultados**: Genera un reporte detallado con el número de estados hasta alcanzar el equilibrio y el tiempo transcurrido.

## Implementación Concurrente

El programa incluye una versión concurrente que distribuye la carga de trabajo entre múltiples hilos de ejecución, mejorando el desempeño en sistemas con múltiples núcleos de CPU.

- **Uso de Hilos**: La versión concurrente divide el procesamiento entre varios hilos para mejorar la eficiencia.
- **Argumento Opcional**: Se puede especificar el número de hilos a utilizar como argumento en la línea de comandos. Si no se proporciona este argumento, el programa utilizará el número de núcleos disponibles en la máquina.

El programa concurrente recibe los mismos datos que la versión serial y produce los mismos resultados. La diferencia es que el programa concurrente realiza el procesamiento de forma paralela para mejorar el rendimiento.

### Requerimientos Concurrentes

- El programa concurrente debe permitir que el usuario lo invoque con un número que indique la cantidad de hilos de ejecución. Si este número no se proporciona, se debe suponer que se utilizará el número de núcleos disponibles en la máquina.
- El programa debe imprimir los resultados en el mismo orden y formato que la versión serial y pasar los mismos casos de prueba. 
- El uso de hilos debe proporcionar un aumento de rendimiento verificable.

## Uso del Programa

Para compilar el programa, use el siguiente comando:

1. "make clean"
2. "make"

Para ejecutar la simulación, use el siguiente comando:

1. "./bin/heatsim-pthread tests/job001 job001.txt 5" esto indica lo siguiente:  <ejecutable> <carpeta_del_trabajo> <trabajo_a_analizar> <cantidad_de_hilos>

2. Aclarar que si no se especifica la cantidad de hilos el sistema usa los máximos posibles por default

### Ideas de Mejoras para entrega 3:

1. Tratar de distribuir más equitativamente o de una manera más óptima las filas por hilos
2. Hacer copias de la matriz para evitar el uso de mutex
3. Solo crear los hilos una vez por simulación, no cada vez que se evalúa la placa
4. Sumar los estados después de que los hilos terminen toda la placa