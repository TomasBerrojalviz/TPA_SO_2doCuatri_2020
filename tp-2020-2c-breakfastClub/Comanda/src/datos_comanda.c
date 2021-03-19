#include "utils_en_comun.h"
#include "datos_comanda.h"


// ENVIO
void enviar_respuesta(bool respuestaBool, uint32_t socketCliente){
	t_paquete* paquete_a_enviar = crear_paquete(CONFIRMAR_PEDIDO);

	t_respuesta2 respuesta;

	if(respuestaBool){
		respuesta = OK2;
	} else{
		respuesta = FAIL2;
	}

	serializar_respuesta(respuesta, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketCliente, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}
void enviar_datos_pedido_comanda(t_estado2 estadoPedido, t_list* infoPedido, uint32_t socketCliente){
	t_paquete* paquete_a_enviar = crear_paquete(DATOS_PEDIDO_COMANDA);

	serializar_datos_pedido_comanda(estadoPedido, infoPedido, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketCliente, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

// SERIALIZACION
void serializar_respuesta(t_respuesta2 rta, t_buffer* buffer){
	//1. Respuesta

	buffer->size = sizeof(t_respuesta2);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(rta), sizeof(t_respuesta2));
}

void serializar_datos_pedido_comanda(t_estado2 estadoPedido, t_list* infoPedido, t_buffer* buffer){
	//0. Estado
	//1. Cantidad de elementos
	//2. tamanioNombre
	//3. NombrePlato
	//4. cantidadTotal
	//5. cantidadLista

	int cant = list_size(infoPedido);
	int offset = 0;

	if(cant > 0){
		buffer->size = sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(uint32_t)*3*cant);

		for (int i = 0; i < cant; i++) {
			int tamanioNombre = strlen(((t_pagina*) list_get(infoPedido, i))->nombrePlato)+1;
			buffer->size += tamanioNombre;
		}

		buffer->stream = malloc(buffer->size);

		memcpy(buffer->stream + offset, &estadoPedido, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &cant, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		for (int i = 0; i < cant; ++i) {
			t_pagina* pagina = (t_pagina*) list_get(infoPedido, i);
			int tamanioNombre = strlen(pagina->nombrePlato) +1;

			memcpy(buffer->stream + offset, &tamanioNombre, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(buffer->stream + offset, pagina->nombrePlato, tamanioNombre);
			offset += tamanioNombre;

			memcpy(buffer->stream + offset, &pagina->cantidadPlato, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(buffer->stream + offset, &pagina->cantidadLista, sizeof(uint32_t));
			offset += sizeof(uint32_t);

		}

	}

}

// DESERIALIZACION
guardar_pedido* deserializar_guardar_pedido(int socket_cliente) {

	//------------ORDEN------------
	//1. idPedido
	//2. tamanioNombre
	//3. nombreRestaurante
	//-----------------------------

	guardar_pedido* msg_recibido = malloc(sizeof(guardar_pedido));

	recv(socket_cliente, &(msg_recibido->idPedido), sizeof(uint32_t), MSG_WAITALL);

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), MSG_WAITALL);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, MSG_WAITALL);


	return msg_recibido;
}

guardar_plato* deserializar_guardar_plato(int socket_cliente) {

	//------------ORDEN------------
	//1. Tamanio Nombre Restaurante
	//2. Nombre Restaurante;
	//3. Tamanio Nombre Plato
	//4. Plato
	//5. Cantidad
	//6. Id Pedido
	//-----------------------------

	guardar_plato* msg_recibido = malloc(sizeof(guardar_plato));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);

	recv(socket_cliente, &(msg_recibido->tamanioNombrePlato), sizeof(uint32_t), 0);
	msg_recibido->nombrePlato = malloc(msg_recibido->tamanioNombrePlato);

	recv(socket_cliente, msg_recibido->nombrePlato, msg_recibido->tamanioNombrePlato, 0);

	recv(socket_cliente, &(msg_recibido->cantidadPlato), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg_recibido->idPedido), sizeof(uint32_t), 0);

	return msg_recibido;
}

confirmar_pedido* deserializar_confirmar_pedido(int socket_cliente){

	//------------ORDEN------------
	//1. Id Pedido
	//2. Tamanio Nombre Restaurante
	//3. Nombre Restaurante;
	//-----------------------------

	confirmar_pedido* msg_recibido = malloc(sizeof(confirmar_pedido));

	recv(socket_cliente, &(msg_recibido->idPedido), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, (msg_recibido->nombreRestaurante), msg_recibido->tamanioNombre, 0);

	return msg_recibido;
}

obtener_pedido* deserializar_obtener_pedido(int socket_cliente){

	//------------ORDEN------------
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante
	//3. ID Pedido
	//-----------------------------

	obtener_pedido* msg_recibido = malloc(sizeof(obtener_pedido));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, (msg_recibido->nombreRestaurante), msg_recibido->tamanioNombre, 0);

	recv(socket_cliente, &(msg_recibido->idPedido), sizeof(uint32_t), 0);

	return msg_recibido;
}

plato_listo* deserializar_plato_listo(int socket_cliente){

	//------------ORDEN------------
	//1. Tamanio Nombre Restaurante
	//2. Nombre Restaurante;
	//3. Tamanio Nombre Plato
	//4. Plato
	//5. id Pedido
	//-----------------------------

	plato_listo* msg_recibido = malloc(sizeof(plato_listo));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, (msg_recibido->nombreRestaurante), msg_recibido->tamanioNombre, 0);

	recv(socket_cliente, &(msg_recibido->tamanioNombrePlato), sizeof(uint32_t), 0);
	msg_recibido->nombrePlato = malloc(msg_recibido->tamanioNombrePlato);

	recv(socket_cliente, (msg_recibido->nombrePlato), msg_recibido->tamanioNombrePlato, 0);

	recv(socket_cliente, &(msg_recibido->idPedido), sizeof(uint32_t), 0);

	return msg_recibido;
}

finalizar_pedido* deserializar_finalizar_pedido(int socket_cliente){

	//------------ORDEN------------
	//1. idPedido
	//2. tamanioNombre
	//3. nombreRestaurante
	//-----------------------------

	finalizar_pedido* msg_recibido = malloc(sizeof(finalizar_pedido));

	recv(socket_cliente, &(msg_recibido->idPedido), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, (msg_recibido->nombreRestaurante), msg_recibido->tamanioNombre, 0);


	return msg_recibido;
}
