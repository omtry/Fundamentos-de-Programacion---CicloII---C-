# Fase 1 - Diagrama ER: Biblioteca Escolar  

Este diagrama representa como quiero presentar de manera conceptual el sistema de gestion de biblioteca.  

- **Entidades**: Autor, Libro, Estudiante, Préstamo.  
- **Relaciones**:  
  - Un Autor puede escribir un  numero ilimitado de Libros (1:N).  
  - Un Libro puede ser prestado infinita cantidad de veces (1:N).  
  - Un Estudiante puede realizar la cantidad de prestamos que el desee (1:N).  

> Nota Importante: El atributo `Fecha_devolucion` en Préstamo es opcional, por que puede ser que un libro no haya sido devuelto aun