Ejecute al menos tres veces los códigos de Ejemplo 7 (imprimir en orden con semáforos) y Ejemplo 8 (imprimir en orden con seguridad condicional) con tantos hilos como su sistema operativo permite.

Anote las tres duraciones de cada solución. Tome la mayor duración de las tres corridas de cada versión. Agregue los resultados a sus cuadros y gráficos del Ejercicio 14 [delayed_busy_wait].

Si en un proyecto con sus clientes usted tuviere que implementar una de las cuatro soluciones para hacer un trabajo en orden ¿Cuál solución escogería? Agregue además un párrafo a la discusión de Ejercicio 14 [delayed_busy_wait] explicando la potencial causa de este comportamiento.

ejemplo 7:
1. Execution time: 0.000828770s
2. Execution time: 0.000924876s //más alto
3. Execution time: 0.000774166s

ejemplo 8: 
1. Execution time: 0.000640056s
2. Execution time: 0.000654644s //más alto
3. Execution time: 0.000511117s

ejemplo busywait espera activa constante 50 micros: 
1. Execution time: 0.000825797s
2. Execution time: 0.000905653s
3. Execution time: 0.002497824s//

ejemplo busywait pseudoaleatorio: 
1. Execution time: 0.000810215s
2. Execution time: 0.001534428s //
3. Execution time: 0.000715121s

![Cuadro comparativo](image.png)