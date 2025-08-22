//Clase:        Programacion Orientada a Objetos
//Tema:         Gestion de una Biblioteca
//Ejercicio:    Reto 1 -- Creacion de Biblioteca
//Descripción:  Crear un sistema simple para gestionar el inventario de una biblioteca. 
//El programa deberá permitir agregar nuevos libros, buscar libros existentes, prestarlos y devolverlos. 

//Autor:        Gerardo Andre Calderon Castilo
//Fecha:        2025-08-25
//Estado:       [ Terminado ]


#include <iostream>
#include <string>
#include <vector>

class Libro { 
public:
    std::string titulo; // Define que la variable titulo va a ser una variable de tipo string
    std::string autor; // Define que la variable autor va a ser una variable de tipo string
    int anioPublicacion; // Define que la variable Año de Publicacion va a ser una variable de tipo entero
    bool estaDisponible; // Define que la variable Esta siponible va a ser una variable de tipo booleano
    Libro(std::string t="", std::string a="", int anio=0) // Esta linea crea una estructura mas sencilla para agregar los libros, y en caso no se ingrese un dato, se tiene un valor predeterminado (Asesoria de Chat GPT)
    : titulo(t), autor(a), anioPublicacion(anio), estaDisponible(true) {} // Estableces que para cada argumento de la clase libro se usen esos valores predeterminados en caso el usuario no lo ingrese

    void mostrarDetalles() {
        std::cout <<""<< std::endl;
        std::cout <<"Titulo: "<< titulo << std::endl; // Estableces que se muestre el titulo del libro
        std::cout <<"Autor: "<< autor << std::endl; // Estableces que se nuestre el autor del libro
        std::cout <<"Año de Publicación: "<< anioPublicacion << std::endl; // Estableces que se muestre el año de publicacion
        std::cout <<"El Libro está Disponible: " << (estaDisponible ? "Sí" : "No") << std::endl; // Dependiendo sea True or False en terminal se muestra Si o No
        std::cout <<""<<std::endl;
        std::cout <<"-----------------------------------"<< std::endl; 
    };
};

class Biblioteca {

private:
    std::vector<Libro> libros; // De forma privada creamos una coleccion dinamica de libros, es mas eficiente para almacenar informacion

public:

    void agregarLibro(const Libro& nuevoLibro) {

         if (buscarLibro(nuevoLibro.titulo)) { // Implementamos una funcion que recorre el vector libros en busca de si ya existe ese titulo, si existe devuelve un puntero y si no un nullptr
        std::cout << "--- El libro '" << nuevoLibro.titulo << "' ya existe en la biblioteca, ingresa otro libro---\n";
        return;
    }

        libros.push_back(nuevoLibro); // En esta linea se programa que se incluya un nuevo libro al final del vector Libro (Asesoria de Chat GPT)
} 

    void mostrarInventario() {
    std::cout << "\n-----Inventario de la Biblioteca-------\n";
    for (auto& libro : libros) { // Aun no hemos aprendido sobre bucles, le pedi a Copilot que me ayudara,
        // por que sentia que era ineficiente estar nombrando cada libro en una linea, y con ese bucle se recorre cada libro en libros
        libro.mostrarDetalles(); // Se manda a indicar que para cada libro en libros se ejecute mostrar los detalles
    }
}

Libro* buscarLibro(const std::string& tituloBuscado) {
        for (auto& libro : libros) { // Busque como funcionaba el bucle auto&, que recorre cada libro en libros (Me apoye de Chat GPT para darle estructura)
            if (libro.titulo == tituloBuscado) { //Condicion que si el titulo de un libro coincide con el titulo buscado ocurra algo
                return &libro; // Como se encontro el libro, se devuelve un puntero al libro encontrado tal como Ish y Erick indicaron
            }
        }
        return nullptr; // Es lo que ocurre si no se encontro ningun libro con ese titulo, se devuelve un dato vacio o un valor nulo
    }

void prestarLibro(const std::string& tituloPrestamo) {
        Libro* libro = buscarLibro(tituloPrestamo); // Primero busca el titulo del libro dentro de la coleccion (Me apoye de Copilot para darle estructura)
        if (libro == nullptr) {
            std::cout << "--- El libro '" << tituloPrestamo << "' no existe en la coleccion de libros ---\n"; // Si no encuentra el libro devuelve que el libro no existe
        } else if (!libro->estaDisponible) {
            std::cout << "--- El libro '" << tituloPrestamo << "' ya está prestado ---\n"; // La segunda condicion es que si el libro existe pero no esta disponible, muestre un mensaje
        } else {
            libro->estaDisponible = false; // Como se presto el libro se cambia su estado a no estar disponible
            std::cout << "--- Has prestado el libro '" << tituloPrestamo << "' ---\n"; // Si pasa las dos condiciones anteriores se permite que se haga el prestamo
        }
    }

