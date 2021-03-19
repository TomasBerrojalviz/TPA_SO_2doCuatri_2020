#include "datos_cliente.h"

//---------------Deserializacion--------------------//

finalizar_pedido* deserializar_finalizar_pedido(int socket_cliente) {

	//------------ORDEN------------
	//1. ID Pedido
	//2. tamanioNombre
	//3. nombreRestaurante
	//-----------------------------


	finalizar_pedido* msg_recibido = malloc(sizeof(finalizar_pedido));

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);

	return msg_recibido;
}

t_receta_respuesta* deserializar_obtener_receta_respuesta(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. Cantidad pasos
	//2. listaTamanioPasos	   		 } COMO SON LISTAS IRAN
	//3. listaPasos            	   	 }   DENTRO DE UN FOR
	//4. listaTiempoPasos     		 }
	//-----------------------------

	t_receta_respuesta* msg_recibido = malloc(sizeof(t_receta_respuesta));

	msg_recibido->listaPasos = list_create();
	msg_recibido->listaTamanioPasos = list_create();
	msg_recibido->listaTiempoPasos = list_create();
	uint32_t cantidadPasos;

	recv(socket_cliente, &(cantidadPasos), sizeof(uint32_t), 0);

	for (int i = 0; i < cantidadPasos; ++i) {
		uint32_t aux;

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaTamanioPasos, &(aux));
		char* auxPlatos = malloc(aux);

		recv(socket_cliente, auxPlatos, aux, 0);
		list_add(msg_recibido->listaPasos, auxPlatos);

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaTiempoPasos, &(aux));
	}

	return msg_recibido;

}
//---------------Envio de mensajes--------------------//

