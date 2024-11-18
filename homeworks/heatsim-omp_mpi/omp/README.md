# Manual de uso
 Para compilar el programa, se debe ejecutar el siguiente comando desde la carpeta `omp`:

```bash
make
```

Para ejecutar el programa, se debe ejecutar el siguiente comando desde la carpeta `omp`:

```bash
./bin/omp <folder> <jobName> <num_threads> 
```

Donde `<folder>` es la ruta al directorio donde se encuentran los archivos binarios y `<jobName>` es el nombre del archivo de trabajo. Por defecto, el programa utilizará el número de hilos disponibles en el sistema. Si se desea especificar un número de hilos diferente, se debe agregar el parámetro `<num_threads>`.

Por ejemplo, si se tiene un archivo de trabajo llamado `job01.txt` en la carpeta `omp/bin`, se puede ejecutar el programa utilizando el siguiente comando:

```bash
./bin/omp tests/job001 job001.txt 4
```

Este comando ejecutará el programa utilizando el número de hilos disponibles en el sistema, cargando los archivos binarios correspondientes a cada lámina en el directorio `omp/bin`.