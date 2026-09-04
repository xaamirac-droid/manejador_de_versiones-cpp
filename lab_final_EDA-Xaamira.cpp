#include <iostream>
#include <string>

using namespace std;

// Tipos de retorno
typedef enum _retorno {
    OK,
    ERROR,
    NO_IMPLEMENTADA
} TipoRet;

// Estructura version
struct Version {
    int niveles[10];
    int profundidad;
    Version* padre;
    Version* primerHijo;
    Version* siguienteHermano;
    string lineas[100];  // hasta 100 líneas de texto
    string cambios[200];   // registro de cambios (IL / BL)
    int cantidadCambios = 0;
    int cantidadLineas;  // cantidad actual de líneas usadas
};

// Estructura archivo
struct Archivo {
    string nombreArchivo;
    Version* primeraVersion; // primera raiz (nivel 1). Las demás raíces están en siguienteHermano.
    Version* ultimaVersion;
};


// Crear archivo
Archivo* crearArchivo(string nombre) {
    Archivo* nuevo = new Archivo;
    nuevo->nombreArchivo = nombre;
    nuevo->primeraVersion = nullptr;
    nuevo->ultimaVersion = nullptr;
    cout << "Archivo " << nombre << " creado." << endl;
    return nuevo;
}


void imprimirVersion(Version* v) {
    if (!v) return;
    for (int i = 0; i < v->profundidad; i++) {
        if (i > 0) cout << ".";
        cout << v->niveles[i];
    }
}

void DestruirVersiones(Version* v) {
    if (v == nullptr) return;
    DestruirVersiones(v->primerHijo);
    DestruirVersiones(v->siguienteHermano);
    cout << "Eliminando version ";
    imprimirVersion(v);
    cout << endl;
    delete v;
}

TipoRet destruirArchivo(Archivo* a) {
    if (a == nullptr) return ERROR;
    if (a->primeraVersion != nullptr) DestruirVersiones(a->primeraVersion);
    cout << "Archivo destruido." << endl;
    delete a;
    return OK;
}

Version* crearVersionRaiz(int numero) {
    Version* v = new Version;
    v->niveles[0] = numero;
    v->profundidad = 1;
    v->padre = nullptr;
    v->primerHijo = nullptr;
    v->siguienteHermano = nullptr;
    return v;
}

Version* crearSubversion(Version* padre, int numero) {
    Version* v = new Version;
    for (int i = 0; i < padre->profundidad; i++) v->niveles[i] = padre->niveles[i];
    v->niveles[padre->profundidad] = numero;
    v->profundidad = padre->profundidad + 1;
    v->padre = padre;
    v->primerHijo = nullptr;
    v->siguienteHermano = nullptr;
    return v;
}

// Imprime recursivamente árbol de versiones
void imprimirVersiones(Version* v, int nivel = 0) {
    if (v == nullptr) return;
    cout << string(nivel * 2, ' ');
    imprimirVersion(v);
    cout << endl;
    imprimirVersiones(v->primerHijo, nivel + 1);
    imprimirVersiones(v->siguienteHermano, nivel);
}

void MostrarVersiones(Archivo& a) {
    cout << a.nombreArchivo << endl;
    if (a.primeraVersion == nullptr) {
        cout << "No hay versiones creadas" << endl;
    } else {
        imprimirVersiones(a.primeraVersion);
    }
}

// Busca en una lista de hermanos (raiz) un nodo cuya posición indexLevel del arreglo niveles == valor
Version* buscarEnListaPorNivel(Version* raiz, int indexLevel, int valor) {
    Version* cur = raiz;
    while (cur) {
        if (cur->niveles[indexLevel] == valor) return cur;
        cur = cur->siguienteHermano;
    }
    return nullptr;
}

// Verifica que existan hermanos previos 1..(nuevoNivel-1) en la lista 'raiz' (usado para evitar huecos)
// indexLevel indica qué posición dentro de niveles[] comparar (0 para raíces)
bool hermanosPreviosExistenEnLista(Version* raiz, int nuevoNivel, int indexLevel) {
    if (nuevoNivel <= 1) return true; // no se requiere previos
    for (int i = 1; i < nuevoNivel; ++i) {
        if (buscarEnListaPorNivel(raiz, indexLevel, i) == nullptr) return false;
    }
    return true;
}