void enviar_datos_cliente(uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(DATOS_CLIENTE);

	serializar_datos_cliente(paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_cerrar_conexion(uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(CERRAR_CONEXION);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_consultar_restaurantes(uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_RESTAURANTES);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	uint32_t op_code;
	recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

	uint32_t buffer_size;
	recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	uint32_t cantRestaurantes;
	recv(socketConexion, &cantRestaurantes, sizeof(uint32_t), MSG_WAITALL);

	log_info(logger, "-Restaurantes disponibles-");

	uint32_t tamanioNombre = 0;
	if (cantRestaurantes > 0) {
		for (int i = 0; i < cantRestaurantes; i++) {
			recv(socketConexion, &tamanioNombre, sizeof(uint32_t), MSG_WAITALL);
			char* nombreRestaurante = malloc(tamanioNombre);

			recv(socketConexion, nombreRestaurante, tamanioNombre, MSG_WAITALL);

			log_info(logger, "%s", nombreRestaurante);

			free(nombreRestaurante);
		}
	} else {
		recv(socketConexion, &tamanioNombre, sizeof(uint32_t), MSG_WAITALL);

		char* nombreRestaurante = NULL;

		nombreRestaurante = malloc(tamanioNombre);

		recv(socketConexion, nombreRestaurante, tamanioNombre, MSG_WAITALL);

		log_info(logger, "%s", nombreRestaurante);

		free(nombreRestaurante);
	}
}

void enviar_seleccionar_restaurante(seleccionar_restaurante* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(SELECCIONAR_RESTAURANTE);

	serializar_seleccionar_restaurante(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {
		log_info(logger, "Restaurante seleccionado con exito");
	} else {
		log_info(logger, "Error al seleccionar restaurante");
	}
}

void enviar_obtener_restaurante(obtener_restaurante* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_RESTAURANTE);

	serializar_obtener_restaurante(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	uint32_t op_code;
	recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

	uint32_t buffer_size;
	recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	uint32_t tamanioNombreRestaurante;
	recv(socketConexion, &tamanioNombreRestaurante, sizeof(uint32_t), MSG_WAITALL);

	char* nombreRestaurante = malloc(tamanioNombreRestaurante);
	recv(socketConexion, nombreRestaurante, tamanioNombreRestaurante, MSG_WAITALL);

	log_info(logger, "Restaurante: %s", nombreRestaurante);

	free(nombreRestaurante);

	uint32_t cantidadCocineros;
	recv(socketConexion, &cantidadCocineros, sizeof(uint32_t), MSG_WAITALL);

	log_info(logger, "Cantidad de cocineros: %d", cantidadCocineros);

	uint32_t posX;
	recv(socketConexion, &posX, sizeof(uint32_t), MSG_WAITALL);

	uint32_t posY;
	recv(socketConexion, &posY, sizeof(uint32_t), MSG_WAITALL);

	log_info(logger, "Posicion del restaurante: (%d,%d)", posX, posY);

	uint32_t cantidadAfinidades;
	recv(socketConexion, &cantidadAfinidades, sizeof(uint32_t), MSG_WAITALL);


	if (cantidadAfinidades > 0) {
		log_info(logger, "Afinidades:");

		for (int i = 0; i < cantidadAfinidades; i++) {

			uint32_t tamanioAfinidad;

			recv(socketConexion, &tamanioAfinidad, sizeof(uint32_t),MSG_WAITALL);

			char* afinidad = malloc(tamanioAfinidad);

			recv(socketConexion, afinidad, tamanioAfinidad, MSG_WAITALL);

			log_info(logger, "%s", afinidad);

			free(afinidad);
		}

	}

	uint32_t cantidadPlatos;
	recv(socketConexion, &cantidadPlatos, sizeof(uint32_t), MSG_WAITALL);

	if (cantidadPlatos > 0) {
		log_info(logger, "Platos:");

		for (int i = 0; i < cantidadPlatos; i++) {

			uint32_t tamanioPlato;

			recv(socketConexion, &tamanioPlato, sizeof(uint32_t), MSG_WAITALL);

			char* plato = malloc(tamanioPlato);

			recv(socketConexion, plato, tamanioPlato, MSG_WAITALL);

			uint32_t precioPlato;

			recv(socketConexion, &precioPlato, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "%s", plato);

			log_info(logger, "Precio: %d", precioPlato);

			free(plato);
		}

	}

	uint32_t cantidadHornos;
	recv(socketConexion, &cantidadHornos, sizeof(uint32_t), MSG_WAITALL);

	log_info(logger, "Cantidad de hornos: %d", cantidadHornos);

	uint32_t cantidadPedidos;
	recv(socketConexion, &cantidadPedidos, sizeof(uint32_t), MSG_WAITALL);

	log_info(logger, "Cantidad de pedidos: %d", cantidadPedidos);

}

void enviar_consultar_platos(consultar_platos* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_PLATOS);

	serializar_consultar_platos(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	uint32_t op_code;
	recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

	uint32_t buffer_size;
	recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	uint32_t cantPlatos;
	recv(socketConexion, &cantPlatos, sizeof(uint32_t), MSG_WAITALL);

	log_info(logger, "-Platos del restaurante %s-",msg->nombreRestaurante);

	uint32_t tamanioNombre = 0;
	if (cantPlatos > 0) {
		for (int i = 0; i < cantPlatos; i++) {
			uint32_t precioPlato;

			recv(socketConexion, &tamanioNombre, sizeof(uint32_t), MSG_WAITALL);
			char* nombrePlato = malloc(tamanioNombre);

			recv(socketConexion, nombrePlato, tamanioNombre, MSG_WAITALL);

			log_info(logger, "%s", nombrePlato);

			recv(socketConexion, &precioPlato, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "Precio: %d", precioPlato);

		free(nombrePlato);
			}
		} else {

			log_info(logger, "El restaurante no tiene platos");
		}
}

void enviar_crear_pedido(uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(CREAR_PEDIDO);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	uint32_t op_code;
	recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

	uint32_t buffer_size;
	recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	t_respuesta2 respuesta;
	recv(socketConexion, &respuesta, sizeof(t_respuesta2), MSG_WAITALL);

	uint32_t idPedido;
	recv(socketConexion, &idPedido, sizeof(uint32_t), MSG_WAITALL);

	if (respuesta == OK2) {

		log_info(logger, "Pedido %d creado", idPedido);

	}else {

		log_info(logger, "Error al crear pedido");

	}
}

void enviar_guardar_pedido(guardar_pedido* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(GUARDAR_PEDIDO);

	serializar_guardar_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {

		log_info(logger, "Pedido guardado");

	}else {

		log_info(logger, "Error al guardar pedido");

	}
}

void enviar_aniadir_plato(aniadir_plato* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(ANIADIR_PLATO);

	serializar_aniadir_plato(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {

		log_info(logger, "Plato aniadido con exito");

	}else {

		log_info(logger, "Error al aniadir plato");

	}
}

void enviar_guardar_plato(guardar_plato* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(GUARDAR_PLATO);

	serializar_guardar_plato(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {

		log_info(logger, "Plato guardado con exito");

	}else {

		log_info(logger, "Error al guardar plato");

	}
}


void enviar_confirmar_pedido(confirmar_pedido* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(CONFIRMAR_PEDIDO);

	serializar_confirmar_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {
		log_info(logger, "Pedido confirmado");
	} else {
		log_info(logger, "Error al confirmar pedido");
	}
}

void enviar_confirmar_pedido_restaurante(confirmar_pedido* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(CONFIRMAR_PEDIDO);

	serializar_confirmar_pedido_restaurante(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {
		log_info(logger, "Pedido confirmado");
	} else {
		log_info(logger, "Error al confirmar pedido");
	}
}

void enviar_plato_listo(plato_listo* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(PLATO_LISTO);

	serializar_plato_listo(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {
		log_info(logger, "Plato listo agregado con exito");
	} else {
		log_info(logger, "Error al agregar plato listo");
	}
}

void enviar_consultar_pedido(consultar_pedido* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_PEDIDO);

	serializar_consultar_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	uint32_t op_code;
	recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

	uint32_t buffer_size;
	recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	uint32_t tamanioNombreRestaurante;
	recv(socketConexion, &tamanioNombreRestaurante, sizeof(uint32_t), MSG_WAITALL);

	char* nombreRestaurante = malloc(tamanioNombreRestaurante);
	recv(socketConexion, nombreRestaurante, tamanioNombreRestaurante, MSG_WAITALL);

	log_info(logger,"Restaurante: %s", nombreRestaurante);

	free(nombreRestaurante);

	estado_pedido estadoPedido;
	recv(socketConexion, &estadoPedido, sizeof(estado_pedido), MSG_WAITALL);

	switch(estadoPedido){
	case PENDIENTE:
		log_info(logger, "Estado pedido: Pendiente");
		break;
	case CONFIRMADO:
		log_info(logger, "Estado pedido: Confirmado");
		break;
	case TERMINADO:
		log_info(logger, "Estado pedido: Terminado");
		break;
	default:
		break;
	}

	uint32_t cantidadPlatos;
	recv(socketConexion, &cantidadPlatos, sizeof(uint32_t), MSG_WAITALL);

	if (cantidadPlatos>0) {
		for (int i = 0; i < cantidadPlatos; i++) {

			uint32_t tamanioPlato;
			recv(socketConexion, &tamanioPlato, sizeof(uint32_t), MSG_WAITALL);

			char* nombrePlato = malloc(tamanioPlato);
			recv(socketConexion, nombrePlato, tamanioPlato, MSG_WAITALL);

			log_info(logger, "Plato: %s", nombrePlato);

			uint32_t cantidadPlato;
			recv(socketConexion, &cantidadPlato, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "Cantidad plato: %d", cantidadPlato);

			uint32_t cantidadLista;
			recv(socketConexion, &cantidadLista, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "Cantidad lista: %d", cantidadLista);

			free(nombrePlato);

		}
	}


}

void enviar_obtener_pedido(obtener_pedido* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_PEDIDO);

	serializar_obtener_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {
		log_info(logger, "Obtener pedido con exito");

		uint32_t op_code;
		recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

		uint32_t buffer_size;
		recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

		estado_pedido estadoPedido;
		recv(socketConexion, &estadoPedido, sizeof(estado_pedido), MSG_WAITALL);

		switch (estadoPedido) {
		case PENDIENTE:
			log_info(logger, "Estado pedido: Pendiente");
			break;
		case CONFIRMADO:
			log_info(logger, "Estado pedido: Confirmado");
			break;
		case TERMINADO:
			log_info(logger, "Estado pedido: Terminado");
			break;
		default:
			break;
		}

		uint32_t cantidadPlatos;
		recv(socketConexion, &cantidadPlatos, sizeof(uint32_t), MSG_WAITALL);

		for (int i = 0; i < cantidadPlatos; i++) {

			uint32_t tamanioPlato;
			recv(socketConexion, &tamanioPlato, sizeof(uint32_t), MSG_WAITALL);

			char* nombrePlato = malloc(tamanioPlato);
			recv(socketConexion, nombrePlato, tamanioPlato, MSG_WAITALL);

			log_info(logger, "Plato: %s", nombrePlato);

			uint32_t cantidadPlato;
			recv(socketConexion, &cantidadPlato, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "Cantidad plato: %d", cantidadPlato);

			uint32_t cantidadLista;
			recv(socketConexion, &cantidadLista, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "Cantidad lista: %d", cantidadLista);

			free(nombrePlato);

		}

//		uint32_t precioTotal;
//
//		recv(socketConexion, &precioTotal, sizeof(uint32_t), MSG_WAITALL);
//
//		log_info(logger, "Precio total: %d", precioTotal);

	} else {
		log_info(logger, "Error al obtener pedido");
	}

}

void enviar_terminar_pedido(terminar_pedido* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(TERMINAR_PEDIDO);

	serializar_terminar_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {
		log_info(logger, "Pedido terminado");
	} else {
		log_info(logger, "Error al terminar pedido");
	}
}

void enviar_finalizar_pedido(finalizar_pedido* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(FINALIZAR_PEDIDO);

	serializar_finalizar_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	t_respuesta2 respuesta = recibir_respuesta(socketConexion);

	if (respuesta == OK2) {
		log_info(logger, "Pedido finalizado");
	} else {
		log_info(logger, "Error al finalizar pedido");
	}
}

void enviar_obtener_receta(obtener_receta* msg, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_RECETA);

	serializar_obtener_receta(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

	uint32_t buffer_size;
	recv(socketConexionSindicato, &buffer_size, sizeof(uint32_t),MSG_WAITALL);

	t_receta_respuesta* receta = deserializar_obtener_receta_respuesta(socketConexionSindicato);

	log_info(logger, "Se recibio la receta y sus pasos son:");

	for (int i = 0; i < list_size(receta->listaPasos); i++) {
		log_info(logger, "Paso %d, %s", i + 1,list_get(receta->listaPasos, i));
	}
}

//-----------------------------------------------------//

t_respuesta2 recibir_respuesta(uint32_t socketConexion) {
	uint32_t op_code;
	recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);
	uint32_t buffer_size;
	recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);
	t_respuesta2 respuesta;
	recv(socketConexion, &respuesta, sizeof(t_respuesta2), MSG_WAITALL);
	return respuesta;
}
//------------------Serializacion----------------------//

void serializar_datos_cliente(t_buffer* buffer) {
	//1. ID Cliente
	//2. Posicion
	//3. Cod mensaje

	buffer->size = strlen(idCliente) + 1 + sizeof(uint32_t)*3;

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	int tamanioId = strlen(idCliente) + 1;

	memcpy(buffer->stream + offset, &(tamanioId), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, idCliente, tamanioId);
	offset += tamanioId;

	memcpy(buffer->stream + offset, &(posicionX), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(posicionY), sizeof(uint32_t));
}

void serializar_seleccionar_restaurante(seleccionar_restaurante* msg,t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante

	buffer->size = strlen(msg->nombreRestaurante) + 1 + sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
}

void serializar_obtener_restaurante(obtener_restaurante* msg, t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante

	buffer->size = strlen(msg->nombreRestaurante) + 1 + sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
}

void serializar_consultar_platos(consultar_platos* msg, t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante

	buffer->size = strlen(msg->nombreRestaurante) + 1 + sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
}

void serializar_guardar_pedido(guardar_pedido* msg, t_buffer* buffer) {
	//0. ID Pedido
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante

	buffer->size = strlen(msg->nombreRestaurante) + 1 + 2 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), (msg->tamanioNombre));

}

void serializar_aniadir_plato(aniadir_plato* msg, t_buffer* buffer) {
	//1. Tamanio plato
	//2. Nombre Plato
	//3. ID Pedido

	buffer->size = strlen(msg->plato) + 1 + 2 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioPlato), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->plato), msg->tamanioPlato);
	offset += (strlen(msg->plato) + 1);

	memcpy(buffer->stream + offset, &(msg->idPedido), sizeof(uint32_t));
}

