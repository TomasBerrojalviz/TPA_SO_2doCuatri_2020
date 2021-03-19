
#ifndef UTILS_CLIENTE_H_
#define UTILS_CLIENTE_H_


//Los include que va a usar el modulo
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
//INCLUIR LOS DATOS DE LOS DEMAS MODULOS, ya que se van a enviar sus respectivos mensajes
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/queue.h>
#include<commons/collections/list.h>
#include<readline/readline.h>
#include "/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/utils_en_comun.h"

typedef enum{
	PENDIENTE,
	CONFIRMADO,
	TERMINADO
} estado_pedido;

typedef struct{
	uint32_t id_pedido;
} consultar_pedido;

typedef struct{
	uint32_t tamanioPlato;
	char* plato;
	uint32_t idPedido;
} aniadir_plato;

typedef struct{
	t_list* listaTamanioPasos;
	t_list* listaPasos;
	t_list* listaTiempoPasos;
} t_receta_respuesta;


typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t id_pedido;
} finalizar_pedido;

typedef struct {
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} seleccionar_restaurante;

typedef struct {
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} consultar_platos;

typedef struct{ // TIRAR REFACTOR PARA T_RESTAURANTE
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} obtener_restaurante;

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
	uint32_t tamanioNombre;
	char* nombreRestaurante;
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
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t id_pedido;
} terminar_pedido;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreReceta;
} obtener_receta;

//Las variables que va a usar el modulo
char* ipRestaurante;
char* puertoRestaurante;
int socketConexionRestaurante;

char* ipSindicato;
char* puertoSindicato;
int socketConexionSindicato;

char* ipApp;
char* puertoApp;
int socketConexionApp;
int socketEscuchaApp;

char* ipComanda;
char* puertoComanda;
int socketConexionComanda;

char* puertoEscucha;

int posicionX;
int posicionY;

char* idCliente;

pthread_t hiloReceive;

t_config* config_cliente;
t_log* logger;

void escuchar_app(void);
void serve_client(int* socket);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);

#endif /* UTILS_CLIENTE_H_ */
