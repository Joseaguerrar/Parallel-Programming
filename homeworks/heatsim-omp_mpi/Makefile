# Variables esenciales
CC = mpicc
CFLAGS = -Wall -Wextra -g -std=c17 -fopenmp -Isrc
LDFLAGS = -fopenmp
OPTFLAGS = -O3 -DNDEBUG  # Flags de optimización para release
SRC = src
BUILD = build
BIN = bin
APPNAME = heatsim-omp_mpi

# Archivos del proyecto
SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(SOURCES:$(SRC)/%.c=$(BUILD)/%.o)
EXE = $(BIN)/$(APPNAME)

# Objetivo por defecto
all: $(EXE)

# Compilar el ejecutable
$(EXE): $(OBJECTS)
	mkdir -p $(BIN)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Compilar cada archivo .c a un .o
$(BUILD)/%.o: $(SRC)/%.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar en modo release (optimizado)
release: CFLAGS = -Wall -Wextra $(OPTFLAGS) -std=c17 -fopenmp -Isrc
release: clean $(EXE)

# Limpieza
clean:
	rm -rf $(BUILD) $(BIN)