void serializar_guardar_plato(guardar_plato* msg, t_buffer* buffer) {
	//1. Tamanio Nombre Restaurante
	//2. Nombre Restaurante;
	//3. Tamanio Nombre Plato
	//4. Plato
	//5. Cantidad
	//6. Id Pedido

	buffer->size = strlen(msg->nombreRestaurante) + strlen(msg->nombrePlato) + 2 + 4 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante),msg->tamanioNombre);
	offset += (strlen(msg->nombreRestaurante) + 1);

	memcpy(buffer->stream + offset, &(msg->tamanioNombrePlato),sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombrePlato),msg->tamanioNombrePlato);
	offset += (strlen(msg->nombrePlato) + 1);

	memcpy(buffer->stream + offset, &(msg->cantidad_platos),sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));

}

void serializar_confirmar_pedido(confirmar_pedido* msg, t_buffer* buffer) {
	//0. ID Pedido
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante

	buffer->size = strlen(msg->nombreRestaurante) + 1 + 2 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
}

void serializar_confirmar_pedido_restaurante(confirmar_pedido* msg, t_buffer* buffer) {
	//1. id Pedido

	buffer->size = sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));

}

void serializar_plato_listo(plato_listo* msg, t_buffer* buffer) {
	//1. Tamanio Nombre Restaurante
	//2. Nombre Restaurante;
	//3. Tamanio Nombre Plato
	//4. Plato
	//5. id Pedido


	buffer->size = strlen(msg->nombreRestaurante) + strlen(msg->nombrePlato) + 2 + 3 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante),msg->tamanioNombre);
	offset += (strlen(msg->nombreRestaurante) + 1);

	memcpy(buffer->stream + offset, &(msg->tamanioNombrePlato),sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombrePlato),msg->tamanioNombrePlato);
	offset += (strlen(msg->nombrePlato) + 1);

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));

}

