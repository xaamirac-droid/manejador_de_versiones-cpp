# Sistema de Control de Versiones Jerárquico (C++)

Aplicación de consola en C++ que simula un sistema de control de versiones de archivos de texto, inspirado en la lógica de versionado jerárquico (similar al esquema de números de versión tipo 1, 1.1, 1.2, 2...). Permite crear versiones y subversiones de un archivo, editar su contenido línea por línea, y llevar un registro de cambios.

## Descripción

El programa permite:
- Crear un archivo y generar versiones raíz y subversiones anidadas (hasta 10 niveles de profundidad).
- Insertar y eliminar líneas de texto dentro de una versión específica.
- Comparar el contenido de dos versiones para determinar si son iguales.
- Mostrar el árbol completo de versiones de un archivo.
- Mostrar el texto acumulado de una versión, incluyendo el heredado de sus versiones padre.
- Consultar el historial de cambios (inserciones y borrados de línea) de cada versión.
- Eliminar versiones específicas del árbol.

## Características técnicas

- **Estructura de árbol N-ario** implementada con la técnica "primer hijo / siguiente hermano" (`primerHijo`, `siguienteHermano`, `padre`), que permite representar un número variable de hijos por nodo usando solo tres punteros.
- **Memoria dinámica**: creación y destrucción de nodos con `new`/`delete`, incluyendo liberación recursiva de subárboles completos.
- **Recursividad extensiva**: recorrido e impresión del árbol, búsqueda de versiones, destrucción de subárboles y despliegue de texto heredado de versiones padre, todo resuelto con funciones recursivas.
- **Parsing manual de strings**: conversión de un identificador de versión en formato texto (ej. "1.2.3") a un arreglo de niveles numéricos, sin usar librerías de parsing.
- **Validación de integridad del árbol**: control de que no existan "huecos" al crear versiones (no se puede crear la versión 3 sin que existan la 1 y la 2), con desplazamiento automático de versiones existentes cuando corresponde.
- **Registro de cambios (changelog)** por versión, guardando cada inserción (`IL`) o borrado (`BL`) de línea con su detalle.


## Cómo compilarlo y ejecutarlo

1. Cloná o descargá el repositorio.
  
2. Compilá con g++ (o el compilador de C++ que tengas):
g++ lab_final_EDA-Xaamira.cpp -o control-versiones

3. Ejecutá:
./control-versiones

## Contexto

Proyecto desarrollado en 2025 para el curso de Estructuras de Datos y Algoritmos — Tecnólogo en Informática, UTEC.

## Posibles mejoras

- Reemplazar los arreglos de tamaño fijo (`niveles[10]`, `lineas[100]`, `cambios[200]`) por estructuras dinámicas (`std::vector`) para eliminar los límites máximos.
- Separar la lógica de negocio de la interfaz de consola (actualmente todo vive en `main`).
- Agregar manejo de excepciones en lugar de códigos de retorno (`TipoRet`) para el control de errores.
