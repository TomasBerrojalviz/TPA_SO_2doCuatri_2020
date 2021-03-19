#ifndef RESTAURANTE_SRC_UTILS_RESTAURANTE_H_
#define RESTAURANTE_SRC_UTILS_RESTAURANTE_H_

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<netdb.h>
#include<string.h>
// Incluir datos de los demas modulos
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<readline/readline.h>
#include <semaphore.h>
#include "utils_en_comun.h"
#include<commons/collections/dictionary.h>

//typdefs

typedef enum {
	NEW,
	READY,
	BLOCKED,
	EXEC,
	EXIT
} t_estado;

typedef enum {
	OK2,
	FAIL2
} t_respuesta2;

typedef enum{
	PENDIENTE,
	CONFIRMADO,
	TERMINADO
} estado_pedido;

typedef struct{
	uint32_t tamanioPlato;
	uint32_t id_pedido;
	char* plato;
} aniadir_plato;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante/* = strlen(nombreRestaurante) + 1*/; // TODO esto funcara?
	uint32_t id_pedido/* = string_duplicate(nombreRestaurante)*/;
} terminar_pedido;


typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t tamanioNombrePlato;
	char* nombrePlato;
	uint32_t cantidad_platos;
	uint32_t id_pedido;
} guardar_plato;

typedef struct{
	uint32_t id_pedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} confirmar_pedido;

typedef struct{
	uint32_t idPedido;
}consultar_pedido;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t id_pedido;
} obtener_pedido;

typedef struct{
	uint32_t posX;
	uint32_t posY;
} posicion;

typedef struct{
	t_queue* cola;
	char* nombrePlato;
	sem_t cocinerosDisponibles;
	sem_t platosEncolados;
	pthread_mutex_t mutex_ready;
} colaReady;

typedef struct{
	char* nombrePaso;
	uint32_t* tiempoRestante;
} pasoReceta;

typedef struct{
	uint32_t id_pcb;
	uint32_t id_pedido;
	t_estado estado;
	char* plato;
	t_list* pasosARealizar;
	uint32_t socketSolicitante;
	bool finDeQuantum;
} t_pcb_plato;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t tamanioNombrePlato;
	char* nombrePlato;
	uint32_t id_pedido;
} plato_listo;

typedef struct{
    uint32_t tamanioNombre;
    char* nombre_restaurante;
    uint32_t cantidad_cocineros;
    posicion posicion;
    t_list* listaTamanioAfinidades;
    t_list* afinidad_cocineros;
    t_list* listaTamanioPlatos;
    t_list* platos;
    t_list* precio_platos;
    uint32_t cantidad_hornos;
    uint32_t cantidadPedidos;
} obtener_restaurante_respuesta;

typedef struct{
    t_list* listaTamanioPlatos;
    t_list* platos;
    t_list* precio_platos;
} consultar_platos_respuesta;

typedef struct{
	estado_pedido estado;
	t_list* listaTamanioPlato;
	t_list* listaPlatos;
	t_list* listaCantidadDePedido;
	t_list* listaCantidadLista;
	uint32_t precioTotal;
} obtener_pedido_respuesta;

typedef struct{
	uint32_t tamanioNombre;
	char* nombre_restaurante;
	estado_pedido estado;
	t_list* listaTamanioPlato;
	t_list* listaPlatos;
	t_list* listaCantidadDePedido;
	t_list* listaCantidadLista;
	uint32_t precioTotal;
} consultar_pedido_respuesta;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t id_pedido;
} guardar_pedido;

typedef struct{
	t_list* listaPasos;
	t_list* listaTiempoPasos;
} t_receta;

typedef struct{
	t_list* listaTamanioPasos;
	t_list* listaPasos;
	t_list* listaTiempoPasos;
} t_receta_respuesta;

// Config
char* puerto_escucha;
char* ip_sindicato;
char* puerto_sindicato;
char* ip_app;
char* puerto_app;
uint32_t quantum; //Quiza ponerlo int
char* archivo_log;
char* algoritmo_planificacion;
char* ip_restaurante;
char* nombreRestaurante;
uint32_t retardo_cpu;
char* pathConfig;

		
// Main
t_config* config;
t_log* logger;
uint32_t socketConexionSindicato;
uint32_t socketConApp;

//Datos del Restaurante
uint32_t cantidadDeCocineros;
posicion posicionRestaurante;
t_list* afinidades;
t_list* platos;
t_list* preciosPlatos;
uint32_t cantidadDeHornos;
uint32_t pedido;

// Planificacion
t_list* listaDeColasReady;
uint32_t id_pcb;
//void* planificacion_corto_plazo();
t_list* lista_pcbs_platos;
t_queue* cola_new;
pthread_t planificar_largo;
t_queue* cola_hornos;
t_queue* cola_exit;


// Semaforos
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_exit;
pthread_mutex_t mutex_cola_ready;
pthread_mutex_t mutex_cola_hornos;
pthread_mutex_t mutex_idPcb;
pthread_mutex_t mutex_listaPcb;
sem_t semaforo_nuevo_pcb;
sem_t semaforo_ready_pcb;
sem_t semaforo_hay_plato_para_el_horno;

pthread_t hiloReceive;

void iniciar_servidor(void);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);
uint32_t conectar_sindicato();
uint32_t conectar_app();
void asignar_datos_restaurante(obtener_restaurante_respuesta* obtenerRestauranteRespuesta);
guardar_plato* aniadir_plato_a_guardar_plato(aniadir_plato* aniadirPlato);
void destruir_elementos(void* elemento);

#endif /* RESTAURANTE_SRC_UTILS_RESTAURANTE_H_ */
