#include <stdio.h> //funciones standar
#include <stdlib.h> //almacenamient
#include <string.h>//texto
#include <time.h>//reloj

//definiciones
//menu
typedef enum { PENDIENTE, EN_CURSO, TERMINADA, CANCELADA } Estado;
//estructura
typedef struct {
    char titulo[100];
    char descripcion[255];
    Estado estado;
    char fecha_creacion[20];
    char vencimiento[20];
    int dificultad;
} Tarea;

///Funciones Auxiliares
//tiempo
void obtener_fecha_actual(char* buffer) {
    time_t tiempo_actual = time(NULL);
    struct tm *t = localtime(&tiempo_actual);
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", t);
}
//espacio con enter
void limpiar_salto_linea(char* cadena) {
    cadena[strcspn(cadena, "\n")] = 0;
}
//anti error
int leer_entero_seguro() {
    char buffer[50];
    fgets(buffer, sizeof(buffer), stdin);
    int numero;
    if (sscanf(buffer, "%d", &numero) == 1) {
        return numero;
    }
    return -1; 
}

//tareas
//estado como parametro
void crear_tarea(Tarea* t, const char* titulo, const char* descripcion, const char* vencimiento, int dificultad, Estado estado_inicial) {
    strncpy(t->titulo, titulo, sizeof(t->titulo) - 1);
    strncpy(t->descripcion, descripcion, sizeof(t->descripcion) - 1);
    if (vencimiento != NULL) {
        strncpy(t->vencimiento, vencimiento, sizeof(t->vencimiento) - 1);
    } else {
        strcpy(t->vencimiento, "");
    }
    //copia el resto del texto
    t->estado = estado_inicial;
    obtener_fecha_actual(t->fecha_creacion);
    t->dificultad = dificultad;
}
//estado con numeros
void mostrar_detalle(Tarea* t) {
    printf("\n--- %s ---\n", t->titulo);
    printf("Estado: ");
    switch(t->estado) {
        case PENDIENTE: printf("Pendiente\n"); break;
        case EN_CURSO: printf("En curso\n"); break;
        case TERMINADA: printf("Terminada\n"); break;
        case CANCELADA: printf("Cancelada\n"); break;
    }
    printf("Dificultad: ");
    if (t->dificultad == 1) printf("@-- (Facil)\n");
    else if (t->dificultad == 2) printf("@@- (Medio)\n");
    else if (t->dificultad == 3) printf("@@@ (Dificil)\n");
    else printf("Desconocida\n");
    
    printf("Creada el: %s\n", t->fecha_creacion);
    if (strlen(t->vencimiento) > 0) printf("Vencimiento: %s\n", t->vencimiento);
    if (strlen(t->descripcion) > 0) printf("Descripcion: %s\n", t->descripcion);
    printf("-----------------------\n");
}

//sitema de guardado
void guardar_tareas(Tarea* tareas, int cantidad, const char* nombre_archivo) {
    FILE* archivo = fopen(nombre_archivo, "wb");
    if (archivo != NULL) {
        fwrite(&cantidad, sizeof(int), 1, archivo);
        fwrite(tareas, sizeof(Tarea), cantidad, archivo);
        fclose(archivo);
    }
}
//sistema de cargado
int cargar_tareas(Tarea** tareas_ptr, const char* nombre_archivo) {
    FILE* archivo = fopen(nombre_archivo, "rb");
    int cantidad = 0;
    if (archivo != NULL) {
        fread(&cantidad, sizeof(int), 1, archivo);
        if (cantidad > 0) {
            *tareas_ptr = malloc(cantidad * sizeof(Tarea));
            fread(*tareas_ptr, sizeof(Tarea), cantidad, archivo);
        }
        fclose(archivo);
    }
    return cantidad;
}

