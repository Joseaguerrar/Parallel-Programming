# Manual de uso
 Para compilar el programa, se debe ejecutar el siguiente comando desde la carpeta `mpi`:

```bash
make
```

Para ejecutar el programa, se debe ejecutar el siguiente comando desde la carpeta `mpi`:

```bash
mpiexec -np <n> ./bin/mpi <folder> <jobName>
```

Donde `<n>` es el número de procesos a utilizar y `<folder>` es la ruta al directorio donde se encuentran los archivos binarios y `<jobName>` es el nombre del archivo de trabajo.

Por ejemplo, si se tiene un archivo de trabajo llamado `job01.txt` en la carpeta `mpi/bin`, se puede ejecutar el programa utilizando el siguiente comando:

```bash
mpiexec -np 4 ./bin/mpi tests/job001 job001.txt
```

Este comando ejecutará el programa utilizando 4 procesos, cargando los archivos binarios correspondientes a cada lámina en el directorio `mpi/bin`.