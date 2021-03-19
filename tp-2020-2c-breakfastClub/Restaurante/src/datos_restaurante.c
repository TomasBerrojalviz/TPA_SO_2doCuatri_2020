#include "datos_restaurante.h"

// ENVIO DE MENSAJES

void enviar_sindicato_guardar_pedido(guardar_pedido* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(GUARDAR_PEDIDO);

	serializar_sindicato_guardar_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_sindicato_guardar_plato(guardar_plato* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(GUARDAR_PLATO);
	
	serializar_sindicato_guardar_plato(msg, paquete_a_enviar->buffer);
	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_sindicato_guardar_pedido(guardar_pedido* msg, t_buffer* buffer){

	//1. Tamanio nombre restaurante
	//2. Nombre restaurante
	//3. ID del pedido

	buffer->size = msg->tamanioNombre + 2*sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, msg->nombreRestaurante, msg->tamanioNombre);
	offset += msg->tamanioNombre;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
}

void serializar_sindicato_guardar_plato(guardar_plato* msg, t_buffer* buffer){

	//------------ORDEN------------
			//1. Tamaño Nombre Restaurante
			//2. Nombre Restaurante
			//3. Tamaño Nombre Plato
			//4. Nombre Plato
			//5. Cantidad de Platos
			//6. ID Pedido
	//-----------------------------

	buffer->size = sizeof(uint32_t)*4 + msg->tamanioNombrePlato + msg->tamanioNombre;

	buffer->stream = malloc(buffer->size);// todo memleak
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, msg->nombreRestaurante, msg->tamanioNombre);
	offset += msg->tamanioNombre;

	memcpy(buffer->stream + offset, &(msg->tamanioNombrePlato), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, msg->nombrePlato, msg->tamanioNombrePlato);
	offset += msg->tamanioNombrePlato;

	memcpy(buffer->stream + offset, &(msg->cantidad_platos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
}

void enviar_app_nombre_restaurante(uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(NOMBRE_RESTAURANTE);

	serializar_app_nombre_restaurante(paquete_a_enviar->buffer);
	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_app_nombre_restaurante(t_buffer* buffer){

	//1. Tamanio nombre restaurante
	//2. Nombre restaurante

	uint32_t tamanioNombre = strlen(nombreRestaurante) + 1;

	buffer->size = tamanioNombre + 3*sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, nombreRestaurante, tamanioNombre);
	offset += tamanioNombre;

	memcpy(buffer->stream + offset, &(posicionRestaurante.posX), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(posicionRestaurante.posY), sizeof(uint32_t));
}

//void serializar_sindicato_confirmar_pedido(confirmar_pedido* msg, t_buffer* buffer) {
//    //1. ID Pedido
//    //2. Tamanio nombre restaurante
//    //3. Nombre restaurante
//
//
//    buffer->size = strlen(msg->nombreRestaurante) + 1 + 2*sizeof(uint32_t);
//
//    buffer->stream = malloc(buffer->size);
//    int offset = 0;
//
//    memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
//    offset += sizeof(uint32_t);
//
//    memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
//    offset += sizeof(uint32_t);
//
//    memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
//
//}

void enviar_sindicato_confirmar_pedido(confirmar_pedido* msg, uint32_t socketConexion){
    t_paquete* paquete_a_enviar = crear_paquete(CONFIRMAR_PEDIDO);

    serializar_sindicato_confirmar_pedido(msg, paquete_a_enviar->buffer);

    int tamanio_a_enviar = 0;

    void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

    send(socketConexion, a_enviar, tamanio_a_enviar, 0);

    free(paquete_a_enviar->buffer);
    free(paquete_a_enviar);
    free(a_enviar);
}

void enviar_sindicato_obtener_restaurante(t_restaurante* obtenerRestaurante, uint32_t socketSindicato) {
	log_info(logger, "RESTAURANTE :: ENVIO MENSAJE: OBTENER_RESTAURANTE");

	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_RESTAURANTE);

	serializar_obtener_restaurante(obtenerRestaurante, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketSindicato, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

}

void enviar_sindicato_consultar_platos(t_restaurante* msg, uint32_t socketSindicato){
	log_info(logger, "RESTAURANTE :: ENVIO MENSAJE: CONSULTAR_PLATOS");
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_PLATOS);

	serializar_consultar_platos(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketSindicato, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}


void enviar_sindicato_obtener_receta(char* nombrePlato, uint32_t socketSindicato) {
	log_info(logger, "RESTAURANTE :: ENVIO MENSAJE: OBTENER_RECETA");
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_RECETA);

	serializar_obtener_receta(nombrePlato, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketSindicato, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_finalizar_pedido(uint32_t id_pedido, uint32_t socketSolicitante) {
	log_info(logger, "RESTAURANTE :: ENVIO MENSAJE: FINALIZAR_PEDIDO");
	t_paquete* paquete_a_enviar = crear_paquete(FINALIZAR_PEDIDO);

	serializar_finalizar_pedido(id_pedido, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketSolicitante, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_finalizar_pedido(uint32_t id_pedido, t_buffer* buffer) {
    //1. ID Pedido
    //2. Tamanio nombre restaurante
    //3. Nombre restaurante


    buffer->size = strlen(nombreRestaurante) + 1 + 2*sizeof(uint32_t);

    uint32_t tamanioNombre = strlen(nombreRestaurante) + 1;

    buffer->stream = malloc(buffer->size);
    int offset = 0;

    memcpy(buffer->stream + offset, &(id_pedido), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer->stream + offset, &(tamanioNombre), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer->stream + offset, (nombreRestaurante), tamanioNombre);

}

void serializar_obtener_receta(char* nombrePlato, t_buffer* buffer){
	    //1. Tamanio nombre plato
	    //2. Nombre plato

	uint32_t tamanioPlato = strlen(nombrePlato) + 1;

	buffer->size = tamanioPlato + sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(tamanioPlato), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, nombrePlato, tamanioPlato);

}

void serializar_sindicato_confirmar_pedido(confirmar_pedido* msg, t_buffer* buffer) {
    //1. ID Pedido
    //2. Tamanio nombre restaurante
    //3. Nombre restaurante


    buffer->size = strlen(msg->nombreRestaurante) + 1 + 2*sizeof(uint32_t);

    buffer->stream = malloc(buffer->size);
    int offset = 0;

    memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);

}


void serializar_obtener_restaurante(t_restaurante* msg, t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante

	buffer->size = strlen(msg->nombreRestaurante) + 1 + sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
}

void serializar_consultar_platos(t_restaurante* msg, t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante

	buffer->size = strlen(msg->nombreRestaurante) + 1 + sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);// todo memleak
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
}

confirmar_pedido* deserializar_confirmar_pedido(int socket_cliente) {

	//------------ORDEN------------
	//1. ID Pedido
	//2. tamanioNombre
	//3. nombreRestaurante
	//-----------------------------

	log_info(logger, "Entra en la Funcion");

	confirmar_pedido* msg_recibido = malloc(sizeof(confirmar_pedido));

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);

	msg_recibido->tamanioNombre = strlen(nombreRestaurante) +1;
	msg_recibido->nombreRestaurante = string_duplicate(nombreRestaurante);


	return msg_recibido;
}

void enviar_sindicato_obtener_pedido(confirmar_pedido* msg, uint32_t socketSindicato) {
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_PEDIDO);

	serializar_sindicato_obtener_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketSindicato, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_sindicato_obtener_pedido(confirmar_pedido* msg, t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante
	//3. ID Pedido

	buffer->size = strlen(msg->nombreRestaurante) + 1 + 2*sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
	offset += msg->tamanioNombre;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
}

consultar_pedido* deserializar_consultar_pedido(int socket_cliente){
	consultar_pedido* msg= malloc(sizeof(consultar_pedido));
	recv(socket_cliente, &(msg->idPedido), sizeof(uint32_t), 0);
	return msg;
}

obtener_pedido_respuesta* deserializar_obtener_pedido_respuesta(uint32_t socket_cliente){

	//------------ORDEN------------
			//1. Cantidad de platos
			//2. Estado
			//3. Tamanio Plato  }
			//3. Platos         } COMO SON LISTAS IRAN
			//4. Cant x Platos  }   DENTRO DE UN FOR
			//5. Cant Lista     }
			//6. Precio Total
	//-----------------------------

	obtener_pedido_respuesta* msg_recibido = malloc(sizeof(obtener_pedido_respuesta));

	msg_recibido->listaCantidadDePedido = list_create();
	msg_recibido->listaCantidadLista = list_create();
	msg_recibido->listaPlatos = list_create();
	msg_recibido->listaTamanioPlato = list_create();

	uint32_t cant;

	recv(socket_cliente, &(msg_recibido->estado), sizeof(estado_pedido), 0);

	recv(socket_cliente, &(cant), sizeof(uint32_t), 0);

	for (int i = 0; i < cant; ++i) {
		uint32_t aux;

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaTamanioPlato, aux);
		char* auxPlatos = malloc(aux);

		recv(socket_cliente, auxPlatos, aux, 0);
		list_add(msg_recibido->listaPlatos, auxPlatos);

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaCantidadDePedido, aux);

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaCantidadLista, aux);

	}

