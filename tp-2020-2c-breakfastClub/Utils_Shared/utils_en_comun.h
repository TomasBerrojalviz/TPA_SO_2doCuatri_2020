#ifndef UTILS_EN_COMUN_H_
#define UTILS_EN_COMUN_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>

typedef enum {
	CONSULTAR_RESTAURANTES,
	SELECCIONAR_RESTAURANTE,
	OBTENER_RESTAURANTE,
	OBTENER_RESTAURANTE_RESPUESTA,
	CONSULTAR_PLATOS,
	CREAR_PEDIDO,
	GUARDAR_PEDIDO,
	ANIADIR_PLATO,
	GUARDAR_PLATO,
	CONFIRMAR_PEDIDO,
	PLATO_LISTO,
	CONSULTAR_PEDIDO,
	OBTENER_PEDIDO,
	OBTENER_PEDIDO_RESPUESTA,
	FINALIZAR_PEDIDO,
	TERMINAR_PEDIDO,
	OBTENER_RECETA,
	RESPUESTA,
	RESPUESTA2,
	RESPUESTA_CREAR_PEDIDO,
	RESPUESTA_ANIADIR_PLATO,
	RESPUESTA_CONFIRMAR_PEDIDO,
	TERMINAR,
	NOMBRE_RESTAURANTE,
	DATOS_CLIENTE,
	CARAJO,
	DATOS_PEDIDO_COMANDA,
	CERRAR_CONEXION
} op_code;

//---------Typedefs--------//
typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} t_restaurante;
//-------------------------//

//---------Mensajes--------//




//-------------------------//



typedef struct {
	int size;
	void* stream;
} t_buffer;

typedef struct {
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

t_paquete* crear_paquete(uint32_t unOp_code);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void* serializar_paquete(t_paquete* paquete, int* bytes);
void crear_buffer(t_paquete* paquete);
uint32_t crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);
int contar_elementos_array(char** array);
void* serializar_buffer_respuesta(t_buffer* buffer, int* bytes);
t_buffer* crear_buffer_respuesta();


#endif /* UTILS_EN_COMUN_H_ */