// Desplazar (incrementar) el campo niveles[indexLevel] y recursivamente a hijos a partir de un nodo
void desplazarVersionesEnLista(Version* nodo, int indexLevel) {
    Version* cur = nodo;
    while (cur) {
        cur->niveles[indexLevel] += 1;
        // desplazar hijos recursivamente en el siguiente índice
        desplazarVersionesEnLista(cur->primerHijo, indexLevel + 1);
        cur = cur->siguienteHermano;
    }
}

// Insertar 'nueva' al final inmediato después de 'anterior' dentro de la lista de hermanos
void insertarDespues(Version*& raiz, Version* anterior, Version* nueva) {
    if (anterior == nullptr) {
        // insertar al inicio
        nueva->siguienteHermano = raiz;
        raiz = nueva;
    } else {
        nueva->siguienteHermano = anterior->siguienteHermano;
        anterior->siguienteHermano = nueva;
    }
}

// FUNCION PRINCIPAL: crear version (arreglada para raíces y subversiones)
TipoRet CrearVersion(Archivo &a, int niveles[], int profundidad) {
    if (profundidad < 1 || profundidad > 10) return ERROR;

    // Caso: no hay versiones aún -> crear raíz (solo permitido si profundidad==1)
    if (a.primeraVersion == nullptr) {
        if (profundidad != 1) {
            cout << "Error: debe crear primero una version raiz (profundidad 1)." << endl;
            return ERROR;
        }
        a.primeraVersion = crearVersionRaiz(niveles[0]);
        a.ultimaVersion = a.primeraVersion;
        cout << "version raíz creada: ";
        imprimirVersion(a.primeraVersion); cout << endl;
        return OK;
    }

    // Si se crea una raíz (profundidad==1), trabajamos sobre la lista de raíces (a.primeraVersion y sus siguientesHermano)
    if (profundidad == 1) {
        int nuevoNivel = niveles[0];
        // validar que no haya huecos entre 1..nuevoNivel-1 en la lista de raíces
        if (!hermanosPreviosExistenEnLista(a.primeraVersion, nuevoNivel, 0)) {
            cout << "Error: faltan versiones raiz previas para crear ";
            cout << nuevoNivel << endl;
            return ERROR;
        }

        // buscar si ya existe ese numero en las raíces
        Version* actual = a.primeraVersion;
        Version* anterior = nullptr;
        while (actual) {
            if (actual->niveles[0] == nuevoNivel) {
                // existe: desplazar desde actual (incrementar nivel en esta y subsiguientes)
                desplazarVersionesEnLista(actual, 0);
                break;
            }
            anterior = actual;
            actual = actual->siguienteHermano;
        }
        // crear nueva raiz
        Version* nueva = crearVersionRaiz(nuevoNivel);
        // insertar en lista de raíces (raiz = a.primeraVersion)
        insertarDespues(a.primeraVersion, anterior, nueva);
        a.ultimaVersion = nueva;
        cout << "version creada: "; imprimirVersion(nueva); cout << endl;
        return OK;
    }

    // Caso: profundidad > 1 -> localizar padre navegando desde las raíces
    // padre será el nodo con profundidad = profundidad-1 y niveles iguales en las primeras (profundidad-1) posiciones
    Version* padre = a.primeraVersion;
    bool encontradoPadre = false;

    // Para buscar el padre, necesitamos recorrer niveles desde 0 hasta profundidad-2
    // Empezamos en lista de raíces y vamos bajando
    // Primer: buscar la raíz con niveles[0]
    padre = buscarEnListaPorNivel(a.primeraVersion, 0, niveles[0]);
    if (!padre) {
        cout << "Error: falta crear la raiz " << niveles[0] << endl;
        return ERROR;
    }

    // para cada nivel intermedio
    for (int nivel = 1; nivel < profundidad - 1; ++nivel) {
        Version* hijo = buscarEnListaPorNivel(padre->primerHijo, nivel, niveles[nivel]);
        if (!hijo) {
            cout << "Error: falta crear el padre ";
            for (int j = 0; j <= nivel; ++j) {
                if (j) cout << ".";
                cout << niveles[j];
            }
            cout << endl;
            return ERROR;
        }
        padre = hijo;
    }

    int nuevoNivel = niveles[profundidad - 1];
    // Verificar que no haya huecos en la lista de hijos del padre (indexLevel = profundidad-1)
    if (!hermanosPreviosExistenEnLista(padre->primerHijo, nuevoNivel, profundidad - 1)) {
        cout << "Error: faltan versiones hermanas previas para crear ";
        // mostrar la version objetivo
        Version* tmp = crearSubversion(padre, nuevoNivel);
        imprimirVersion(tmp);
        cout << endl;
        delete tmp; // solo para mostrar
        return ERROR;
    }

    // Buscar si ya existe una version con ese número entre los hijos del padre
    Version* actual = padre->primerHijo;
    Version* anterior = nullptr;
    while (actual) {
        if (actual->niveles[profundidad - 1] == nuevoNivel) {
            // existe: desplazar desde aquí
            desplazarVersionesEnLista(actual, profundidad - 1);
            break;
        }
        anterior = actual;
        actual = actual->siguienteHermano;
    }

    // Crear nueva subversion e insertar
    Version* nueva = crearSubversion(padre, nuevoNivel);
    insertarDespues(padre->primerHijo, anterior, nueva);
    a.ultimaVersion = nueva;

    cout << "version creada: ";
    imprimirVersion(nueva);
    cout << endl;
    return OK;
}


