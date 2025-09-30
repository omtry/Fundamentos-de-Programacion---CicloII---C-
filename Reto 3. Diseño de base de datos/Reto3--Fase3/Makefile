# Makefile para el sistema de gestión de biblioteca en Windows con MinGW
# Compila Biblioteca.cpp y main.cpp en un ejecutable 'biblioteca.exe'

# Compilador a usar
CC = g++

# Banderas del compilador
CFLAGS = -std=c++17 -Wall -Wextra

# Nombre del ejecutable
TARGET = biblioteca.exe

# Archivos fuente
SOURCES = main.cpp Biblioteca.cpp

# Archivos objeto generados
OBJECTS = $(SOURCES:.cpp=.o)

# Archivo de cabecera
HEADERS = Biblioteca.h

# Regla por defecto: compila el ejecutable
all: $(TARGET)

# Regla para crear el ejecutable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Regla para compilar archivos .cpp a .o
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar archivos generados
clean:
	del /Q $(OBJECTS) $(TARGET)

# Regla para ejecutar el programa
run: $(TARGET)
	.\$(TARGET)

# Regla para recompilar y ejecutar
rebuild: clean all run

# Indica que estas reglas no son archivos
.PHONY: all clean run rebuild