//	recv(socket_cliente, &(msg_recibido->precioTotal), sizeof(uint32_t), 0);

	return msg_recibido;
}

consultar_pedido_respuesta* deserializar_consultar_pedido_respuesta(uint32_t socket_cliente){
	//------------ORDEN------------
			//1. Cantidad de platos
			//2. Estado
			//3. Tamanio Plato  }
			//3. Platos         } COMO SON LISTAS IRAN
			//4. Cant x Platos  }   DENTRO DE UN FOR
			//5. Cant Lista     }
			//6. Precio Total
	//-----------------------------

	consultar_pedido_respuesta* msg_recibido = malloc(sizeof(consultar_pedido_respuesta));
	msg_recibido->listaCantidadDePedido = list_create();
	msg_recibido->listaCantidadLista = list_create();
	msg_recibido->listaPlatos = list_create();
	msg_recibido->listaTamanioPlato = list_create();

	uint32_t cant;
	uint32_t aux=0;
	recv(socket_cliente, &(msg_recibido->estado), sizeof(estado_pedido), 0);

	recv(socket_cliente, &(cant), sizeof(uint32_t), 0);



	for (int i = 0; i < cant; i++) {

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaTamanioPlato, aux);
		char* auxPlatos = malloc(aux);

		recv(socket_cliente, auxPlatos, aux, 0);
		list_add(msg_recibido->listaPlatos, auxPlatos);

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaCantidadDePedido, aux);

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaCantidadLista, aux);
	}