Version* buscarVersionCompleta(Version* raiz, int niveles[], int profundidad) {
    if (!raiz) return nullptr;

    Version* actual = raiz;
    while (actual) {
        // Verifica si esta version coincide con todos los niveles conocidos hasta su profundidad
        bool coincideHastaAhora = true;
        for (int i = 0; i < actual->profundidad && i < profundidad; ++i) {
            if (actual->niveles[i] != niveles[i]) {
                coincideHastaAhora = false;
                break;
            }
        }

        // Si encontramos coincidencia completa y la profundidad también coincide → la version exacta
        if (coincideHastaAhora && actual->profundidad == profundidad)
            return actual;

        // Buscar en los hijos (si los niveles coinciden hasta este punto)
        if (coincideHastaAhora && actual->profundidad < profundidad) {
            Version* encontrada = buscarVersionCompleta(actual->primerHijo, niveles, profundidad);
            if (encontrada)
                return encontrada;
        }

        // Si no coincide, seguir con el siguiente hermano
        actual = actual->siguienteHermano;
    }

    return nullptr;
}



bool destruirVersionEspecifica(Version*& raiz, int niveles[], int profundidad) {
    if (!raiz) return false;

    Version* actual = raiz;
    Version* anterior = nullptr;

    while (actual) {
        // Comparar si esta version es la buscada
        if (actual->profundidad == profundidad) {
            bool igual = true;
            for (int i = 0; i < profundidad; ++i) {
                if (actual->niveles[i] != niveles[i]) {
                    igual = false;
                    break;
                }
            }

            if (igual) {
                // Desconectar y eliminar la version encontrada
                if (anterior == nullptr)
                    raiz = actual->siguienteHermano;
                else
                    anterior->siguienteHermano = actual->siguienteHermano;

                DestruirVersiones(actual);
                return true;
            }
        }

        // Buscar recursivamente en los hijos del nodo actual
        if (destruirVersionEspecifica(actual->primerHijo, niveles, profundidad))
            return true;

        anterior = actual;
        actual = actual->siguienteHermano;
    }

    return false;
}

Version* transformarVersion(Version* raiz, string numVersion) {
    int niveles[10];
    int profundidad = 0;
    int numero = 0;

    for (int i = 0; i < numVersion.length(); i++) { //length refiere al tamaño
        char c = numVersion[i];
        if (c == '.') {
            niveles[profundidad] = numero;
            profundidad++;
            numero = 0; // reiniciar el número para el siguiente nivel
        } else if (c >= '0' && c <= '9') {
            numero = numero * 10 + (c - '0'); // construir número a mano
        }
    }

    // guardar el último número
    niveles[profundidad] = numero;
    profundidad++;

    return buscarVersionCompleta(raiz, niveles, profundidad);
}

TipoRet InsertarLinea(Archivo& a, int niveles[], int profundidad, string texto, int nroLinea) {
    Version* v = buscarVersionCompleta(a.primeraVersion, niveles, profundidad);
    if (v == nullptr)
        return ERROR;

    if (v->primerHijo != nullptr)
        return ERROR;

    if (nroLinea < 1 || nroLinea > v->cantidadLineas + 1)
        return ERROR;

    // mover las líneas hacia abajo
    for (int i = v->cantidadLineas; i >= nroLinea; i--) {
        v->lineas[i] = v->lineas[i - 1];
    }

    // insertar nueva línea
    v->lineas[nroLinea - 1] = texto;
    v->cantidadLineas++;

    // --- registrar cambio ---
    if (v->cantidadCambios < 200) {
        v->cambios[v->cantidadCambios++] = "IL " + to_string(nroLinea) + " " + texto; // to_string para convertir int a string (texto)
    }

    return OK;
}

