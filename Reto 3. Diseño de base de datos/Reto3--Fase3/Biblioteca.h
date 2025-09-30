#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <string>
#include <vector>

// Representa un estudiante en el sistema de biblioteca
struct Estudiante {
    int id;                    // Identificador unico del estudiante
    std::string nombre;        // Nombre completo del estudiante
    std::string grado;         // Grado academico (e.g., "1er ano", "Maestria")
};

// Representa un autor de libros
struct Autor {
    int id;                    // Identificador unico del autor
    std::string nombre;        // Nombre completo del autor
    std::string nacionalidad;  // Nacionalidad del autor (e.g., "Mexicana")
};

// Representa una editorial que publica libros
struct Editorial {
    int id;                    // Identificador unico de la editorial
    std::string nombre;        // Nombre de la editorial (e.g., "Planeta")
};

// Representa un libro en la biblioteca
struct Libro {
    int id;                    // Identificador unico del libro
    std::string titulo;        // Titulo del libro
    std::string isbn;          // ISBN unico del libro
    int anio;                  // Ano de publicacion
    int id_autor;              // ID del autor asociado
    int id_editorial;          // ID de la editorial asociada
};

// Representa un prestamo de un libro a un estudiante
struct Prestamo {
    int id;                    // Identificador unico del prestamo
    int id_libro;              // ID del libro prestado
    int id_estudiante;         // ID del estudiante que solicito el prestamo
    std::string fecha_prestamo;   // Fecha de prestamo (formato YYYY-MM-DD)
    std::string fecha_devolucion; // Fecha de devolucion (vacia si no devuelto)
};

// Clase que gestiona la base de datos en memoria y operaciones CRUD/persistencia
class BibliotecaDB {
public:
    // Vectores para almacenar datos en memoria
    std::vector<Estudiante> estudiantes;  // Lista de estudiantes registrados
    std::vector<Autor> autores;           // Lista de autores registrados
    std::vector<Editorial> editoriales;   // Lista de editoriales registradas
    std::vector<Libro> libros;            // Lista de libros registrados
    std::vector<Prestamo> prestamos;      // Lista de prestamos registrados

    // --- Persistencia ---
    bool cargarDatos();                   // Carga todos los datos desde archivos CSV
    bool guardarDatos();                  // Guarda todos los datos en archivos CSV

    // --- CRUD para Estudiante ---
    int nextEstudianteId() const;                         // Genera el siguiente ID unico
    bool agregarEstudiante(const Estudiante& e);          // Agrega un estudiante, valida ID unico
    void listarEstudiantes() const;                       // Muestra todos los estudiantes
    Estudiante* buscarEstudiantePorId(int id);            // Version modificable
    const Estudiante* buscarEstudiantePorId(int id) const; // Version solo lectura
    bool actualizarEstudiante(int id);                    // Actualiza datos de un estudiante
    bool eliminarEstudiante(int id);                      // Elimina estudiante, valida prestamos activos

    // --- CRUD para Autor ---
    int nextAutorId() const;                              
    bool agregarAutor(const Autor& a);                    
    void listarAutores() const;                           
    Autor* buscarAutorPorId(int id);                      
    const Autor* buscarAutorPorId(int id) const;          
    bool actualizarAutor(int id);                         
    bool eliminarAutor(int id);                           

    // --- CRUD para Editorial ---
    int nextEditorialId() const;                           
    bool agregarEditorial(const Editorial& ed);           
    void listarEditoriales() const;                        
    Editorial* buscarEditorialPorId(int id);              
    const Editorial* buscarEditorialPorId(int id) const;  
    bool actualizarEditorial(int id);                      
    bool eliminarEditorial(int id);                        

    // --- CRUD para Libro ---
    int nextLibroId() const;                               
    bool agregarLibro(const Libro& l);                     
    void listarLibros() const;                              
    Libro* buscarLibroPorId(int id);                       
    const Libro* buscarLibroPorId(int id) const;           
    bool actualizarLibro(int id);                           
    bool eliminarLibro(int id);                             

    // --- Gestion de Prestamos ---
    int nextPrestamoId() const;                              
    bool prestarLibro(int id_libro, int id_estudiante, const std::string& fecha_prestamo); 
    bool devolverPrestamo(int id_prestamo);                 
    void listarPrestamos(bool soloActivos = false) const;   
    void listarPrestamosPorEstudiante(int id_estudiante) const; 
    Prestamo* buscarPrestamoPorId(int id);                  
    const Prestamo* buscarPrestamoPorId(int id) const;      
    std::string fechaHoy() const;                            

private:
    // --- Persistencia por entidad ---
    bool guardarEstudiantes() const;      
    bool cargarEstudiantes();             
    bool guardarAutores() const;          
    bool cargarAutores();                 
    bool guardarEditoriales() const;      
    bool cargarEditoriales();             
    bool guardarLibros() const;           
    bool cargarLibros();                  
    bool guardarPrestamos() const;        
    bool cargarPrestamos();               

    // --- Manejo CSV ---
    std::vector<std::string> splitLine(const std::string& s, char delimiter) const; 
    std::string escapeField(const std::string& s) const; 
    std::string unescapeField(const std::string& s) const; 
};

#endif // BIBLIOTECA_H
