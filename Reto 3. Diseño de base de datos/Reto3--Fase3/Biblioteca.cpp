#include "Biblioteca.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <limits>
#include <regex>

// --- Métodos auxiliares para la biblioteca ---

// Divide una línea CSV en campos, respetando comillas para valores que contienen comas
std::vector<std::string> BibliotecaDB::splitLine(const std::string& s, char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;
    // Itera cada carácter de la línea para separar campos
    for (char c : s) {
        if (c == '"') {
            inQuotes = !inQuotes; // Alterna estado de comillas
        } else if (c == delimiter && !inQuotes) {
            tokens.push_back(unescapeField(token)); // Procesa y añade campo completo
            token.clear();
        } else {
            token += c; // Acumula caracteres en el campo actual
        }
    }
    if (!token.empty()) {
        tokens.push_back(unescapeField(token)); // Añade el último campo
    }
    return tokens;
}

// Escapa comas y barras verticales en un campo para cumplir con el formato CSV
std::string BibliotecaDB::escapeField(const std::string& s) const {
    std::string result = s;
    // Reemplaza barras verticales por punto y coma para evitar conflictos
    std::replace(result.begin(), result.end(), '|', ';');
    // Encierra el campo en comillas si contiene comas o barras
    if (result.find(',') != std::string::npos || result.find('|') != std::string::npos) {
        result = "\"" + result + "\"";
    }
    return result;
}

// Revierte el escape de un campo CSV, eliminando comillas y restaurando barras verticales
std::string BibliotecaDB::unescapeField(const std::string& s) const {
    std::string result = s;
    // Elimina comillas iniciales y finales si existen
    if (!result.empty() && result.front() == '"' && result.back() == '"') {
        result = result.substr(1, result.size() - 2);
    }
    // Restaura barras verticales reemplazando punto y coma
    std::replace(result.begin(), result.end(), ';', '|');
    return result;
}

// Obtiene la fecha actual en formato YYYY-MM-DD para registrar préstamos o devoluciones
std::string BibliotecaDB::fechaHoy() const {
    std::time_t now = std::time(nullptr);
    std::tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &now); // Versión segura para Windows
#else
    localtime_r(&now, &tm_now); // Versión segura para sistemas POSIX
#endif
    char buffer[11];
    // Formatea la fecha como YYYY-MM-DD
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm_now);
    return std::string(buffer);
}

// Carga todos los datos desde archivos CSV al iniciar el sistema
bool BibliotecaDB::cargarDatos() {
    // Intenta cargar todas las entidades; retorna false si alguna falla
    return cargarEstudiantes() && cargarAutores() && cargarEditoriales() && cargarLibros() && cargarPrestamos();
}

// Guarda todas las entidades en sus respectivos archivos CSV
bool BibliotecaDB::guardarDatos() {
    // Intenta guardar todas las entidades; retorna false si alguna falla
    return guardarEstudiantes() && guardarAutores() && guardarEditoriales() && guardarLibros() && guardarPrestamos();
}

// --- Gestión de Estudiantes ---

// Genera el siguiente ID único para un nuevo estudiante
int BibliotecaDB::nextEstudianteId() const {
    int maxId = 0;
    // Busca el mayor ID existente en la lista de estudiantes
    for (const auto& e : estudiantes) {
        maxId = std::max(maxId, e.id);
    }
    return maxId + 1; // Retorna el siguiente ID disponible
}

// Agrega un estudiante nuevo, asegurando que el ID sea único
bool BibliotecaDB::agregarEstudiante(const Estudiante& e) {
    // Verifica si el ID ya está en uso
    for (const auto& est : estudiantes) {
        if (est.id == e.id) {
            std::cout << "Error: ID de estudiante " << e.id << " ya existe.\n";
            return false;
        }
    }
    estudiantes.push_back(e); // Añade el estudiante al vector
    return guardarEstudiantes(); // Persiste los cambios en el archivo
}

// Muestra la lista completa de estudiantes registrados
void BibliotecaDB::listarEstudiantes() const {
    std::cout << "\n---- Estudiantes (" << estudiantes.size() << ") ----\n";
    if (estudiantes.empty()) {
        std::cout << "No hay estudiantes registrados.\n";
        return;
    }
    // Itera e imprime cada estudiante con su ID, nombre y grado
    for (const auto& e : estudiantes) {
        std::cout << "ID: " << e.id << " | Nombre: " << e.nombre << " | Grado: " << e.grado << "\n";
    }
}