TipoRet Iguales(Archivo& a, int niveles1[], int niveles2[], int profundidad1, int profundidad2, bool &iguales) {
    // Buscar ambas versiones
    Version* v1 = buscarVersionCompleta(a.primeraVersion, niveles1, profundidad1);
    Version* v2 = buscarVersionCompleta(a.primeraVersion, niveles2, profundidad2);

    // Si alguna versión no existe, devolvemos ERROR
    if (v1 == nullptr || v2 == nullptr) {
        iguales = false;
        return ERROR;
    }

    // Comparar cantidad de líneas
    if (v1->cantidadLineas != v2->cantidadLineas) {
        iguales = false;
        return OK;
    }

    // Comparar línea por línea
    for (int i = 0; i < v1->cantidadLineas; i++) {
        if (v1->lineas[i] != v2->lineas[i]) {
            iguales = false;
            return OK;
        }
    }

    // Si no hay diferencias, son iguales
    iguales = true;
    return OK;
}




TipoRet DestruirLinea(Archivo& a, int niveles[], int profundidad, int nroLinea) {
    Version* v = buscarVersionCompleta(a.primeraVersion, niveles, profundidad);
    if (v == nullptr)
        return ERROR;

    if (v->primerHijo != nullptr)
        return ERROR;

    if (nroLinea < 1 || nroLinea > v->cantidadLineas)
        return ERROR;

    // mover líneas hacia arriba
    for (int i = nroLinea - 1; i < v->cantidadLineas - 1; i++) {
        v->lineas[i] = v->lineas[i + 1];
    }

    // limpiar última línea (opcional)
    v->lineas[v->cantidadLineas - 1] = "";
    v->cantidadLineas--;

    // --- registrar cambio ---
    if (v->cantidadCambios < 200) {
        v->cambios[v->cantidadCambios++] = "BL " + to_string(nroLinea); // to_string para convertir int a string (texto)
    }

    return OK;
}



void MostrarCambios(Archivo& a, int niveles[], int profundidad) {
    Version* v = buscarVersionCompleta(a.primeraVersion, niveles, profundidad);
    if (v == nullptr) {
        cout << "ERROR: versión no encontrada." << endl;
        return;
    }

    cout << a.nombreArchivo << " - ";
    for (int i = 0; i < profundidad; i++) {
        cout << v->niveles[i];
        if (i < profundidad - 1) cout << ".";
    }
    cout << endl;

    if (v->cantidadCambios == 0) {
        cout << "  No se realizaron modificaciones" << endl;
        return;
    }

    for (int i = 0; i < v->cantidadCambios; i++) {
        cout << "  " << v->cambios[i] << endl;
    }
}



void MostrarTextoRecursivo(Version* v) {
    if (v == nullptr) return;

    // Mostrar primero el texto del padre
    MostrarTextoRecursivo(v->padre);

    // Encabezado de esta versión
    cout << endl << "(Version ";
    for (int i = 0; i < v->profundidad; i++) {
        cout << v->niveles[i];
        if (i < v->profundidad - 1)
            cout << ".";
    }
    cout << ")" << endl;

    // Mostrar líneas de esta versión
    for (int i = 0; i < v->cantidadLineas; i++) {
        cout << (i + 1) << " " << v->lineas[i] << endl;
    }
}

void MostrarTexto(Archivo& a, int niveles[], int profundidad) {
    Version* v = buscarVersionCompleta(a.primeraVersion, niveles, profundidad);
    if (v == nullptr) {
        cout << "version no encontrada." << endl;
        return;
    }

    // Mostrar encabezado principal
    cout << a.nombreArchivo << " - ";
    for (int i = 0; i < profundidad; i++) {
        cout << v->niveles[i];
        if (i < profundidad - 1)
            cout << ".";
    }
    cout << endl << endl;

    // Mostrar recursivamente el texto de los padres y de sí misma
    MostrarTextoRecursivo(v);
}


