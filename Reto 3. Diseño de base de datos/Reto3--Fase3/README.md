Sistema de Gestion de Bibliotecas:

Descripcion: Este proyecto es un sistema de gestión de bibliotecas implementado en C++ que permite administrar estudiantes, autores, editoriales, libros y préstamos. Los datos se almacenan en archivos CSV (estudiantes.txt, autores.txt, editoriales.txt, libros.txt, prestamos.txt) y se gestionan mediante una clase BibliotecaDB. El sistema soporta operaciones CRUD (Crear, Leer, Actualizar, Eliminar) para todas las entidades, con validaciones para mantener la integridad de los datos.

Características

    Gestión de entidades:

        1-Estudiantes: Agregar, listar, actualizar y eliminar estudiantes (ID, nombre, grado).
        2-Autores: Agregar, listar, actualizar y eliminar autores (ID, nombre, nacionalidad).
        3-Editoriales: Agregar, listar, actualizar y eliminar editoriales (ID, nombre).
        4-Libros: Agregar, listar, actualizar y eliminar libros (ID, título, ISBN, año, ID autor, ID editorial).
        5-Préstamos: Registrar préstamos, devolver libros, listar préstamos (activos o todos) y consultar préstamos por estudiante.

    Validaciones:

        -IDs únicos para cada entidad.
        -ISBN único para libros.
        -Verificación de existencia de autor/editorial al agregar libros.
        -Prevención de eliminación de entidades referenciadas (por ejemplo, autores con libros asociados o estudiantes con    préstamos activos).
        -Validación de formato de fecha (YYYY-MM-DD) para préstamos.

    Persistencia: Los datos se guardan y cargan desde archivos CSV, con manejo de comas y comillas para campos complejos.
    Portabilidad: Compatible con Windows, Linux y macOS gracias al manejo de fechas con #ifdef _WIN32.


Para ser capaces de correr el codigo se tienen como requisitos minimos: 

    1-Compilador C++ compatible con C++11 o superior (por ejemplo, g++, MinGW con el atributo make).
    2-Sistema operativo: Windows, Linux o macOS.
    3-No se requieren bibliotecas externas más allá de la STL (Standard Template Library).


Para Correr el Programa:

    1- Clona o descarga el repositorio des GitHub.
    2- Asegúrate de que los archivos Biblioteca.h, biblioteca.cpp, main.cpp y Makefile estén en el mismo directorio.
    3- Compila el programa usando Makefile, abriendo la terminal dentro de la direccion de la carpeta en la que se tienen los documentos y escribir el comando: mingw32-make (Esta alternativa solo funciona si el ussuario tiene en su computadora instalado MinGW con el atributo make y g++) y para correrlo solo se escribe el comando: mingw32-make run
    4- Si se realizan cambios es conveniente utilizar mingw32-make clean para borrar cualquier archivo que haya quedado guardado o resagado de versiones anteriores
    

El sistema interactúa a través de la consola, solicitando entradas del usuario para realizar operaciones. Ejemplo de flujo:

    1-Agregar un estudiante: Ingresa un ID único, nombre y grado. El sistema valida que el ID no exista y guarda el estudiante en estudiantes.txt.

    2-Prestar un libro: Ingresa el ID del libro, el ID del estudiante y la fecha de préstamo (YYYY-MM-DD).
    El sistema verifica la existencia del libro/estudiante y que el libro no esté prestado.

    3- Listar préstamos: Muestra todos los préstamos (o solo los activos) con detalles de libros y estudiantes.

    4- Actualizar o eliminar: Ingresa el ID de la entidad y los nuevos valores (presiona Enter para mantener los actuales). No se permite eliminar entidades con referencias activas (por ejemplo, un libro con préstamo activo).

Estructura de Archivos:

    Biblioteca.h: Define las estructuras (Estudiante, Autor, Editorial, Libro, Prestamo) y la clase BibliotecaDB.
    Biblioteca.cpp: Implementa los métodos de BibliotecaDB para gestionar entidades y archivos CSV.
    Archivos de datos:

        estudiantes.txt: ID,nombre,grado
        autores.txt: ID,nombre,nacionalidad
        editoriales.txt: ID,nombre
        libros.txt: ID,título,ISBN,año,ID_autor,ID_editorial
        prestamos.txt: ID,ID_libro,ID_estudiante,fecha_prestamo,fecha_devolucion    

Desarrollado como parte de un proyecto académico para la gestión de bibliotecas por Gerardo Andre Calderon Castillo -- KEY-00007 de Ingenieria Industrial y Manufactura Avanzada