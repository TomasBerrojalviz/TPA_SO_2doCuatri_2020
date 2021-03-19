#ifndef SRC_UTILS_COMANDA_H_
#define SRC_UTILS_COMANDA_H_

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
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include <commons/temporal.h>
#include<commons/collections/list.h>
#include<readline/readline.h>
#include "/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/utils_en_comun.h"
#include "datos_comanda.h"

//---------CONFIG--------//
char* puerto_escucha;
uint32_t tamanio_memoria;
uint32_t tamanio_swap;
char* ip_comanda;
char* algoritmo_reemplazo;
char* archivo_log;
//-------------------------//

//---------TYPEDEF--------//
typedef enum {
	PENDIENTE,
	CONFIRMADO,
	TERMINADO
} t_estado;

typedef struct {
	t_list* tabla_paginas;
	uint32_t idPedido;
	t_estado estadoPedido;
} t_segmento;

typedef struct {
	uint32_t cantidadLista;
	uint32_t cantidadPlato;
	char* nombrePlato;
} t_pagina;

typedef struct {
	uint32_t idPedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	t_estado estadoPedido;
} t_pedido;


// a chequear
typedef struct {
	uint32_t idPedido;
	t_pagina* pagina;
} platoEnMemoria;

typedef struct {
	uint32_t indice;
	void* marco;
	bool libre;
	bool bitUso;
	bool bitModificado;
	char* timeStamp;
	int idPedido;
	char* restaurante;
} entradaTablaMarcos;

typedef struct {
	int indiceMarcoSwap;
	bool libre;
	int idPedido;
	char* nombrePlato;
} entradaSwap;
//-------------------------//

//---------MAIN--------//
t_config* config; //1
t_log* logger; //1
uint32_t socketConexionApp;//1
void* memoria;
FILE* espacio_swap;
t_list* entradas_swap;
uint32_t tamanio_pagina;
t_dictionary* dic_restaurante_tabla_segmentos;
uint32_t tamanio_nombre_plato_maximo;
t_list* pedidosCreados;
entradaTablaMarcos* punteroMarcoClock;
t_list* tablaDeMarcos;
//-------------------------//

//Semaforos
pthread_mutex_t mutexTablaMarcos;
pthread_mutex_t mutexEntradasSwap;
pthread_mutex_t mutexMemoria;
pthread_mutex_t mutexSwap;
pthread_mutex_t mutexTablaSegmentos;
pthread_mutex_t mutexInfoPedidoAEnviar;
pthread_mutex_t mutexEscribirMemoria;
pthread_mutex_t mutexEliminarDeMemoria;
pthread_mutex_t mutexPedidosCreados;
pthread_mutex_t mutexFree;
pthread_mutex_t mutexVictima;
pthread_mutex_t mutexBuscarSegmento;
pthread_mutex_t mutexBuscarPagina;
pthread_mutex_t mutexBuscarInfoPedido;
pthread_mutex_t mutexBuscarSwap;

pthread_t hiloReceive;

// Server
void iniciar_servidor(void);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);

// Cosas de Memoria que estan en Utils
t_segmento* buscar_segmento(t_list*, uint32_t);
t_pagina* buscar_pagina(t_list*, char*);
t_list* obtener_info_pedido(t_segmento* segmentoPedido);
bool platos_terminados(t_segmento* segmentoPedido);
t_list* entradas_segun_pedido(uint32_t idPedido);
bool el_pedido_esta_en_memoria(t_segmento* segmentoPedido);
bool esta_en_memoria(t_pagina* pagina, uint32_t idPedido);
void eliminar_entradas_por_pedido(int idPedido);
void eliminar_pedido(t_segmento* segmentoPedido, t_list* tablaSegmentos);
void buscar_en_swap(int pos, t_pagina* pagina);
void escribir_en_swap(t_pagina* pagina, uint32_t idPedido);
void borrar_de_swap(uint32_t pos);
void eliminar_pedido_de_swap(uint32_t idPedido);

//---------FUNCIONES--------//
void* seleccionar_victima;
int indice_elemento(t_list* lista, void* elemento);
//-------------------------//



#endif /* SRC_UTILS_COMANDA_H_ */