//	recv(socket_cliente, &(msg_recibido->precioTotal), sizeof(uint32_t), 0);

	return msg_recibido;
}

obtener_restaurante_respuesta* deserializar_obtener_restaurante_respuesta(uint32_t socket_cliente){

	//------------ORDEN------------
			//1. Tamaño Nombre
			//2. Nombre
			//3. Cantidad cocineros
			//4. Posicion en X
			//5. Posicion en Y
			//6. Cantidad Afinidades
			//7. listaTamanioAfinidades } COMO SON LISTAS IRAN
			//8. afinidad_cocineros		}   DENTRO DE UN FOR
			//9. Cantidad platos
			//10. listaTamanioPlatos	} COMO SON LISTAS IRAN
			//11. platos				}   DENTRO DE UN FOR
			//12. precio_platos			}
			//13. cantidad_hornos
	//-----------------------------

	log_info(logger, "RESTAURANTE :: ESTOY EN deserializar_obtener_restaurante_respuesta");

	obtener_restaurante_respuesta* msg_recibido = malloc(sizeof(obtener_restaurante_respuesta));


	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);

	if(msg_recibido->tamanioNombre != -1){

		uint32_t cantidadAfinidades;
		uint32_t cantidadPlatos;
		uint32_t aux;
		msg_recibido->listaTamanioAfinidades = list_create();
		msg_recibido->afinidad_cocineros = list_create();
		msg_recibido->listaTamanioPlatos = list_create();
		msg_recibido->platos = list_create();
		msg_recibido->precio_platos = list_create();

		log_info(logger, "RESTAURANTE :: ESTOY EN deserializar_obtener_restaurante_respuesta");

		msg_recibido->nombre_restaurante = malloc(msg_recibido->tamanioNombre);
		recv(socket_cliente, msg_recibido->nombre_restaurante,
				msg_recibido->tamanioNombre, 0);

		recv(socket_cliente, &(msg_recibido->cantidad_cocineros),
				sizeof(uint32_t), 0);

		recv(socket_cliente, &(msg_recibido->posicion.posX), sizeof(uint32_t),
				0);
		recv(socket_cliente, &(msg_recibido->posicion.posY), sizeof(uint32_t),
				0);

		recv(socket_cliente, &(cantidadAfinidades), sizeof(uint32_t), 0);
		for (int i = 0; i < cantidadAfinidades; i++) {
			recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
			list_add(msg_recibido->listaTamanioAfinidades, aux);

			char* auxAfinidades = malloc(aux);
			recv(socket_cliente, auxAfinidades, aux, 0);

			list_add(msg_recibido->afinidad_cocineros, auxAfinidades);
		}

		recv(socket_cliente, &(cantidadPlatos), sizeof(uint32_t), 0);
		for (int i = 0; i < cantidadPlatos; i++) {
			recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
			list_add(msg_recibido->listaTamanioPlatos, aux);

			char* auxPlatos = malloc(aux);
			recv(socket_cliente, auxPlatos, aux, 0);
			list_add(msg_recibido->platos, auxPlatos);

			recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
			list_add(msg_recibido->precio_platos, aux);
		}

		recv(socket_cliente, &(msg_recibido->cantidad_hornos), sizeof(uint32_t),
				0);

		recv(socket_cliente, &(msg_recibido->cantidadPedidos), sizeof(uint32_t),
				0);

		return msg_recibido;
	}

	log_info(logger, "RESTAURANTE :: NO SE PUDIERON OBTENER LOS DATOS :: FINALIZANDO");

	exit(0);
}