int main() {
    Archivo* archivo = nullptr;
    int opcion;

    do {
        cout << "\n--- Gestor de Versiones ---\n";
        cout << "1. Crear archivo\n";
        cout << "2. Crear version\n";
        cout << "3. Insertar línea en version\n";
        cout << "4. Mostrar texto de version\n";
        cout << "5. Mostrar versiones\n";
        cout << "6. Mostrar cambios de version\n";  
        cout << "7. Destruir línea en version\n";
        cout << "8. Eliminar version específica\n";
        cout << "9. Destruir archivo\n";
        cout << "10.Comparar dos versiones\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                string nombre;
                cout << "Ingrese nombre del archivo: ";
                cin >> nombre;
                archivo = crearArchivo(nombre);
                break;
            }

            case 2: {
                if (!archivo) {
                    cout << "Primero debe crear un archivo.\n";
                    break;
                }

                int profundidad;
                cout << "Ingrese profundidad de la version (1 a 10): ";
                cin >> profundidad;

                if (profundidad < 1 || profundidad > 10) {
                    cout << "Profundidad inválida.\n";
                    break;
                }

                int niveles[10];
                cout << "Ingrese los niveles separados por espacio: ";
                for (int i = 0; i < profundidad; ++i)
                    cin >> niveles[i];

                CrearVersion(*archivo, niveles, profundidad);
                break;
            }

            case 3: {  // Insertar línea
                if (!archivo) {
                    cout << "Primero debe crear un archivo.\n";
                    break;
                }

                int profundidad, nroLinea;
                int niveles[10];
                string texto;

                cout << "Ingrese profundidad de la version: ";
                cin >> profundidad;
                cout << "Ingrese los niveles separados por espacio: ";
                for (int i = 0; i < profundidad; ++i)
                    cin >> niveles[i];
                cin.ignore(); // limpiar buffer

                cout << "Ingrese el texto a insertar: ";
                getline(cin, texto);
                cout << "Ingrese número de línea donde insertar: ";
                cin >> nroLinea;

                TipoRet resultado = InsertarLinea(*archivo, niveles, profundidad, texto, nroLinea);
                if (resultado == OK)
                    cout << "Línea insertada correctamente.\n";
                else
                    cout << "Error al insertar línea.\n";
                break;
            }

            case 4: {  // Mostrar texto
                if (!archivo) {
                    cout << "Primero debe crear un archivo.\n";
                    break;
                }

                int profundidad;
                int niveles[10];

                cout << "Ingrese profundidad de la version: ";
                cin >> profundidad;
                cout << "Ingrese los niveles separados por espacio: ";
                for (int i = 0; i < profundidad; ++i)
                    cin >> niveles[i];

                MostrarTexto(*archivo, niveles, profundidad);
                break;
            }

            case 5: {
                if (!archivo) {
                    cout << "Primero debe crear un archivo.\n";
                    break;
                }
                MostrarVersiones(*archivo);
                break;
            }

            case 6: {  // Mostrar cambios
                if (!archivo) {
                    cout << "Primero debe crear un archivo.\n";
                    break;
                }

                int profundidad;
                int niveles[10];

                cout << "Ingrese profundidad de la version: ";
                cin >> profundidad;
                cout << "Ingrese los niveles separados por espacio: ";
                for (int i = 0; i < profundidad; ++i)
                    cin >> niveles[i];

                MostrarCambios(*archivo, niveles, profundidad);
                break;
            }

            case 7: {  // Destruir línea
                if (!archivo) {
                    cout << "Primero debe crear un archivo.\n";
                    break;
                }

                int profundidad, nroLinea;
                int niveles[10];

                cout << "Ingrese profundidad de la version: ";
                cin >> profundidad;
                cout << "Ingrese los niveles separados por espacio: ";
                for (int i = 0; i < profundidad; ++i)
                    cin >> niveles[i];

                cout << "Ingrese número de línea a destruir: ";
                cin >> nroLinea;

                TipoRet resultado = DestruirLinea(*archivo, niveles, profundidad, nroLinea);
                if (resultado == OK)
                    cout << "Línea destruida correctamente.\n";
                else
                    cout << "Error al destruir línea.\n";
                break;
            }

            case 8: {
                if (!archivo) {
                    cout << "Primero debe crear un archivo.\n";
                    break;
                }

                int profundidad;
                cout << "Ingrese profundidad de la version a eliminar: ";
                cin >> profundidad;

                int niveles[10];
                cout << "Ingrese los niveles separados por espacio: ";
                for (int i = 0; i < profundidad; ++i)
                    cin >> niveles[i];

                bool eliminada = destruirVersionEspecifica(archivo->primeraVersion, niveles, profundidad);
                if (eliminada)
                    cout << "version eliminada correctamente.\n";
                else
                    cout << "No se encontró la version.\n";
                break;
            }

            case 9: {
                if (!archivo) {
                    cout << "No hay archivo para destruir.\n";
                    break;
                }
                destruirArchivo(archivo);
                archivo = nullptr;
                cout << "Archivo destruido correctamente.\n";
                break;
            }

            case 10: {  // Comparar dos versiones
                if (!archivo) {
                    cout << "Primero debe crear un archivo.\n";
                    break;
                }

                int profundidad1, profundidad2;
                int niveles1[10], niveles2[10];

                cout << "Ingrese profundidad de la primera versión: ";
                cin >> profundidad1;
                cout << "Ingrese los niveles de la primera versión separados por espacio: ";
                for (int i = 0; i < profundidad1; i++)
                    cin >> niveles1[i];

                cout << "Ingrese profundidad de la segunda versión: ";
                cin >> profundidad2;
                cout << "Ingrese los niveles de la segunda versión separados por espacio: ";
                for (int i = 0; i < profundidad2; i++)
                    cin >> niveles2[i];

                bool iguales = false;
                TipoRet resultado = Iguales(*archivo, niveles1, niveles2, profundidad1, profundidad2, iguales);

                if (resultado == ERROR) {
                    cout << " Alguna de las versiones no existe.\n";
                } else {
                    if (iguales)
                        cout << " Las versiones SON IGUALES.\n";
                    else
                        cout << " Las versiones SON DIFERENTES.\n";
                }
                break;
            }
            case 0:
                cout << "Saliendo del programa...\n";
                break;

            default:
                cout << "Opción inválida.\n";
        }

    } while (opcion != 0);

    return 0;
}