// Busca un estudiante por ID, retorna puntero constante para acceso de solo lectura
const Estudiante* BibliotecaDB::buscarEstudiantePorId(int id) const {
    // Itera la lista de estudiantes para encontrar el ID solicitado
    for (const auto& e : estudiantes) {
        if (e.id == id) return &e;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Busca un estudiante por ID, retorna puntero modificable para edición
Estudiante* BibliotecaDB::buscarEstudiantePorId(int id) {
    // Itera la lista de estudiantes para encontrar el ID solicitado
    for (auto& e : estudiantes) {
        if (e.id == id) return &e;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Actualiza los datos de un estudiante existente (nombre o grado)
bool BibliotecaDB::actualizarEstudiante(int id) {
    Estudiante* e = buscarEstudiantePorId(id);
    if (!e) {
        std::cout << "Error: Estudiante ID " << id << " no encontrado.\n";
        return false;
    }
    std::cout << "Actualizar Estudiante ID " << id << " (Enter para mantener valor):\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Nombre actual: " << e->nombre << "\nNuevo nombre: ";
    std::string s;
    std::getline(std::cin, s);
    // Actualiza el nombre solo si se ingresa un valor nuevo
    if (!s.empty()) e->nombre = s;
    std::cout << "Grado actual: " << e->grado << "\nNuevo grado: ";
    std::getline(std::cin, s);
    // Actualiza el grado solo si se ingresa un valor nuevo
    if (!s.empty()) e->grado = s;
    return guardarEstudiantes(); // Persiste los cambios
}

// Elimina un estudiante, verificando que no tenga préstamos activos
bool BibliotecaDB::eliminarEstudiante(int id) {
    // Verifica si el estudiante tiene préstamos pendientes
    for (const auto& p : prestamos) {
        if (p.id_estudiante == id && p.fecha_devolucion.empty()) {
            std::cout << "Error: Estudiante tiene prestamo activo (ID Prestamo " << p.id << ").\n";
            return false;
        }
    }
    // Elimina el estudiante de la lista usando remove_if
    auto it = std::remove_if(estudiantes.begin(), estudiantes.end(),
                             [id](const Estudiante& e) { return e.id == id; });
    if (it == estudiantes.end()) {
        std::cout << "Error: Estudiante ID " << id << " no encontrado.\n";
        return false;
    }
    estudiantes.erase(it, estudiantes.end()); // Borra los elementos eliminados
    return guardarEstudiantes(); // Persiste los cambios
}

// --- Gestión de Autores ---

// Genera el siguiente ID único para un nuevo autor
int BibliotecaDB::nextAutorId() const {
    int maxId = 0;
    // Busca el mayor ID existente en la lista de autores
    for (const auto& a : autores) {
        maxId = std::max(maxId, a.id);
    }
    return maxId + 1; // Retorna el siguiente ID disponible
}

// Agrega un autor nuevo, asegurando que el ID sea único
bool BibliotecaDB::agregarAutor(const Autor& a) {
    // Verifica si el ID ya está en uso
    for (const auto& aut : autores) {
        if (aut.id == a.id) {
            std::cout << "Error: ID de autor " << a.id << " ya existe.\n";
            return false;
        }
    }
    autores.push_back(a); // Añade el autor al vector
    return guardarAutores(); // Persiste los cambios en el archivo
}

// Muestra la lista completa de autores registrados
void BibliotecaDB::listarAutores() const {
    std::cout << "\n---- Autores (" << autores.size() << ") ----\n";
    if (autores.empty()) {
        std::cout << "No hay autores registrados.\n";
        return;
    }
    // Itera e imprime cada autor con su ID, nombre y nacionalidad
    for (const auto& a : autores) {
        std::cout << "ID: " << a.id << " | Nombre: " << a.nombre << " | Nacionalidad: " << a.nacionalidad << "\n";
    }
}

// Busca un autor por ID, retorna puntero constante para acceso de solo lectura
const Autor* BibliotecaDB::buscarAutorPorId(int id) const {
    // Itera la lista de autores para encontrar el ID solicitado
    for (const auto& a : autores) {
        if (a.id == id) return &a;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Busca un autor por ID, retorna puntero modificable para edición
Autor* BibliotecaDB::buscarAutorPorId(int id) {
    // Itera la lista de autores para encontrar el ID solicitado
    for (auto& a : autores) {
        if (a.id == id) return &a;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Actualiza los datos de un autor existente (nombre o nacionalidad)
bool BibliotecaDB::actualizarAutor(int id) {
    Autor* a = buscarAutorPorId(id);
    if (!a) {
        std::cout << "Error: Autor ID " << id << " no encontrado.\n";
        return false;
    }
    std::cout << "Actualizar Autor ID " << id << " (Enter para mantener valor):\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Nombre actual: " << a->nombre << "\nNuevo nombre: ";
    std::string s;
    std::getline(std::cin, s);
    // Actualiza el nombre solo si se ingresa un valor nuevo
    if (!s.empty()) a->nombre = s;
    std::cout << "Nacionalidad actual: " << a->nacionalidad << "\nNueva nacionalidad: ";
    std::getline(std::cin, s);
    // Actualiza la nacionalidad solo si se ingresa un valor nuevo
    if (!s.empty()) a->nacionalidad = s;
    return guardarAutores(); // Persiste los cambios
}

// Elimina un autor, verificando que no esté asociado a ningún libro
bool BibliotecaDB::eliminarAutor(int id) {
    // Verifica si el autor está referenciado por algún libro
    for (const auto& l : libros) {
        if (l.id_autor == id) {
            std::cout << "Error: Autor referenciado por libro ID " << l.id << ".\n";
            return false;
        }
    }
    // Elimina el autor de la lista usando remove_if
    auto it = std::remove_if(autores.begin(), autores.end(),
                             [id](const Autor& a) { return a.id == id; });
    if (it == autores.end()) {
        std::cout << "Error: Autor ID " << id << " no encontrado.\n";
        return false;
    }
    autores.erase(it, autores.end()); // Borra los elementos eliminados
    return guardarAutores(); // Persiste los cambios
}

// --- Gestión de Editoriales ---

// Genera el siguiente ID único para una nueva editorial
int BibliotecaDB::nextEditorialId() const {
    int maxId = 0;
    // Busca el mayor ID existente en la lista de editoriales
    for (const auto& ed : editoriales) {
        maxId = std::max(maxId, ed.id);
    }
    return maxId + 1; // Retorna el siguiente ID disponible
}

// Agrega una editorial nueva, asegurando que el ID sea único
bool BibliotecaDB::agregarEditorial(const Editorial& ed) {
    // Verifica si el ID ya está en uso
    for (const auto& e : editoriales) {
        if (e.id == ed.id) {
            std::cout << "Error: ID de editorial " << ed.id << " ya existe.\n";
            return false;
        }
    }
    editoriales.push_back(ed); // Añade la editorial al vector
    return guardarEditoriales(); // Persiste los cambios en el archivo
}

// Muestra la lista completa de editoriales registradas
void BibliotecaDB::listarEditoriales() const {
    std::cout << "\n---- Editoriales (" << editoriales.size() << ") ----\n";
    if (editoriales.empty()) {
        std::cout << "No hay editoriales registradas.\n";
        return;
    }
    // Itera e imprime cada editorial con su ID y nombre
    for (const auto& ed : editoriales) {
        std::cout << "ID: " << ed.id << " | Nombre: " << ed.nombre << "\n";
    }
}

// Busca una editorial por ID, retorna puntero constante para acceso de solo lectura
const Editorial* BibliotecaDB::buscarEditorialPorId(int id) const {
    // Itera la lista de editoriales para encontrar el ID solicitado
    for (const auto& ed : editoriales) {
        if (ed.id == id) return &ed;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Busca una editorial por ID, retorna puntero modificable para edición
Editorial* BibliotecaDB::buscarEditorialPorId(int id) {
    // Itera la lista de editoriales para encontrar el ID solicitado
    for (auto& ed : editoriales) {
        if (ed.id == id) return &ed;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Actualiza el nombre de una editorial existente
bool BibliotecaDB::actualizarEditorial(int id) {
    Editorial* ed = buscarEditorialPorId(id);
    if (!ed) {
        std::cout << "Error: Editorial ID " << id << " no encontrada.\n";
        return false;
    }
    std::cout << "Actualizar Editorial ID " << id << " (Enter para mantener valor):\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Nombre actual: " << ed->nombre << "\nNuevo nombre: ";
    std::string s;
    std::getline(std::cin, s);
    // Actualiza el nombre solo si se ingresa un valor nuevo
    if (!s.empty()) ed->nombre = s;
    return guardarEditoriales(); // Persiste los cambios
}

// Elimina una editorial, verificando que no esté asociada a ningún libro
bool BibliotecaDB::eliminarEditorial(int id) {
    // Verifica si la editorial está referenciada por algún libro
    for (const auto& l : libros) {
        if (l.id_editorial == id) {
            std::cout << "Error: Editorial referenciada por libro ID " << l.id << ".\n";
            return false;
        }
    }
    // Elimina la editorial de la lista usando remove_if
    auto it = std::remove_if(editoriales.begin(), editoriales.end(),
                             [id](const Editorial& ed) { return ed.id == id; });
    if (it == editoriales.end()) {
        std::cout << "Error: Editorial ID " << id << " no encontrada.\n";
        return false;
    }
    editoriales.erase(it, editoriales.end()); // Borra los elementos eliminados
    return guardarEditoriales(); // Persiste los cambios
}

// --- Gestión de Libros ---

// Genera el siguiente ID único para un nuevo libro
int BibliotecaDB::nextLibroId() const {
    int maxId = 0;
    // Busca el mayor ID existente en la lista de libros
    for (const auto& l : libros) {
        maxId = std::max(maxId, l.id);
    }
    return maxId + 1; // Retorna el siguiente ID disponible
}

// Agrega un libro nuevo, validando ID, ISBN, autor y editorial
bool BibliotecaDB::agregarLibro(const Libro& l) {
    // Verifica unicidad del ID del libro
    for (const auto& lib : libros) {
        if (lib.id == l.id) {
            std::cout << "Error: ID de libro " << l.id << " ya existe.\n";
            return false;
        }
        // Verifica unicidad del ISBN
        if (lib.isbn == l.isbn) {
            std::cout << "Error: ISBN " << l.isbn << " ya existe (Libro ID " << lib.id << ").\n";
            return false;
        }
    }
    // Valida que el año esté en un rango razonable
    if (l.anio < 0 || l.anio > 2025) {
        std::cout << "Error: Ano invalido (debe ser entre 0 y 2025).\n";
        return false;
    }
    // Valida la existencia del autor
    if (!buscarAutorPorId(l.id_autor)) {
        std::cout << "Error: Autor ID " << l.id_autor << " no existe.\n";
        return false;
    }
    // Valida la existencia de la editorial
    if (!buscarEditorialPorId(l.id_editorial)) {
        std::cout << "Error: Editorial ID " << l.id_editorial << " no existe.\n";
        return false;
    }
    libros.push_back(l); // Añade el libro al vector
    return guardarLibros(); // Persiste los cambios
}

// Muestra la lista completa de libros con detalles de autor y editorial
void BibliotecaDB::listarLibros() const {
    std::cout << "\n---- Libros (" << libros.size() << ") ----\n";
    if (libros.empty()) {
        std::cout << "No hay libros registrados.\n";
        return;
    }
    // Itera cada libro, recuperando autor y editorial asociados
    for (const auto& l : libros) {
        const Autor* a = buscarAutorPorId(l.id_autor);
        const Editorial* ed = buscarEditorialPorId(l.id_editorial);
        std::cout << "ID: " << l.id << " | Titulo: " << l.titulo << " | ISBN: " << l.isbn
                  << " | Ano: " << l.anio << " | Autor: " << (a ? a->nombre : "Desconocido")
                  << " | Editorial: " << (ed ? ed->nombre : "Desconocida") << "\n";
    }
}

// Busca un libro por ID, retorna puntero constante para acceso de solo lectura
const Libro* BibliotecaDB::buscarLibroPorId(int id) const {
    // Itera la lista de libros para encontrar el ID solicitado
    for (const auto& l : libros) {
        if (l.id == id) return &l;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Busca un libro por ID, retorna puntero modificable para edición
Libro* BibliotecaDB::buscarLibroPorId(int id) {
    // Itera la lista de libros para encontrar el ID solicitado
    for (auto& l : libros) {
        if (l.id == id) return &l;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Actualiza los datos de un libro existente (título, ISBN, año, autor, editorial)
bool BibliotecaDB::actualizarLibro(int id) {
    Libro* l = buscarLibroPorId(id);
    if (!l) {
        std::cout << "Error: Libro ID " << id << " no encontrado.\n";
        return false;
    }
    std::cout << "Actualizar Libro ID " << id << " (Enter para mantener valor):\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string s;

    // Actualiza el título si se ingresa un valor nuevo
    std::cout << "Titulo actual: " << l->titulo << "\nNuevo titulo: ";
    std::getline(std::cin, s);
    if (!s.empty()) l->titulo = s;

    // Actualiza el ISBN, verificando unicidad
    std::cout << "ISBN actual: " << l->isbn << "\nNuevo ISBN: ";
    std::getline(std::cin, s);
    if (!s.empty()) {
        for (const auto& other : libros) {
            if (other.id != l->id && other.isbn == s) {
                std::cout << "Error: ISBN " << s << " ya usado por otro libro.\n";
                return false;
            }
        }
        l->isbn = s;
    }

    // Actualiza el año, validando el rango
    std::cout << "Ano actual: " << l->anio << "\nNuevo ano (Enter para mantener): ";
    std::getline(std::cin, s);
    if (!s.empty()) {
        try {
            int nuevoAno = std::stoi(s);
            if (nuevoAno < 0 || nuevoAno > 2025) {
                std::cout << "Error: Ano invalido, se mantiene el anterior.\n";
            } else {
                l->anio = nuevoAno;
            }
        } catch (...) {
            std::cout << "Error: Ano invalido, se mantiene el anterior.\n";
        }
    }

    // Actualiza el ID del autor, validando su existencia
    std::cout << "ID Autor actual: " << l->id_autor << "\nNuevo ID Autor (Enter para mantener): ";
    std::getline(std::cin, s);
    if (!s.empty()) {
        try {
            int idaut = std::stoi(s);
            if (!buscarAutorPorId(idaut)) {
                std::cout << "Error: Autor ID " << idaut << " no existe, se mantiene el anterior.\n";
            } else {
                l->id_autor = idaut;
            }
        } catch (...) {
            std::cout << "Error: ID Autor invalido, se mantiene el anterior.\n";
        }
    }

    // Actualiza el ID de la editorial, validando su existencia
    std::cout << "ID Editorial actual: " << l->id_editorial << "\nNuevo ID Editorial (Enter para mantener): ";
    std::getline(std::cin, s);
    if (!s.empty()) {
        try {
            int ided = std::stoi(s);
            if (!buscarEditorialPorId(ided)) {
                std::cout << "Error: Editorial ID " << ided << " no existe, se mantiene la anterior.\n";
            } else {
                l->id_editorial = ided;
            }
        } catch (...) {
            std::cout << "Error: ID Editorial invalido, se mantiene la anterior.\n";
        }
    }

    return guardarLibros(); // Persiste los cambios
}

// Elimina un libro, verificando que no tenga préstamos activos
bool BibliotecaDB::eliminarLibro(int id) {
    // Verifica si el libro está en un préstamo activo
    for (const auto& p : prestamos) {
        if (p.id_libro == id && p.fecha_devolucion.empty()) {
            std::cout << "Error: Libro tiene prestamo activo (ID Prestamo " << p.id << ").\n";
            return false;
        }
    }
    // Elimina el libro de la lista usando remove_if
    auto it = std::remove_if(libros.begin(), libros.end(),
                             [id](const Libro& l) { return l.id == id; });
    if (it == libros.end()) {
        std::cout << "Error: Libro ID " << id << " no encontrado.\n";
        return false;
    }
    libros.erase(it, libros.end()); // Borra los elementos eliminados
    return guardarLibros(); // Persiste los cambios
}

// --- Gestión de Préstamos ---

// Genera el siguiente ID único para un nuevo préstamo
int BibliotecaDB::nextPrestamoId() const {
    int maxId = 0;
    // Busca el mayor ID existente en la lista de préstamos
    for (const auto& p : prestamos) {
        maxId = std::max(maxId, p.id);
    }
    return maxId + 1; // Retorna el siguiente ID disponible
}

// Registra un nuevo préstamo, validando libro, estudiante y disponibilidad
bool BibliotecaDB::prestarLibro(int id_libro, int id_estudiante, const std::string& fecha_prestamo) {
    // Valida el formato de la fecha (YYYY-MM-DD)
    std::regex dateRegex("\\d{4}-\\d{2}-\\d{2}");
    if (!std::regex_match(fecha_prestamo, dateRegex)) {
        std::cout << "Error: Formato de fecha invalido (use YYYY-MM-DD).\n";
        return false;
    }

    // Valida la existencia del libro
    Libro* l = buscarLibroPorId(id_libro);
    if (!l) {
        std::cout << "Error: Libro ID " << id_libro << " no existe.\n";
        return false;
    }

    // Valida la existencia del estudiante
    Estudiante* e = buscarEstudiantePorId(id_estudiante);
    if (!e) {
        std::cout << "Error: Estudiante ID " << id_estudiante << " no existe.\n";
        return false;
    }

    // Verifica si el libro ya está prestado
    for (const auto& p : prestamos) {
        if (p.id_libro == id_libro && p.fecha_devolucion.empty()) {
            std::cout << "Error: Libro ya esta prestado (ID Prestamo " << p.id << ").\n";
            return false;
        }
    }

    // Crea y registra el nuevo préstamo
    Prestamo p;
    p.id = nextPrestamoId();
    p.id_libro = id_libro;
    p.id_estudiante = id_estudiante;
    p.fecha_prestamo = fecha_prestamo;
    p.fecha_devolucion = "";
    prestamos.push_back(p);
    return guardarPrestamos(); // Persiste los cambios
}

// Registra la devolución de un préstamo, actualizando la fecha de devolución
bool BibliotecaDB::devolverPrestamo(int id_prestamo) {
    Prestamo* p = buscarPrestamoPorId(id_prestamo);
    if (!p) {
        std::cout << "Error: Prestamo ID " << id_prestamo << " no existe.\n";
        return false;
    }
    // Verifica si el préstamo ya fue devuelto
    if (!p->fecha_devolucion.empty()) {
        std::cout << "Error: Prestamo ya devuelto el " << p->fecha_devolucion << ".\n";
        return false;
    }
    p->fecha_devolucion = fechaHoy(); // Asigna la fecha actual
    return guardarPrestamos(); // Persiste los cambios
}

// Muestra todos los préstamos o solo los activos, con detalles de libro y estudiante
void BibliotecaDB::listarPrestamos(bool soloActivos) const {
    std::cout << "\n---- Prestamos (" << prestamos.size() << ") ----\n";
    if (prestamos.empty()) {
        std::cout << "No hay prestamos registrados.\n";
        return;
    }
    int activos = 0;
    // Itera cada préstamo, mostrando solo los activos si se especifica
    for (const auto& p : prestamos) {
        if (soloActivos && !p.fecha_devolucion.empty()) continue;
        const Libro* l = buscarLibroPorId(p.id_libro);
        const Estudiante* e = buscarEstudiantePorId(p.id_estudiante);
        std::cout << "ID Prestamo: " << p.id << " | Libro ID: " << p.id_libro << " (" << (l ? l->titulo : "Desconocido") << ")"
                  << " | Estudiante ID: " << p.id_estudiante << " (" << (e ? e->nombre : "Desconocido") << ")"
                  << " | Fecha Prestamo: " << p.fecha_prestamo
                  << " | Fecha Devolucion: " << (p.fecha_devolucion.empty() ? "(Pendiente)" : p.fecha_devolucion)
                  << "\n";
        if (soloActivos) activos++;
    }
    if (soloActivos && activos == 0) {
        std::cout << "No hay prestamos activos.\n";
    }
}

// Muestra los préstamos asociados a un estudiante específico
void BibliotecaDB::listarPrestamosPorEstudiante(int id_estudiante) const {
    std::cout << "\n---- Prestamos para Estudiante ID " << id_estudiante << " ----\n";
    const Estudiante* e = buscarEstudiantePorId(id_estudiante);
    std::string nombre = e ? e->nombre : "Desconocido";
    std::cout << "Estudiante: " << nombre << "\n";
    bool found = false;
    // Itera los préstamos filtrando por ID de estudiante
    for (const auto& p : prestamos) {
        if (p.id_estudiante == id_estudiante) {
            found = true;
            const Libro* l = buscarLibroPorId(p.id_libro);
            std::cout << "  ID Prestamo: " << p.id << " | Libro: " << (l ? l->titulo : "Desconocido")
                      << " | Fecha Prestamo: " << p.fecha_prestamo
                      << " | Fecha Devolucion: " << (p.fecha_devolucion.empty() ? "(Pendiente)" : p.fecha_devolucion)
                      << "\n";
        }
    }
    if (!found) std::cout << "  (Ningun prestamo registrado)\n";
}

// Busca un préstamo por ID, retorna puntero constante para acceso de solo lectura
const Prestamo* BibliotecaDB::buscarPrestamoPorId(int id) const {
    // Itera la lista de préstamos para encontrar el ID solicitado
    for (const auto& p : prestamos) {
        if (p.id == id) return &p;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// Busca un préstamo por ID, retorna puntero modificable para edición
Prestamo* BibliotecaDB::buscarPrestamoPorId(int id) {
    // Itera la lista de préstamos para encontrar el ID solicitado
    for (auto& p : prestamos) {
        if (p.id == id) return &p;
    }
    return nullptr; // Retorna nullptr si no se encuentra
}

// --- Persistencia (guardar/cargar) ---

// Guarda la lista de estudiantes en estudiantes.txt en formato CSV
bool BibliotecaDB::guardarEstudiantes() const {
    std::ofstream file("estudiantes.txt");
    if (!file.is_open()) {
        std::cout << "Error al abrir estudiantes.txt para guardar.\n";
        return false;
    }
    // Escribe cada estudiante como una línea CSV: id,nombre,grado
    for (const auto& e : estudiantes) {
        file << e.id << "," << escapeField(e.nombre) << "," << escapeField(e.grado) << "\n";
    }
    file.close();
    return true;
}

// Carga los estudiantes desde estudiantes.txt al vector en memoria
bool BibliotecaDB::cargarEstudiantes() {
    estudiantes.clear(); // Limpia el vector antes de cargar
    std::ifstream file("estudiantes.txt");
    if (!file.is_open()) return true; // No es error si el archivo no existe
    std::string line;
    // Lee cada línea y procesa los campos
    while (std::getline(file, line)) {
        auto tokens = splitLine(line, ',');
        if (tokens.size() >= 3) {
            try {
                Estudiante e;
                e.id = std::stoi(tokens[0]);
                e.nombre = tokens[1];
                e.grado = tokens[2];
                estudiantes.push_back(e);
            } catch (...) {
                std::cout << "Error al procesar linea en estudiantes.txt: " << line << "\n";
            }
        }
    }
    file.close();
    return true;
}

// Guarda la lista de autores en autores.txt en formato CSV
bool BibliotecaDB::guardarAutores() const {
    std::ofstream file("autores.txt");
    if (!file.is_open()) {
        std::cout << "Error al abrir autores.txt para guardar.\n";
        return false;
    }
    // Escribe cada autor como una línea CSV: id,nombre,nacionalidad
    for (const auto& a : autores) {
        file << a.id << "," << escapeField(a.nombre) << "," << escapeField(a.nacionalidad) << "\n";
    }
    file.close();
    return true;
}

// Carga los autores desde autores.txt al vector en memoria
bool BibliotecaDB::cargarAutores() {
    autores.clear(); // Limpia el vector antes de cargar
    std::ifstream file("autores.txt");
    if (!file.is_open()) return true;
    std::string line;
    // Lee cada línea y procesa los campos
    while (std::getline(file, line)) {
        auto tokens = splitLine(line, ',');
        if (tokens.size() >= 3) {
            try {
                Autor a;
                a.id = std::stoi(tokens[0]);
                a.nombre = tokens[1];
                a.nacionalidad = tokens[2];
                autores.push_back(a);
            } catch (...) {
                std::cout << "Error al procesar linea en autores.txt: " << line << "\n";
            }
        }
    }
    file.close();
    return true;
}

// Guarda la lista de editoriales en editoriales.txt en formato CSV
bool BibliotecaDB::guardarEditoriales() const {
    std::ofstream file("editoriales.txt");
    if (!file.is_open()) {
        std::cout << "Error al abrir editoriales.txt para guardar.\n";
        return false;
    }
    // Escribe cada editorial como una línea CSV: id,nombre
    for (const auto& ed : editoriales) {
        file << ed.id << "," << escapeField(ed.nombre) << "\n";
    }
    file.close();
    return true;
}

// Carga las editoriales desde editoriales.txt al vector en memoria
bool BibliotecaDB::cargarEditoriales() {
    editoriales.clear(); // Limpia el vector antes de cargar
    std::ifstream file("editoriales.txt");
    if (!file.is_open()) return true;
    std::string line;
    // Lee cada línea y procesa los campos
    while (std::getline(file, line)) {
        auto tokens = splitLine(line, ',');
        if (tokens.size() >= 2) {
            try {
                Editorial ed;
                ed.id = std::stoi(tokens[0]);
                ed.nombre = tokens[1];
                editoriales.push_back(ed);
            } catch (...) {
                std::cout << "Error al procesar linea en editoriales.txt: " << line << "\n";
            }
        }
    }
    file.close();
    return true;
}

// Guarda la lista de libros en libros.txt en formato CSV
bool BibliotecaDB::guardarLibros() const {
    std::ofstream file("libros.txt");
    if (!file.is_open()) {
        std::cout << "Error al abrir libros.txt para guardar.\n";
        return false;
    }
    // Escribe cada libro como una línea CSV: id,título,isbn,año,id_autor,id_editorial
    for (const auto& l : libros) {
        file << l.id << "," << escapeField(l.titulo) << "," << escapeField(l.isbn) << "," << l.anio
             << "," << l.id_autor << "," << l.id_editorial << "\n";
    }
    file.close();
    return true;
}

// Carga los libros desde libros.txt al vector en memoria
bool BibliotecaDB::cargarLibros() {
    libros.clear(); // Limpia el vector antes de cargar
    std::ifstream file("libros.txt");
    if (!file.is_open()) return true;
    std::string line;
    // Lee cada línea y procesa los campos
    while (std::getline(file, line)) {
        auto tokens = splitLine(line, ',');
        if (tokens.size() >= 6) {
            try {
                Libro l;
                l.id = std::stoi(tokens[0]);
                l.titulo = tokens[1];
                l.isbn = tokens[2];
                l.anio = std::stoi(tokens[3]);
                l.id_autor = std::stoi(tokens[4]);
                l.id_editorial = std::stoi(tokens[5]);
                libros.push_back(l);
            } catch (...) {
                std::cout << "Error al procesar linea en libros.txt: " << line << "\n";
            }
        }
    }
    file.close();
    return true;
}

// Guarda la lista de préstamos en prestamos.txt en formato CSV
bool BibliotecaDB::guardarPrestamos() const {
    std::ofstream file("prestamos.txt");
    if (!file.is_open()) {
        std::cout << "Error al abrir prestamos.txt para guardar.\n";
        return false;
    }
    // Escribe cada préstamo como una línea CSV: id,id_libro,id_estudiante,fecha_prestamo,fecha_devolucion
    for (const auto& p : prestamos) {
        file << p.id << "," << p.id_libro << "," << p.id_estudiante << ","
             << escapeField(p.fecha_prestamo) << "," << escapeField(p.fecha_devolucion) << "\n";
    }
    file.close();
    return true;
}

// Carga los préstamos desde prestamos.txt al vector en memoria
bool BibliotecaDB::cargarPrestamos() {
    prestamos.clear(); // Limpia el vector antes de cargar
    std::ifstream file("prestamos.txt");
    if (!file.is_open()) return true;
    std::string line;
    // Lee cada línea y procesa los campos
    while (std::getline(file, line)) {
        auto tokens = splitLine(line, ',');
        if (tokens.size() >= 5) {
            try {
                Prestamo p;
                p.id = std::stoi(tokens[0]);
                p.id_libro = std::stoi(tokens[1]);
                p.id_estudiante = std::stoi(tokens[2]);
                p.fecha_prestamo = tokens[3];
                p.fecha_devolucion = tokens[4];
                prestamos.push_back(p);
            } catch (...) {
                std::cout << "Error al procesar linea en prestamos.txt: " << line << "\n";
            }
        }
    }
    file.close();
    return true;
}