consultar_platos_respuesta* deserializar_consultar_platos_respuesta(uint32_t socket_cliente){
	//------------ORDEN------------
			//1. Cantidad platos
			//2. listaTamanioPlatos		} COMO SON LISTAS IRAN
			//3. platos					}   DENTRO DE UN FOR
			//4. precio_platos			}
	//-----------------------------

	consultar_platos_respuesta* msg_recibido = malloc(sizeof(consultar_platos_respuesta));
	msg_recibido->listaTamanioPlatos = list_create();
	msg_recibido->platos = list_create();
	msg_recibido->precio_platos = list_create();

	uint32_t cantidadPlatos;
	uint32_t aux;

	recv(socket_cliente, &(cantidadPlatos), sizeof(uint32_t), 0);

	for (int i = 0; i < cantidadPlatos; ++i) {
		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaTamanioPlatos, aux);

		char* auxPlatos = malloc(aux);
		recv(socket_cliente, auxPlatos, aux, 0);
		list_add(msg_recibido->platos, auxPlatos);

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->precio_platos, aux);
	}

	return msg_recibido;
}

void enviar_app_consultar_platos_respuesta(consultar_platos_respuesta* consultarPlatoRespuesta,  uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_PLATOS);

	serializar_consultar_platos_respuesta(consultarPlatoRespuesta, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_obtener_pedido_respuesta(obtener_pedido_respuesta* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_PEDIDO_RESPUESTA);

	serializar_obtener_pedido_respuesta(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_obtener_pedido_respuesta(obtener_pedido_respuesta* msg, t_buffer* buffer) {
	//------------ORDEN------------
			//1. tamanioNombre
			//2. Nombre
			//3. Estado
			//4. Cantidad de platos
			//5. Tamanio Plato  }
			//6. Platos         } COMO SON LISTAS IRAN
			//7. Cant x Platos  }   DENTRO DE UN FOR
			//8. Cant Lista     }
			//9. Precio Total
	//-----------------------------

	log_info(logger,"SINDICATO :: ENTRAMOS AL serializar_restaurante_obtener_pedido_respuesta");
	uint32_t cant;
	uint32_t aux = strlen(nombreRestaurante)+1;

	if(msg->listaPlatos)
		cant = list_size(msg->listaPlatos);
	else
		cant = 0;

	log_info(logger,"SINDICATO :: cant: %d", cant);

	buffer->size = sizeof(estado_pedido) + sizeof(uint32_t)*cant*2 + 3*sizeof(uint32_t) + aux;

	for (int i = 0; i < cant; i++) {
		buffer->size =+ (uint32_t) list_get(msg->listaTamanioPlato, i);
	}

	buffer->stream = malloc(buffer->size);
	int offset = 0;
	memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, nombreRestaurante, aux);
	offset += aux;

	memcpy(buffer->stream + offset, &(msg->estado), sizeof(estado_pedido));
	offset += sizeof(estado_pedido);

	memcpy(buffer->stream + offset, &(cant), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < cant; i++) {

		aux = (uint32_t) list_get(msg->listaTamanioPlato, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, (char*) list_get(msg->listaPlatos, i), aux);
		offset += aux;

		aux = (uint32_t) list_get(msg->listaCantidadDePedido, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		aux = (uint32_t) list_get(msg->listaCantidadLista, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	memcpy(buffer->stream + offset, &(msg->precioTotal), sizeof(uint32_t));
}

aniadir_plato*  deserializar_aniadir_plato(int socket_cliente){
	//------------ORDEN------------
			//1. Tamaño Plato
			//2. Nombre Plato
			//3. Id Pedido
	//-----------------------------

	aniadir_plato* msg_recibido = malloc(sizeof(aniadir_plato));// todo memleak
	recv(socket_cliente, &(msg_recibido->tamanioPlato), sizeof(uint32_t), 0);
	msg_recibido->plato = malloc(msg_recibido->tamanioPlato);

	recv(socket_cliente, msg_recibido->plato, msg_recibido->tamanioPlato, 0);

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);


	return msg_recibido;
}
void serializar_consultar_platos_respuesta(consultar_platos_respuesta* msg, t_buffer* buffer){

	//------------ORDEN------------
		//1. Cantidad platos
		//2. listaTamanioPlatos    } COMO SON LISTAS IRAN
		//3. platos                }   DENTRO DE UN FOR
		//4. precio_platos         }
		//-----------------------------

	uint32_t cantidadPlatos = list_size(msg->platos);
	uint32_t offset = 0;
	uint32_t aux;

	log_info(logger, "Entramos a serializar_consultar_platos_respuesta");

	buffer->size = sizeof(uint32_t);

	for (int i = 0; i < cantidadPlatos; i++) {
		buffer->size += ((uint32_t) list_get(msg->listaTamanioPlatos, i)) + 2 * sizeof(uint32_t);
	}

	buffer->stream = malloc(buffer->size);
	log_info(logger, "malloc stream");

	memcpy(buffer->stream + offset, &(cantidadPlatos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	log_info(logger, "memcpy cantplatos");

	for (int i = 0; i < cantidadPlatos; i++) {
		log_info(logger, "entramos for");

		aux = (uint32_t) list_get(msg->listaTamanioPlatos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, list_get(msg->platos, i), aux);
		log_info(logger, "memcpy platos");
		offset += aux;

		aux = (uint32_t) list_get(msg->precio_platos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		log_info(logger, "memcpy platos");

		offset += sizeof(uint32_t);
	}
	log_info(logger, "salimos for");

}

t_receta_respuesta* deserializar_obtener_receta_respuesta(uint32_t socket_cliente){

	//------------ORDEN------------
			//1. Cantidad platos
			//2. listaTamanioPlatos		} COMO SON LISTAS IRAN
			//3. platos					}   DENTRO DE UN FOR
			//4. precio_platos			}
	//-----------------------------

	t_receta_respuesta* msg_recibido = malloc(sizeof(t_receta_respuesta));// todo memleak
	msg_recibido->listaTamanioPasos = list_create();
	msg_recibido->listaPasos = list_create();
	msg_recibido->listaTiempoPasos = list_create();

	uint32_t cantidadPasos;
	uint32_t aux;

	recv(socket_cliente, &(cantidadPasos), sizeof(uint32_t), 0);

	for (int i = 0; i < cantidadPasos; ++i) {
		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaTamanioPasos, aux);

		char* auxPasos = malloc(aux);
		recv(socket_cliente, auxPasos, aux, 0);
		list_add(msg_recibido->listaPasos, auxPasos);

		recv(socket_cliente, &(aux), sizeof(uint32_t), 0);
		list_add(msg_recibido->listaTiempoPasos, aux);
	}

	return msg_recibido;
}

void enviar_respuesta(t_respuesta2 confirmado, uint32_t socketConexion, op_code codigo){
	t_paquete* paquete_a_enviar = crear_paquete(codigo);

	paquete_a_enviar->buffer->size = sizeof(t_respuesta2);

	paquete_a_enviar->buffer->stream = malloc(paquete_a_enviar->buffer->size);// todo memleak

    memcpy(paquete_a_enviar->buffer->stream, &(confirmado), sizeof(t_respuesta2));
    log_info(logger,"respuesta: %d", confirmado);


	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}
void enviar_respuesta_pedido(t_respuesta2 confirmado, uint32_t id_pedido, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(RESPUESTA_CREAR_PEDIDO);

	paquete_a_enviar->buffer->size = sizeof(t_respuesta2) + sizeof(uint32_t);

	paquete_a_enviar->buffer->stream = malloc(paquete_a_enviar->buffer->size);// todo memleak

    memcpy(paquete_a_enviar->buffer->stream, &(confirmado), sizeof(t_respuesta2));
    log_info(logger,"respuesta: %d", confirmado);

    memcpy(paquete_a_enviar->buffer->stream + sizeof(t_respuesta2), &(id_pedido), sizeof(uint32_t));

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

char* dar_estado_pedido(estado_pedido estado){
	switch(estado){
		case PENDIENTE:
			return "pendiente";
		case CONFIRMADO:
			return "confirmado";
		case TERMINADO:
			return "terminado";
	}
	return NULL;
}


void enviar_sindicato_terminar_pedido(terminar_pedido* msg, uint32_t socketSindicato) {
	t_paquete* paquete_a_enviar = crear_paquete(TERMINAR_PEDIDO);

	serializar_sindicato_terminar_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketSindicato, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_sindicato_terminar_pedido(terminar_pedido* msg, t_buffer* buffer) {
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante
	//3. ID Pedido

	buffer->size = strlen(msg->nombreRestaurante) + 1 + 2*sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante), msg->tamanioNombre);
	offset += msg->tamanioNombre;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
}

void enviar_sindicato_plato_listo(plato_listo* msg, uint32_t socketConexion){

	t_paquete* paquete_a_enviar = crear_paquete(PLATO_LISTO);

	serializar_plato_listo(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_plato_listo(plato_listo* msg, t_buffer* buffer){
	//1. Tamanio Nombre Restaurante
	//2. Nombre Restaurante;
	//3. Tamanio Nombre Plato
	//4. Plato
	//5. id Pedido


	buffer->size = strlen(msg->nombreRestaurante) + strlen(msg->nombrePlato) + 2 + 3 * sizeof(uint32_t);

	buffer->stream = malloc(buffer->size);// todo memleak
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