/*
┌────────────────────────────┐
│        ARCHIVO             │
│ nombreArchivo = "main.txt" │
│ primeraVersion ────────────┼──► (Versión 1)
│ ultimaVersion ─────────────┘
└────────────────────────────┘


                   ▼
        ┌────────────────────────────┐
        │        Versión 1           │
        │ niveles = [1]              │
        │ profundidad = 1            │
        │ cantidadLineas = 3         │
        │ lineas = { "Hola", "Mundo", "v1" } │
        │ cantidadCambios = 1         │
        │ cambios = { "IL: v1" }      │
        │ padre = NULL                │
        │ primerHijo ─────────────┐
        │ siguienteHermano = NULL │
        └─────────────────────────┘
                                 │
                                 ▼
             ┌────────────────────────────┐
             │        Versión 1.1         │
             │ niveles = [1,1]            │
             │ profundidad = 2            │
             │ cantidadLineas = 4         │
             │ lineas = { "Hola", "Mundo", "v1", "v1.1" } │
             │ cantidadCambios = 1         │
             │ cambios = { "IL: v1.1" }    │
             │ padre ───────────► (Versión 1)
             │ primerHijo ───────┐
             │ siguienteHermano ─┼──► (Versión 1.2)
             └───────────────────┘
                                 │
                                 ▼
             ┌────────────────────────────┐
             │        Versión 1.1.1       │
             │ niveles = [1,1,1]          │
             │ profundidad = 3            │
             │ cantidadLineas = 5         │
             │ lineas = { "Hola", "Mundo", "v1", "v1.1", "v1.1.1" } │
             │ cambios = { "IL: v1.1.1" } │
             │ padre ─────────► (Versión 1.1)
             │ primerHijo = NULL          │
             │ siguienteHermano = NULL    │
             └────────────────────────────┘


             ┌────────────────────────────┐
             │        Versión 1.2         │
             │ niveles = [1,2]            │
             │ profundidad = 2            │
             │ cantidadLineas = 4         │
             │ lineas = { "Hola", "Mundo", "v1", "v1.2" } │
             │ cambios = { "IL: v1.2" }   │
             │ padre ─────────► (Versión 1)
             │ primerHijo = NULL          │
             │ siguienteHermano = NULL    │
             └────────────────────────────┘

*/