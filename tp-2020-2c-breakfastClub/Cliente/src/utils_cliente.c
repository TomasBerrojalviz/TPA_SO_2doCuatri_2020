/*
 * utils_cliente.c
 *
 *  Created on: 31 ago. 2020
 *      Author: utnso
 */

#include "utils_cliente.h"
#include <sys/time.h>
#include <sys/types.h>
#include "/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/utils_en_comun.h"

void procesar_mensaje_recibido(int cod_op, int cliente_fd) {

	uint32_t buffer_size;
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);


	//Procesar mensajes recibidos
	switch (cod_op) {
		case FINALIZAR_PEDIDO:
			log_info(logger, "ME LLEGO EL MENSAJE: FINALIZAR_PEDIDO");

			finalizar_pedido* finalizar_pedido_recibido = deserializar_finalizar_pedido(cliente_fd);

			log_info(logger,"Recibi el pedido %d del restaurante %s", finalizar_pedido_recibido->id_pedido, finalizar_pedido_recibido->nombreRestaurante);

			free(finalizar_pedido_recibido->nombreRestaurante);
			free(finalizar_pedido_recibido);

			break;
		default:
			break;
	}
}


