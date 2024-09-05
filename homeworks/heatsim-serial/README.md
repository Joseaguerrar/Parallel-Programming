# Simulación de Transferencia de Calor en una Lámina Rectangular

## Descripción del Problema

Este proyecto tiene como objetivo desarrollar una simulación por computadora que modele la transferencia de calor en una lámina rectangular de material homogéneo. La simulación se basa en la subdivisión de la lámina en una matriz de celdas cuadradas, donde cada celda representa una porción del material con una temperatura específica. A lo largo del tiempo, la simulación calcula cómo la temperatura de cada celda varía debido a la transferencia de calor con sus celdas vecinas.

### Contexto

En un entorno experimental, se inyecta calor de manera constante a través de los bordes de la lámina. El propósito de la simulación es predecir el comportamiento térmico de la lámina hasta que se alcance el equilibrio térmico, es decir, cuando la temperatura de todas las celdas se estabiliza y deja de cambiar significativamente con el tiempo.

### Modelo de Transferencia de Calor

La simulación se basa en un modelo físico donde la temperatura de cada celda en un instante de tiempo se actualiza en función de las temperaturas de las celdas vecinas. Este modelo toma en cuenta la conductividad térmica del material y el tiempo que el calor tarda en difundirse a través de la lámina.

### Objetivo

El objetivo de la simulación es continuar actualizando las temperaturas de las celdas hasta que todas las celdas internas de la matriz presenten cambios de temperatura menores a un umbral(epsilon), indicando que el equilibrio térmico ha sido alcanzado.

## Características del Programa

- **Flexibilidad**: La simulación permite trabajar con láminas de diferentes tamaños y materiales, y los parámetros de entrada, como la duración de cada etapa y la difusividad térmica, son configurables.
- **Resultados**: El programa genera un reporte detallado de la simulación, incluyendo el número de estados hasta alcanzar el equilibrio y el tiempo transcurrido.

## Uso del Programa
Cada simulación involucra la lectura de una matriz de temperaturas desde un archivo binario, la actualización de las temperaturas hasta que se alcanza el equilibrio térmico, y la generación de un reporte con los resultados.

Comandos Básicos:
Para ejecutar una simulación, el comando básico es:

./bin/heatsim-serial <ruta_al_archivo_de_trabajo> <ruta_de_salida>

<ruta_al_archivo_de_trabajo>: Es la ruta al archivo de trabajo que contiene las especificaciones de la simulación, como job001.txt.
<ruta_de_salida>: Es la ruta donde se generará el reporte de la simulación, por ejemplo, job001.out o un directorio en el que se guardarán múltiples archivos de salida.
Ejemplo de Uso:

./bin/heatsim-serial tests/job001/job001.txt tests/job001/job001.out
Este comando ejecutará todas las simulaciones descritas en job001.txt y guardará el reporte en job001.out.

Formato del Archivo de Trabajo

El archivo de trabajo (job001.txt, por ejemplo) contiene una lista de simulaciones, donde cada línea define una simulación. El formato de cada línea es el siguiente:

<archivo_binario> <dtime> <alpha> <height> <epsilon>

<archivo_binario>: Nombre del archivo que contiene la matriz inicial de la lámina.
<dtime>: Duración de cada etapa en segundos.
<alpha>: Difusividad térmica del material.
<height>: Tamaño de las celdas.
<epsilon>: Umbral de cambio de temperatura para determinar el equilibrio.
Ejemplo de Contenido:

plate001.bin 1200 127 1000 2
plate001.bin 1200 127 1000 1.5
plate002.bin 60 0.08 450 0.075
Cada línea representa una simulación independiente que se ejecutará secuencialmente.

### Créditos
Algunas ideas o ayudas complementarias:

Manejo de archivos: https://youtu.be/4Ob7tCFaMHw
Diagrama UML: https://youtu.be/Z0yLerU0g-Q
Matrices dinámicas en c, lo básico: https://youtu.be/nrJS1cXyEKM

Comprobar si una ubicación es archivo o carpeta: chatgpt, imagen adjunta en design, nombre: archivo-carpeta.png

