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

//falta//
