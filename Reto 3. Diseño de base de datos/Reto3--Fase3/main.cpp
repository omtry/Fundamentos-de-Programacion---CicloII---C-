#include "Biblioteca.h"
#include <iostream>
#include <limits>
#include <string>
#include <regex>
#include <sstream>

/* Valida que la entrada para opciones de menú sea un número entero no negativo (>= 0).
 * Parámetros:
 *   - valor: Referencia al entero donde se almacenará la entrada.
 * Retorna: true si la entrada es válida, false si es inválida (limpia el buffer en caso de error).
 */
bool leerOpcionMenu(int& valor) {
    std::cin >> valor;
    if (std::cin.fail() || valor < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error: Ingrese un numero entero no negativo.\n";
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

/* Valida que la entrada sea un número entero positivo (> 0) para IDs y años.
 * Parámetros:
 *   - valor: Referencia al entero donde se almacenará la entrada.
 * Retorna: true si la entrada es válida, false si es inválida (limpia el buffer en caso de error).
 */
bool leerEnteroPositivo(int& valor) {
    std::cin >> valor;
    if (std::cin.fail() || valor <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error: Ingrese un numero entero positivo.\n";
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

/* Valida que una entrada de nombre contenga solo letras y espacios.
 * Parámetros:
 *   - mensaje: Texto a mostrar para solicitar la entrada.
 *   - tipo: Nombre del campo (para mensajes de error).
 * Retorna: Cadena válida que cumple con el patrón.
 */
std::string leerNombreValido(const std::string& mensaje, const std::string& tipo) {
    std::string input;
    std::regex patron("^[A-Za-z ]+$");
    while (true) {
        std::cout << mensaje;
        std::getline(std::cin, input);
        if (input.empty()) {
            std::cout << "Error: " << tipo << " no puede estar vacio.\n";
            continue;
        }
        if (std::regex_match(input, patron)) {
            return input;
        } else {
            std::cout << "Error: " << tipo << " solo puede contener letras y espacios.\n";
        }
    }
}

/* Valida que una entrada contenga letras, números y espacios (para grados, nacionalidades, títulos).
 * Parámetros:
 *   - mensaje: Texto a mostrar para solicitar la entrada.
 *   - tipo: Nombre del campo (para mensajes de error).
 * Retorna: Cadena válida que cumple con el patrón.
 */
std::string leerCadenaValida(const std::string& mensaje, const std::string& tipo) {
    std::string input;
    std::regex patron("^[A-Za-z0-9 ]+$");
    while (true) {
        std::cout << mensaje;
        std::getline(std::cin, input);
        if (input.empty()) {
            std::cout << "Error: " << tipo << " no puede estar vacio.\n";
            continue;
        }
        if (std::regex_match(input, patron)) {
            return input;
        } else {
            std::cout << "Error: " << tipo << " solo puede contener letras, espacios y numeros.\n";
        }
    }
}

/* Valida que una entrada de ISBN tenga 10 o 13 dígitos, con guiones opcionales.
 * Parámetros:
 *   - mensaje: Texto a mostrar para solicitar la entrada.
 * Retorna: ISBN válido en el formato especificado.
 */
std::string leerISBN(const std::string& mensaje) {
    std::string input;
    std::regex patron("^[0-9]{10}(-[0-9]{3})?$|^[0-9]{13}$");
    while (true) {
        std::cout << mensaje;
        std::getline(std::cin, input);
        if (std::regex_match(input, patron)) {
            return input;
        } else {
            std::cout << "Error: ISBN debe tener 10 o 13 digitos (guiones opcionales, ej. 1234567890 o 123-456-789-0123).\n";
        }
    }
}

/* Lee y valida una fecha en formato YYYY-MM-DD, usando la fecha actual si se omite.
 * Parámetros:
 *   - mensaje: Texto a mostrar para solicitar la entrada.
 *   - db: Instancia de BibliotecaDB para obtener la fecha actual.
 * Retorna: Fecha válida en formato YYYY-MM-DD.
 */
std::string leerFecha(const std::string& mensaje, BibliotecaDB& db) {
    std::string input;
    std::regex dateRegex("\\d{4}-\\d{2}-\\d{2}");
    while (true) {
        std::cout << mensaje;
        std::getline(std::cin, input);
        if (input.empty()) {
            return db.fechaHoy(); // Usar fecha actual si no se ingresa nada.
        }
        // Validar formato de fecha.
        if (!std::regex_match(input, dateRegex)) {
            std::cout << "Error: Formato de fecha invalido (use YYYY-MM-DD).\n";
            continue;
        }
        int year, month, day;
        char dash1, dash2;
        std::istringstream iss(input);
        iss >> year >> dash1 >> month >> dash2 >> day;
        // Validar componentes de la fecha.
        if (dash1 != '-' || dash2 != '-' || year < 1900 || year > 2025 || month < 1 || month > 12 || day < 1 || day > 31) {
            std::cout << "Error: Fecha invalida (use anios entre 1900 y 2025, meses 1-12, dias 1-31).\n";
            continue;
        }
        // Validar días según el mes.
        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
            std::cout << "Error: Mes " << month << " no puede tener mas de 30 dias.\n";
            continue;
        }
        if (month == 2) {
            bool esBisiesto = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if (day > (esBisiesto ? 29 : 28)) {
                std::cout << "Error: Febrero no puede tener mas de " << (esBisiesto ? 29 : 28) << " dias en " << year << ".\n";
                continue;
            }
        }
        return input;
    }
}

/* Muestra el submenú para gestionar estudiantes, permitiendo operaciones CRUD.
 * Parámetros:
 *   - db: Instancia de BibliotecaDB para acceder a los datos.
 */
void menuEstudiantes(BibliotecaDB& db) {
    while (true) {
        std::cout << "\n--- Menu Estudiantes ---\n"
                  << "1) Agregar estudiante\n"
                  << "2) Listar estudiantes\n"
                  << "3) Buscar estudiante por ID\n"
                  << "4) Actualizar estudiante\n"
                  << "5) Eliminar estudiante\n"
                  << "0) Volver\n"
                  << "Opcion: ";
        int op;
        if (!leerOpcionMenu(op)) continue; // Validar entrada numérica.
        if (op == 0) break; // Salir del submenú.
        switch (op) {
            case 1: {
                Estudiante e;
                e.id = db.nextEstudianteId();
                e.nombre = leerNombreValido("Nombre: ", "Nombre");
                e.grado = leerCadenaValida("Grado: ", "Grado");
                if (db.agregarEstudiante(e)) {
                    std::cout << "Estudiante agregado con ID " << e.id << ".\n";
                } else {
                    std::cout << "Error al agregar estudiante.\n";
                }
                break;
            }
            case 2:
                db.listarEstudiantes();
                break;
            case 3: {
                int id;
                std::cout << "ID: ";
                if (!leerEnteroPositivo(id)) break;
                Estudiante* e = db.buscarEstudiantePorId(id);
                if (e) {
                    std::cout << "ID: " << e->id << " | Nombre: " << e->nombre << " | Grado: " << e->grado << "\n";
                } else {
                    std::cout << "Estudiante no encontrado.\n";
                }
                break;
            }
            case 4: {
                int id;
                std::cout << "ID a actualizar: ";
                if (!leerEnteroPositivo(id)) break;
                if (db.actualizarEstudiante(id)) {
                    std::cout << "Estudiante actualizado.\n";
                } else {
                    std::cout << "Error: Estudiante no encontrado.\n";
                }
                break;
            }
            case 5: {
                int id;
                std::cout << "ID a eliminar: ";
                if (!leerEnteroPositivo(id)) break;
                if (db.eliminarEstudiante(id)) {
                    std::cout << "Estudiante eliminado.\n";
                } else {
                    std::cout << "Error: No se pudo eliminar el estudiante.\n";
                }
                break;
            }
            default:
                std::cout << "Opcion invalida.\n";
        }
    }
}

/* Muestra el submenú para gestionar autores, permitiendo operaciones CRUD.
 * Parámetros:
 *   - db: Instancia de BibliotecaDB para acceder a los datos.
 */
void menuAutores(BibliotecaDB& db) {
    while (true) {
        std::cout << "\n--- Menu Autores ---\n"
                  << "1) Agregar autor\n"
                  << "2) Listar autores\n"
                  << "3) Buscar autor por ID\n"
                  << "4) Actualizar autor\n"
                  << "5) Eliminar autor\n"
                  << "0) Volver\n"
                  << "Opcion: ";
        int op;
        if (!leerOpcionMenu(op)) continue; // Validar entrada numérica.
        if (op == 0) break; // Salir del submenú.
        switch (op) {
            case 1: {
                Autor a;
                a.id = db.nextAutorId();
                a.nombre = leerNombreValido("Nombre: ", "Nombre");
                a.nacionalidad = leerCadenaValida("Nacionalidad: ", "Nacionalidad");
                if (db.agregarAutor(a)) {
                    std::cout << "Autor agregado con ID " << a.id << ".\n";
                } else {
                    std::cout << "Error al agregar autor.\n";
                }
                break;
            }
            case 2:
                db.listarAutores();
                break;
            case 3: {
                int id;
                std::cout << "ID: ";
                if (!leerEnteroPositivo(id)) break;
                Autor* a = db.buscarAutorPorId(id);
                if (a) {
                    std::cout << "ID: " << a->id << " | Nombre: " << a->nombre << " | Nacionalidad: " << a->nacionalidad << "\n";
                } else {
                    std::cout << "Autor no encontrado.\n";
                }
                break;
            }
            case 4: {
                int id;
                std::cout << "ID a actualizar: ";
                if (!leerEnteroPositivo(id)) break;
                if (db.actualizarAutor(id)) {
                    std::cout << "Autor actualizado.\n";
                } else {
                    std::cout << "Error: Autor no encontrado.\n";
                }
                break;
            }
            case 5: {
                int id;
                std::cout << "ID a eliminar: ";
                if (!leerEnteroPositivo(id)) break;
                if (db.eliminarAutor(id)) {
                    std::cout << "Autor eliminado.\n";
                } else {
                    std::cout << "Error: No se pudo eliminar el autor.\n";
                }
                break;
            }
            default:
                std::cout << "Opcion invalida.\n";
        }
    }
}

/* Muestra el submenú para gestionar editoriales, permitiendo operaciones CRUD.
 * Parámetros:
 *   - db: Instancia de BibliotecaDB para acceder a los datos.
 */
void menuEditoriales(BibliotecaDB& db) {
    while (true) {
        std::cout << "\n--- Menu Editoriales ---\n"
                  << "1) Agregar editorial\n"
                  << "2) Listar editoriales\n"
                  << "3) Buscar editorial por ID\n"
                  << "4) Actualizar editorial\n"
                  << "5) Eliminar editorial\n"
                  << "0) Volver\n"
                  << "Opcion: ";
        int op;
        if (!leerOpcionMenu(op)) continue; // Validar entrada numérica.
        if (op == 0) break; // Salir del submenú.
        switch (op) {
            case 1: {
                Editorial ed;
                ed.id = db.nextEditorialId();
                ed.nombre = leerCadenaValida("Nombre: ", "Nombre");
                if (db.agregarEditorial(ed)) {
                    std::cout << "Editorial agregada con ID " << ed.id << ".\n";
                } else {
                    std::cout << "Error al agregar editorial.\n";
                }
                break;
            }
            case 2:
                db.listarEditoriales();
                break;
            case 3: {
                int id;
                std::cout << "ID: ";
                if (!leerEnteroPositivo(id)) break;
                Editorial* ed = db.buscarEditorialPorId(id);
                if (ed) {
                    std::cout << "ID: " << ed->id << " | Nombre: " << ed->nombre << "\n";
                } else {
                    std::cout << "Editorial no encontrada.\n";
                }
                break;
            }
            case 4: {
                int id;
                std::cout << "ID a actualizar: ";
                if (!leerEnteroPositivo(id)) break;
                if (db.actualizarEditorial(id)) {
                    std::cout << "Editorial actualizada.\n";
                } else {
                    std::cout << "Error: Editorial no encontrada.\n";
                }
                break;
            }
            case 5: {
                int id;
                std::cout << "ID a eliminar: ";
                if (!leerEnteroPositivo(id)) break;
                if (db.eliminarEditorial(id)) {
                    std::cout << "Editorial eliminada.\n";
                } else {
                    std::cout << "Error: No se pudo eliminar la editorial.\n";
                }
                break;
            }
            default:
                std::cout << "Opcion invalida.\n";
        }
    }
}

/* Muestra el submenú para gestionar libros, permitiendo operaciones CRUD.
 * Parámetros:
 *   - db: Instancia de BibliotecaDB para acceder a los datos.
 */
void menuLibros(BibliotecaDB& db) {
    while (true) {
        std::cout << "\n--- Menu Libros ---\n"
                  << "1) Agregar libro\n"
                  << "2) Listar libros\n"
                  << "3) Buscar libro por ID\n"
                  << "4) Actualizar libro\n"
                  << "5) Eliminar libro\n"
                  << "0) Volver\n"
                  << "Opcion: ";
        int op;
        if (!leerOpcionMenu(op)) continue; // Validar entrada numérica.
        if (op == 0) break; // Salir del submenú.
        switch (op) {
            case 1: {
                Libro l;
                l.id = db.nextLibroId(); // Generar ID único.
                l.titulo = leerCadenaValida("Titulo: ", "Titulo");
                l.isbn = leerISBN("ISBN: ");
                std::cout << "Anio de publicacion (1900-2025): ";
                if (!leerEnteroPositivo(l.anio)) break;
                // Validar rango de año.
                if (l.anio < 1900 || l.anio > 2025) {
                    std::cout << "Error: Anio debe estar entre 1900 y 2025.\n";
                    break;
                }
                std::cout << "ID Autor: ";
                if (!leerEnteroPositivo(l.id_autor)) break;
                std::cout << "ID Editorial: ";
                if (!leerEnteroPositivo(l.id_editorial)) break;
                if (db.agregarLibro(l)) {
                    std::cout << "Libro agregado con ID " << l.id << ".\n";
                } else {
                    std::cout << "Error al agregar libro.\n";
                }
                break;
            }
            case 2:
                db.listarLibros();
                break;
            case 3: {
                int id;
                std::cout << "ID: ";
                if (!leerEnteroPositivo(id)) break;
                Libro* l = db.buscarLibroPorId(id);
                if (l) {
                    Autor* a = db.buscarAutorPorId(l->id_autor);
                    Editorial* ed = db.buscarEditorialPorId(l->id_editorial);
                    std::cout << "ID: " << l->id << " | Titulo: " << l->titulo << " | ISBN: " << l->isbn
                              << " | Anio: " << l->anio << " | Autor: " << (a ? a->nombre : "Desconocido")
                              << " | Editorial: " << (ed ? ed->nombre : "Desconocida") << "\n";
                } else {
                    std::cout << "Libro no encontrado.\n";
                }
                break;
            }
            case 4: {
                int id;
                std::cout << "ID a actualizar: ";
                if (!leerEnteroPositivo(id)) break;
                if (db.actualizarLibro(id)) {
                    std::cout << "Libro actualizado.\n";
                } else {
                    std::cout << "Error: Libro no encontrado o error al actualizar.\n";
                }
                break;
            }
            case 5: {
                int id;
                std::cout << "ID a eliminar: ";
                if (!leerEnteroPositivo(id)) break;
                if (db.eliminarLibro(id)) {
                    std::cout << "Libro eliminado.\n";
                } else {
                    std::cout << "Error: No se pudo eliminar el libro.\n";
                }
                break;
            }
            default:
                std::cout << "Opcion invalida.\n";
        }
    }
}

/* Muestra el submenú para gestionar préstamos, permitiendo operaciones de préstamo, devolución y consultas.
 * Parámetros:
 *   - db: Instancia de BibliotecaDB para acceder a los datos.
 */
void menuPrestamos(BibliotecaDB& db) {
    while (true) {
        std::cout << "\n--- Menu Prestamos ---\n"
                  << "1) Prestar libro\n"
                  << "2) Devolver libro\n"
                  << "3) Listar todos los prestamos\n"
                  << "4) Listar prestamos activos\n"
                  << "5) Buscar prestamo por ID\n"
                  << "6) Listar prestamos por estudiante\n"
                  << "0) Volver\n"
                  << "Opcion: ";
        int op;
        if (!leerOpcionMenu(op)) continue; // Validar entrada numérica.
        if (op == 0) break; // Salir del submenú.
        switch (op) {
            case 1: {
                int idlib, idest;
                std::cout << "ID Libro: ";
                if (!leerEnteroPositivo(idlib)) break;
                std::cout << "ID Estudiante: ";
                if (!leerEnteroPositivo(idest)) break;
                std::string fecha = db.fechaHoy();
                fecha = leerFecha("Fecha de prestamo (" + fecha + "): ", db);
                if (db.prestarLibro(idlib, idest, fecha)) {
                    std::cout << "Prestamo registrado.\n";
                } else {
                    std::cout << "Error al registrar prestamo.\n";
                }
                break;
            }
            case 2: {
                int idp;
                std::cout << "ID Prestamo: ";
                if (!leerEnteroPositivo(idp)) break;
                if (db.devolverPrestamo(idp)) {
                    std::cout << "Libro devuelto.\n";
                } else {
                    std::cout << "Error al devolver el libro prestado.\n";
                }
                break;
            }
            case 3:
                db.listarPrestamos(false);
                break;
            case 4:
                db.listarPrestamos(true);
                break;
            case 5: {
                int idp;
                std::cout << "ID Prestamo: ";
                if (!leerEnteroPositivo(idp)) break;
                Prestamo* p = db.buscarPrestamoPorId(idp);
                if (p) {
                    Libro* l = db.buscarLibroPorId(p->id_libro);
                    Estudiante* e = db.buscarEstudiantePorId(p->id_estudiante);
                    std::cout << "ID Prestamo: " << p->id << " | Libro: " << (l ? l->titulo : "Desconocido")
                              << " | Estudiante: " << (e ? e->nombre : "Desconocido")
                              << " | Fecha Prestamo: " << p->fecha_prestamo
                              << " | Fecha Devolucion: " << (p->fecha_devolucion.empty() ? "(Pendiente)" : p->fecha_devolucion) << "\n";
                } else {
                    std::cout << "Prestamo no encontrado.\n";
                }
                break;
            }
            case 6: {
                int id;
                std::cout << "ID Estudiante: ";
                if (!leerEnteroPositivo(id)) break;
                db.listarPrestamosPorEstudiante(id);
                break;
            }
            default:
                std::cout << "Opcion invalida.\n";
        }
    }
}

/* Punto de entrada del sistema de gestión de biblioteca.
 * Inicializa la base de datos, carga datos desde archivos y muestra el menú principal.
 */
int main() {
    BibliotecaDB db;
    db.cargarDatos(); // Cargar datos iniciales desde archivos.
    std::cout << "Bienvenido al Sistema de Gestion de Biblioteca\n";

    while (true) {
        std::cout << "\n======= Menu Principal =======\n"
                  << "1) Gestion de Estudiantes\n"
                  << "2) Gestion de Autores\n"
                  << "3) Gestion de Editoriales\n"
                  << "4) Gestion de Libros\n"
                  << "5) Gestion de Prestamos\n"
                  << "6) Guardar datos\n"
                  << "7) Cargar datos\n"
                  << "0) Salir\n"
                  << "Opcion: ";
        int op;
        if (!leerOpcionMenu(op)) continue; // Validar entrada numérica.
        if (op == 0) {
            db.guardarDatos(); // Guardar datos antes de salir.
            std::cout << "Datos guardados. Saliendo...\n";
            break;
        }
        switch (op) {
            case 1:
                menuEstudiantes(db);
                break;
            case 2:
                menuAutores(db);
                break;
            case 3:
                menuEditoriales(db);
                break;
            case 4:
                menuLibros(db);
                break;
            case 5:
                menuPrestamos(db);
                break;
            case 6:
                db.guardarDatos();
                std::cout << "Datos guardados.\n";
                break;
            case 7:
                db.cargarDatos();
                std::cout << "Datos cargados.\n";
                break;
            default:
                std::cout << "Opcion invalida.\n";
        }
    }
    return 0;
}