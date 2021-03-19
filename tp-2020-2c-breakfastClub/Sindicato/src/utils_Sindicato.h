#ifndef UTILS_SINDICATO_H_

#define UTILS_SINDICATO_H_

#define ARCHIVO_CONFIG "/home/utnso/tp-2020-2c-breakfastClub/Sindicato/sindicato.config"

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<signal.h>
#include<unistd.h>
#include <sys/stat.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<netdb.h>
#include<string.h>
#include <math.h>
// Incluir datos de los demas modulos
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/queue.h>
#include<commons/collections/list.h>
#include<commons/bitarray.h>
#include<readline/readline.h>
#include <semaphore.h>

#include "utils_en_comun.h"

typedef enum {
	NEW,
	READY,
	BLOCKED,
	EXEC,
	EXIT
} t_estado;

typedef enum {
	INFO,
	PEDIDO,
	RECETA,
	BLOQUE
} tipo_archivo;

typedef enum{
	PENDIENTE,
	CONFIRMADO,
	TERMINADO
} estado_pedido;

typedef enum {
	OK2,
	FAIL2
} t_respuesta2;

typedef struct{
	char* restaurante;
	uint32_t cantPedidos;
	pthread_mutex_t semInfo;
	pthread_mutex_t* semPedidos;
} FS_restaurante;

typedef struct{
	char* receta;
	pthread_mutex_t semReceta;
} FS_receta;

typedef struct{
	char* stream;
	uint32_t tamanioTotal;
	uint32_t bloqueInicial;
	t_dictionary* diccionario;
} t_archivo_FS;

typedef struct{
	char* stream;
	uint32_t bloqueSiguiente;
	t_dictionary* diccionario;
} t_bloque_FS;

typedef struct{
	uint32_t posX;
	uint32_t posY;
} posicion;

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
	char* nombre_receta;
	t_list* pasos;
	t_list* tiempo_pasos;
} crear_receta;

typedef struct{
    t_list* listaTamanioPlatos;
    t_list* platos;
    t_list* precio_platos;
} consultar_platos_respuesta;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t id_pedido;
} guardar_pedido;

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
	char* nombreRestaurante;
	uint32_t tamanioNombre;

} confirmar_pedido;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t tamanioNombrePlato;
	char* nombrePlato;
	uint32_t id_pedido;
} plato_listo;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t id_pedido;
} obtener_pedido;

typedef struct{
	estado_pedido estado;
	t_list* listaTamanioPlato;
	t_list* platos;
	t_list* listaCantidadDePedido;
	t_list* listaCantidadLista;
	uint32_t precioTotal;
} obtener_pedido_respuesta;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t id_pedido;
} terminar_pedido;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreReceta;
} obtener_receta;

typedef struct{
	t_list* listaPasos;
	t_list* listaTiempoPasos;
} t_receta;

typedef struct{
	t_list* listaTamanioPasos;
	t_list* listaPasos;
	t_list* listaTiempoPasos;
} t_receta_respuesta;

typedef struct{
	uint32_t bloqueInicial;
	uint32_t tamanio;
}info_bloque;

char* ip_sindicato;
char* punto_montaje;
char* puerto_escucha;
char* archivo_log;
char* magic_number;
uint32_t blocks;
uint32_t block_size;

t_config* config;
t_log* logger;

char* path_files;
char* path_restaurantes;
char* path_recetas;
char* path_metadata;
char* path_bloques;
char* bitarrayFS;
char* ruta_bitmap;
char* ruta_metada;
char* ruta_bloques;

pthread_t hiloReceive;

//Semaforos

pthread_mutex_t mutexConsultarPlatos;
pthread_mutex_t mutexGuardarPedido;
pthread_mutex_t mutexGuardarPlato;
pthread_mutex_t mutexConfirmarPedido;
pthread_mutex_t mutexObtenerPedido;
pthread_mutex_t mutexObtenerRestaurante;
pthread_mutex_t mutexPlatoListo;
pthread_mutex_t mutexObtenerReceta;
pthread_mutex_t mutexTerminarPedido;
pthread_mutex_t* mutexBlocks;

void iniciar_servidor(void);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);
uint32_t dar_tamanio_t_list(t_list* lista);
obtener_pedido_respuesta* dar_nuevo_pedido();
void inicializar_semaforos();

#endif /* UTILS_SINDICATO_H_ */