//PROGRAMA PRINCIPAL
int main() {
    Tarea* mis_tareas = NULL; 
    int cantidad_tareas = cargar_tareas(&mis_tareas, "mis_tareas.dat");
    int opcion;

    do {
        printf("\n=============================\n");
        printf("      GESTOR DE TAREAS\n");
        printf("=============================\n");
        printf("1. Ver mis tareas\n");
        printf("2. Buscar una tarea\n");
        printf("3. Agregar nueva tarea\n");
        printf("4. Editar una tarea\n"); // NUEVA OPCIÓN
        printf("5. Guardar y Salir\n"); // Desplazado al 5
        printf("=============================\n");
        printf("Elige una opcion: ");
        
        opcion = leer_entero_seguro();

        switch(opcion) {
            case 1: {
                int sub_opcion;
                do {
                    printf("\n--- MENU VISUALIZACION ---\n");
                    printf("1. Ver Todas\n");
                    printf("2. Ver Pendientes\n");
                    printf("3. Ver En Curso\n");
                    printf("4. Ver Terminadas\n");
                    printf("5. Volver al Menu Principal\n");
                    printf("Elige una opcion: ");
                    
                    sub_opcion = leer_entero_seguro();

                    if (sub_opcion >= 1 && sub_opcion <= 4) {
                        int encontradas = 0;
                        for (int i = 0; i < cantidad_tareas; i++) {
                            if (sub_opcion == 1 || 
                               (sub_opcion == 2 && mis_tareas[i].estado == PENDIENTE) ||
                               (sub_opcion == 3 && mis_tareas[i].estado == EN_CURSO) ||
                               (sub_opcion == 4 && mis_tareas[i].estado == TERMINADA)) {
                                
                                mostrar_detalle(&mis_tareas[i]);
                                encontradas++;
                            }
                        }
                        if (encontradas == 0) {
                            printf("\n[!] No se encontraron tareas en esta categoria.\n");
                        }
                    } else if (sub_opcion != 5) {
                        printf("\n[Error] Opcion invalida. Por favor, intenta de nuevo.\n");
                    }
                } while (sub_opcion != 5);
                break;
            }

            case 2: {
                char termino[100];
                int encontradas = 0;
                printf("\nIngresa una palabra para buscar en el titulo: ");
                fgets(termino, sizeof(termino), stdin);
                limpiar_salto_linea(termino);

                for (int i = 0; i < cantidad_tareas; i++) {
                    if (strstr(mis_tareas[i].titulo, termino) != NULL) {
                        mostrar_detalle(&mis_tareas[i]);
                        encontradas++;
                    }
                }
                if (encontradas == 0) printf("\n[!] No se encontraron tareas.\n");
                break;
            }

            case 3: {
                char titulo[100], desc[255], venc[20];
                int dif, est;

                printf("\n--- NUEVA TAREA ---\n");
                printf("Titulo (obligatorio): ");
                fgets(titulo, sizeof(titulo), stdin);
                limpiar_salto_linea(titulo);

                printf("Descripcion (opcional, presiona Enter para saltar): ");
                fgets(desc, sizeof(desc), stdin);
                limpiar_salto_linea(desc);

                printf("Vencimiento (ej. 2026-10-15, Enter para saltar): ");
                fgets(venc, sizeof(venc), stdin);
                limpiar_salto_linea(venc);

                do {
                    printf("Estado (0=Pendiente, 1=En Curso, 2=Terminada, 3=Cancelada): ");
                    est = leer_entero_seguro();
                    if (est < 0 || est > 3) printf("[Error] Ingresa un numero valido (0 a 3).\n");
                } while (est < 0 || est > 3);

                do {
                    printf("Dificultad (1=Facil, 2=Medio, 3=Dificil): ");
                    dif = leer_entero_seguro();
                    if (dif < 1 || dif > 3) printf("[Error] Ingresa un numero valido (1, 2 o 3).\n");
                } while (dif < 1 || dif > 3);

                mis_tareas = realloc(mis_tareas, (cantidad_tareas + 1) * sizeof(Tarea));
                
                crear_tarea(&mis_tareas[cantidad_tareas], titulo, desc, venc, dif, (Estado)est);
                cantidad_tareas++;
                printf("\n[+] Tarea agregada con exito.\n");
                break;
            }

            case 4: {
                if (cantidad_tareas == 0) {
                    printf("\n[!] No hay tareas para editar.\n");
                    break;
                }

                printf("\n--- SELECCIONA UNA TAREA PARA EDITAR ---\n");
                for (int i = 0; i < cantidad_tareas; i++) {
                    printf("[%d] %s\n", i + 1, mis_tareas[i].titulo);
                }
                printf("[0] Cancelar y volver\n");
                printf("Elige el numero de la tarea: ");
                
                int indice = leer_entero_seguro();

                if (indice > 0 && indice <= cantidad_tareas) {
                    Tarea* tarea_editar = &mis_tareas[indice - 1];
                    int op_editar;

                    do {
                        mostrar_detalle(tarea_editar);
                        printf("\n--- EDITANDO TAREA ---\n");
                        printf("1. Cambiar Titulo\n");
                        printf("2. Cambiar Descripcion\n");
                        printf("3. Cambiar Estado\n");
                        printf("4. Cambiar Vencimiento\n");
                        printf("5. Cambiar Dificultad\n");
                        printf("6. Terminar edicion y volver\n");
                        printf("Elige que deseas modificar: ");
                        
                        op_editar = leer_entero_seguro();

                        switch (op_editar) {
                            case 1:
                                printf("Nuevo Titulo: ");
                                fgets(tarea_editar->titulo, sizeof(tarea_editar->titulo), stdin);
                                limpiar_salto_linea(tarea_editar->titulo);
                                break;
                            case 2:
                                printf("Nueva Descripcion: ");
                                fgets(tarea_editar->descripcion, sizeof(tarea_editar->descripcion), stdin);
                                limpiar_salto_linea(tarea_editar->descripcion);
                                break;
                            case 3: {
                                int nuevo_estado;
                                do {
                                    printf("Nuevo Estado (0=Pendiente, 1=En Curso, 2=Terminada, 3=Cancelada): ");
                                    nuevo_estado = leer_entero_seguro();
                                } while (nuevo_estado < 0 || nuevo_estado > 3);
                                tarea_editar->estado = (Estado)nuevo_estado;
                                break;
                            }
                            case 4:
                                printf("Nuevo Vencimiento: ");
                                fgets(tarea_editar->vencimiento, sizeof(tarea_editar->vencimiento), stdin);
                                limpiar_salto_linea(tarea_editar->vencimiento);
                                break;
                            case 5: {
                                int nueva_dif;
                                do {
                                    printf("Nueva Dificultad (1=Facil, 2=Medio, 3=Dificil): ");
                                    nueva_dif = leer_entero_seguro();
                                } while (nueva_dif < 1 || nueva_dif > 3);
                                tarea_editar->dificultad = nueva_dif;
                                break;
                            }
                            case 6:
                                printf("\n[+] Edicion finalizada.\n");
                                break;
                            default:
                                printf("\n[Error] Opcion invalida.\n");
                        }
                    } while (op_editar != 6);
                } else if (indice != 0) {
                    printf("\n[Error] Numero de tarea invalido.\n");
                }
                break;
            }

            case 5:
                guardar_tareas(mis_tareas, cantidad_tareas, "mis_tareas.dat");
                free(mis_tareas); 
                printf("\nGuardando cambios... Hasta luego!\n");
                break;

            default:
                printf("\n[Error] Opcion invalida. Por favor, intenta de nuevo.\n");
                break;
        }

    } while(opcion != 5);

    return 0;
}