    void devolverLibro(const std::string& tituloDevolucion) {
        Libro* libro = buscarLibro(tituloDevolucion); // Again el programa al devolver busca si el titulo ingresado existe (Me apoye de Copilot para darle estructura)
        if (libro == nullptr) {
            std::cout << "--- El libro '" << tituloDevolucion << "' no existe en la biblioteca ---\n"; // Si no existe el libro devuelve el mensaje correspondiente
        } else if (libro->estaDisponible) {
            std::cout << "--- El libro '" << tituloDevolucion << "' ya estaba disponible ---\n"; // Por el contrario de cuando se presta lo que se verifica es si ya se cambio el estado del libro a disponible
        } else {
            libro->estaDisponible = true;
          std::cout << "--- Has devuelto el libro '" << tituloDevolucion << "' ---\n"; // Si se pasaron las dos condiciones se devuelve el estado del libro que se qeire devolver a disponible
        }
    }
};



int main() {

    Biblioteca biblioteca;
    int opcion = 0;
   
    biblioteca.agregarLibro(Libro("Cien anos de soledad", "Gabriel Garcia Marquez", 1967));
    biblioteca.agregarLibro(Libro("El Principito", "Antoine de Saint-Exupery", 1943));
    biblioteca.agregarLibro(Libro("Don Quijote de la Mancha", "Miguel de Cervantes", 1605));
    biblioteca.agregarLibro(Libro("Moby Dick", "Herman Melville", 1851));
    biblioteca.agregarLibro(Libro("La Odisea", "Homero", 800));
    biblioteca.agregarLibro(Libro("Orgullo y Prejuicio", "Jane Austen", 1813)); // Segun el formato que indicamos arriba, agregamos algunos libros como ejemplo


 while (opcion != 5) {
        std::cout << "\n--- Bienvenido a la Biblioteca ---\n";
        std::cout << "1. Añadir libro\n";
        std::cout << "2. Mostrar Inventario\n";
        std::cout << "3. Prestar libro\n";
        std::cout << "4. Devolver libro\n";
        std::cout << "5. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion; // El cin es el input de c++ donde el valor que ingresa el usuario es lo que se le asigna a la variable

        std::cin.ignore(); // Asi limpiamos el buffer para futuras lecturas de texto

        if (opcion == 1) {
            std::string titulo, autor; // Establecemos que tanto titulo como autor son strings
            int anio; // Definimos al año como un entero
            std::cout << "Ingrese el título: ";
            std::getline(std::cin, titulo); // Se le pide al susuario que escriba el titulo del libro, con getline se lee toda la linea con espacios y el cin es lo que el usuario ingresa
            std::cout << "Ingrese el autor: ";
            std::getline(std::cin, autor); // Se le pide al susuario que escriba el autor del libro, con getline se lee toda la linea con espacios y el cin es lo que el usuario ingresa
            std::cout << "Ingrese el año de publicación: ";
            std::cin >> anio; //Se le pide al susuario que escriba el año de publicacion del libro, con getline se lee toda la linea con espacios y el cin es lo que el usuario ingresa
            std::cin.ignore();
            biblioteca.agregarLibro(Libro(titulo, autor, anio)); // Al haber rellenado los datos necesarios, se agrega el libro a la biblioteca con el formato indicado
        }
        else if (opcion == 2) {
            biblioteca.mostrarInventario(); // Se muestra directamente el inventario
        }
        else if (opcion == 3) {
            std::string titulo;
            std::cout << "Ingrese el título del libro a prestar: ";
            std::getline(std::cin, titulo); // Se le pide al susuario que escriba el titulo del libro que desea prestar, con getline se lee toda la linea con espacios y el cin es lo que el usuario ingresa
            biblioteca.prestarLibro(titulo); // Se manda a llamar el metodo de prestar, para ver si el libro existe o esta disponible para prestamo
        }
        else if (opcion == 4) {
            std::string titulo;
            std::cout << "Ingrese el título del libro a devolver: ";
            std::getline(std::cin, titulo); // Se le pide al susuario que escriba el titulo del libro que desea devolver
            biblioteca.devolverLibro(titulo);  // Se manda a llamar el metodo de devolver, para ver si el libro existe, esta disponible o si esta prestado y se puede devolver
        }
        else if (opcion == 5) {
            std::cout << "¡Vuelve pronto a la biblioteca de Andre!\n"; // Con esta linea se le da fin al porgrama.
        }
        else {
            std::cout << "Opción no válida. Igrese un numero valido.\n"; // En caso el usuario ingrese un numero o letra que no esta en las opciones para que no crashee el programa
        }
    }

    return 0;
}

