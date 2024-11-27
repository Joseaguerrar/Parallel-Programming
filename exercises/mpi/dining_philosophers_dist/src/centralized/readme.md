# Filósofos Comensales - Solución Centralizada
## Descripción del Problema
El problema de los Filósofos Comensales, propuesto inicialmente por Edsger Dijkstra, es un ejercicio clásico en sistemas operativos que explora la sincronización de procesos en un entorno de recursos compartidos. La premisa básica es la siguiente:

Cinco filósofos están sentados alrededor de una mesa circular.
Cada filósofo alterna entre dos actividades: pensar y comer.
Para comer, un filósofo necesita dos palillos: uno a su izquierda y otro a su derecha.
Hay un total de cinco palillos en la mesa, compartidos entre los filósofos.
El reto consiste en diseñar un algoritmo que cumpla con las siguientes condiciones:

Ausencia de Deadlock: Los filósofos no deben quedarse bloqueados esperando palillos de forma indefinida.
Ausencia de Inanición: Ningún filósofo debe morir de hambre; todos deben eventualmente comer.
Concurrencia: Múltiples filósofos deben poder comer al mismo tiempo si los recursos lo permiten.
## Solución Centralizada
En esta implementación, se adopta un enfoque centralizado en el que un proceso adicional, denominado el mesero, actúa como árbitro para gestionar el acceso a los palillos. Este enfoque garantiza que no haya conflictos en el acceso a los recursos compartidos.

### Estrategia del Mesero
El mesero controla el estado de los cinco palillos (libres u ocupados).
Cuando un filósofo quiere comer, envía una solicitud al mesero.
El mesero otorga permiso solo si ambos palillos requeridos están disponibles.
Una vez que el filósofo termina de comer, notifica al mesero, quien libera los palillos y procesa solicitudes pendientes.
## Ventajas de la Solución Centralizada
Simplicidad: La lógica de sincronización está centralizada en un solo proceso, lo que facilita su comprensión e implementación.
Garantías de Seguridad: El mesero asegura que no haya condiciones de carrera ni conflictos entre procesos.
Evita Deadlock: El mesero coordina el acceso a los palillos de manera ordenada.