void serializar_consultar_pedido(consultar_pedido* msg, t_buffer* buffer) {
	//1. id Pedido

	buffer->size = sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));

}

void serializar_obtener_pedido(obtener_pedido* msg, t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante
	//3. ID Pedido

	buffer->size = strlen(msg->nombreRestaurante) + 1 + 2 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
	offset += (strlen(msg->nombreRestaurante) + 1);

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
}

void serializar_finalizar_pedido(finalizar_pedido* msg, t_buffer* buffer) {
	//1. ID Pedido
	//2. Tamanio nombre restaurante
	//3. Nombre restaurante

	buffer->size = strlen(msg->nombreRestaurante) + 1 + 2 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante),msg->tamanioNombre);

}

void serializar_terminar_pedido(terminar_pedido* msg, t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante
	//3. ID Pedido

	buffer->size = strlen(msg->nombreRestaurante) + 1 + 2 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante),
			msg->tamanioNombre);
	offset += (strlen(msg->nombreRestaurante) + 1);

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
}

void serializar_obtener_receta(obtener_receta* msg, t_buffer* buffer) {
	//1. Tamanio nombre plato
	//2. Nombre plato

	buffer->size = strlen(msg->nombreReceta) + 1 + sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreReceta), msg->tamanioNombre);
}
//----------------------------------------------------//

//----------------Escucha pedido---------------------//

void confirmar_pedido_app(void* msg){
	uint32_t socket = crear_socket_app();

	confirmar_pedido* mensaje = (confirmar_pedido*) msg;

	enviar_datos_cliente(socket);
	enviar_confirmar_pedido(mensaje, socket);

	free(mensaje);

	uint32_t op_code;
	recv(socket, &op_code, sizeof(uint32_t), MSG_WAITALL);

	procesar_mensaje_recibido(op_code, socket);

}

void confirmar_pedido_restaurante(void* msg){

	uint32_t socketConexion = crear_conexion(ipRestaurante,puertoRestaurante);
	if (socketConexion == -1) {
	 	log_info(logger, "No me pude conectar al Restaurante");
	} else {
		log_info(logger, "Me conecte exitosamente al Restaurante");
	}

	confirmar_pedido* mensaje = (confirmar_pedido*) msg;

	enviar_confirmar_pedido_restaurante(mensaje, socketConexion);

	free(mensaje);

	uint32_t op_code;
	recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

	procesar_mensaje_recibido(op_code, socketConexion);
}
