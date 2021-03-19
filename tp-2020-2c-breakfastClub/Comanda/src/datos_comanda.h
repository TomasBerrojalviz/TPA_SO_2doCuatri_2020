#ifndef SRC_DATOS_COMANDA_H_
#define SRC_DATOS_COMANDA_H_

#include "/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/utils_en_comun.h"
#include "utils_comanda.h"

typedef enum {
	OK2,
	FAIL2
} t_respuesta2;

typedef enum {
	PENDIENTE2,
	CONFIRMADO2
} t_estado2;


typedef struct {
	uint32_t idPedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} guardar_pedido;

typedef struct {
	uint32_t idPedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t tamanioNombrePlato;
	char* nombrePlato;
	uint32_t cantidadPlato;
} guardar_plato;

typedef struct {
	uint32_t idPedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} confirmar_pedido;

typedef struct {
	uint32_t idPedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} obtener_pedido;

typedef struct {
	uint32_t idPedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t tamanioNombrePlato;
	char* nombrePlato;
} plato_listo;


typedef struct {
	uint32_t idPedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} finalizar_pedido;

// ENVIO
void enviar_respuesta(bool respuestaBool, uint32_t socketCliente);
void enviar_datos_pedido_comanda(t_estado2 estadoPedido, t_list* infoPedido, uint32_t socketCliente);

// DESERIALIZAR
confirmar_pedido* deserializar_confirmar_pedido(int socket_cliente);
guardar_plato* deserializar_guardar_plato(int socket_cliente);
guardar_pedido* deserializar_guardar_pedido(int socket_cliente);
obtener_pedido* deserializar_obtener_pedido(int socket_cliente);

// SERIALIZACION
void serializar_respuesta(t_respuesta2 rta, t_buffer* buffer);
void serializar_datos_pedido_comanda(t_estado2 estadoPedido, t_list* infoPedido, t_buffer* buffer);

#endif /* SRC_DATOS_COMANDA_H_ */
