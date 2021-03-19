#include "datos_app.h"

//-----------DESSERIALIZACION--------------//

datos_cliente*  deserializar_datos_cliente(int socket_cliente){
	//------------ORDEN------------
			//1. Tamaño id
			//2. Id cliente
			//3. Posicion
	//-----------------------------

	datos_cliente* msg_recibido = malloc(sizeof(datos_cliente));

	recv(socket_cliente, &(msg_recibido->tamanioId), sizeof(uint32_t), MSG_WAITALL);
	msg_recibido->idCliente = malloc(msg_recibido->tamanioId);

	recv(socket_cliente, msg_recibido->idCliente, msg_recibido->tamanioId, MSG_WAITALL);

	recv(socket_cliente, &(msg_recibido->posCliente.posX), sizeof(uint32_t), MSG_WAITALL);

	recv(socket_cliente, &(msg_recibido->posCliente.posY), sizeof(uint32_t), MSG_WAITALL);

	return msg_recibido;
}

confirmar_pedido* deserializar_confirmar_pedido(int socket_cliente) {

	//------------ORDEN------------
	//1. ID Pedido
	//2. tamanioNombre
	//3. nombreRestaurante
	//-----------------------------


	confirmar_pedido* msg_recibido = malloc(sizeof(confirmar_pedido));

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), MSG_WAITALL);

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), MSG_WAITALL);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, MSG_WAITALL);

	return msg_recibido;
}

seleccionar_restaurante* deserializar_seleccionar_restaurante(int socket_cliente){

	//------------ORDEN------------
			//1. Tamaño nombre
			//2. Nombre
	//-----------------------------

	seleccionar_restaurante* msg_recibido = malloc(sizeof(seleccionar_restaurante));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), MSG_WAITALL);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, MSG_WAITALL);

	return msg_recibido;
}

consultar_platos* deserializar_consultar_platos(int socket_cliente){

	//------------ORDEN------------
			//1. Tamaño nombre
			//2. Nombre
	//-----------------------------

	consultar_platos* msg_recibido = malloc(sizeof(consultar_platos));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), MSG_WAITALL);
	msg_recibido->nombreRestaurante = malloc(sizeof(msg_recibido->tamanioNombre));

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, MSG_WAITALL);

	return msg_recibido;
}

consultar_pedido* deserializar_consultar_pedido(int socket_cliente){

	//------------ORDEN------------
			//1. ID pedido
	//-----------------------------

	consultar_pedido* msg_recibido = malloc(sizeof(consultar_pedido));

	recv(socket_cliente, &(msg_recibido->idPedido), sizeof(uint32_t), MSG_WAITALL);

	return msg_recibido;
}

aniadir_plato*  deserializar_aniadir_plato(int socket_cliente){
	//------------ORDEN------------
			//1. Tamaño Plato
			//2. Nombre Plato
			//3. Id Pedido
	//-----------------------------

	aniadir_plato* msg_recibido = malloc(sizeof(aniadir_plato));

	recv(socket_cliente, &(msg_recibido->tamanioPlato), sizeof(uint32_t), MSG_WAITALL);
	msg_recibido->nombrePlato = malloc(msg_recibido->tamanioPlato);

	recv(socket_cliente, msg_recibido->nombrePlato, msg_recibido->tamanioPlato, MSG_WAITALL);

	recv(socket_cliente, &(msg_recibido->idPedido), sizeof(uint32_t), MSG_WAITALL);

	return msg_recibido;
}

respuesta_consultar_platos* deserializar_respuesta_consultar_platos(int socket_cliente){
	//------------ORDEN------------
	//1. Cantidad platos
	//2. Tamanios plato
	//3. Platos
	//4. Precios plato
	//-----------------------------

	respuesta_consultar_platos* msg_recibido = malloc(sizeof(respuesta_consultar_platos));

	recv(socket_cliente, &(msg_recibido->cantPlatos), sizeof(uint32_t),MSG_WAITALL);

	for(int i=0; i < msg_recibido->cantPlatos; i++){
		recv(socket_cliente, &(msg_recibido->tamaniosPlato[i]), sizeof(uint32_t),MSG_WAITALL);
		msg_recibido->nombresPlato[i] = malloc(msg_recibido->tamaniosPlato[i]);

		recv(socket_cliente, msg_recibido->nombresPlato[i], msg_recibido->tamaniosPlato[i], MSG_WAITALL);

		recv(socket_cliente, &(msg_recibido->preciosPlato[i]), sizeof(uint32_t),MSG_WAITALL);
	}
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

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), MSG_WAITALL);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, MSG_WAITALL);

	recv(socket_cliente, &(msg_recibido->tamanioNombrePlato), sizeof(uint32_t), MSG_WAITALL);
	msg_recibido->nombrePlato = malloc(msg_recibido->tamanioNombrePlato);

	recv(socket_cliente, msg_recibido->nombrePlato, msg_recibido->tamanioNombrePlato, MSG_WAITALL);

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), MSG_WAITALL);

	return msg_recibido;
}

//-----------SERIALIZACION--------------//

void serializar_confirmar_pedido(confirmar_pedido* msg, t_buffer* buffer) {
	//1. id Pedido
	//2. tamanioNombre
	//3. nombreRestaurante

	buffer->size = sizeof(uint32_t)*2 + strlen(msg->nombreRestaurante) +1;

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante),msg->tamanioNombre);

}

void serializar_consultar_restaurantes(t_buffer* buffer){
	//0. Cantidad de veces de RECV
	//1. tamanioNombre
	//2. nombreRestaurante

	int cant = list_size(restaurantes_conectados);
	int offset = 0;

	if (cant > 0) {
		buffer->size = sizeof(uint32_t) + sizeof(uint32_t) * cant;

		for (int i = 0; i < cant; ++i) {
			buffer->size += ((restaurante_conectado*) list_get(restaurantes_conectados, i))->tamanioNombre +1;
		}

		buffer->stream = malloc(buffer->size);

		memcpy(buffer->stream + offset, &cant, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		for (int i = 0; i < cant; ++i) {
			memcpy(buffer->stream + offset, &(((restaurante_conectado*) list_get(restaurantes_conectados, i))->tamanioNombre), sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(buffer->stream + offset, (((restaurante_conectado*) list_get(restaurantes_conectados,i))->nombreRestaurante), ((restaurante_conectado*) list_get(restaurantes_conectados,i))->tamanioNombre);
			offset += ((restaurante_conectado*) list_get(restaurantes_conectados, i))->tamanioNombre;
		}
	} else {
		buffer->size = sizeof(uint32_t) + sizeof(uint32_t) + strlen("Default")+1;
		buffer->stream = malloc(buffer->size);

		memcpy(buffer->stream + offset, &cant, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		int tamanioDefault = strlen("Default")+1;
		memcpy(buffer->stream + offset, &tamanioDefault, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		char* nombreDefault = "Default";
		memcpy(buffer->stream + offset, nombreDefault, tamanioDefault);
	}
}

void serializar_guardar_pedido(guardar_pedido* msg, t_buffer* buffer) {
	//1. idPedido
	//2. tamanioNombre
	//3. nombreRestaurante

	buffer->size = sizeof(uint32_t)*2 + strlen(msg->nombreRestaurante) +1;

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->idPedido), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante),msg->tamanioNombre);

}

void serializar_aniadir_plato(aniadir_plato* msg, t_buffer* buffer) {
	//1. tamanioNombre
	//2. nombrePlato
	//3. id Pedido

	buffer->size = sizeof(uint32_t)*2 + strlen(msg->nombrePlato) +1;

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioPlato), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombrePlato),msg->tamanioPlato);
	offset += msg->tamanioPlato;

	memcpy(buffer->stream + offset, &(msg->idPedido), sizeof(uint32_t));
}

void serializar_guardar_plato(guardar_plato* msg, t_buffer* buffer) {
	//1. tamanioNombre
	//2. nombreRestaurante
	//3. Tamanio Plato
	//4. Nombre Plato
	//5. Cantidad
	//6. id Pedido

	buffer->size = sizeof(uint32_t)*4 + strlen(msg->nombreRestaurante) + strlen(msg->nombrePlato) + 2;

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante),msg->tamanioNombre);
	offset += msg->tamanioNombre;

	memcpy(buffer->stream + offset, &(msg->tamanioNombrePlato), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombrePlato),msg->tamanioNombrePlato);
	offset += msg->tamanioNombrePlato;

	memcpy(buffer->stream + offset, &(msg->cantidad_platos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
}

void serializar_obtener_pedido(obtener_pedido* msg, t_buffer* buffer) {
	//------------ORDEN------------
	//1. Tamanio nombre restaurante
	//2. Nombre restaurante
	//3. ID Pedido
	//-----------------------------

	buffer->size = sizeof(uint32_t)*2 + msg->tamanioNombre;

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (msg->nombreRestaurante),msg->tamanioNombre);
	offset += msg->tamanioNombre;

	memcpy(buffer->stream + offset, &(msg->id_pedido), sizeof(uint32_t));
}

void serializar_finalizar_pedido(t_pcb* pcb, t_buffer* buffer){
	//1. idPedido
	//2. tamanioNombre
	//3. nombreRestaurante

	uint32_t tamanioNombre = strlen(pcb->nombreRestaurante) + 1;

	buffer->size = sizeof(uint32_t)*2 + strlen(pcb->nombreRestaurante) + 1;

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(pcb->id_pedido), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, (pcb->nombreRestaurante),tamanioNombre);

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

void serializar_respuesta(t_respuesta2 rta, t_buffer* buffer){
	//1. Respuesta

	buffer->size = sizeof(t_respuesta2);

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(rta), sizeof(t_respuesta2));
}

void serializar_respuesta_platos(char** listaPlatos, t_buffer* buffer){
	//1. Cant platos
	//2. Tamanios nombre
	//3. Nombres plato
	//4. Precios plato

	buffer->size = sizeof(uint32_t);

	uint32_t cantPlatos = 0;
	uint32_t precioPlato = 5; //TODO REVISAR ESTO LO INVENTE
	int i = 0;
	while(listaPlatos[i] != NULL){
		cantPlatos ++;
		buffer->size += sizeof(uint32_t)*2 + strlen(listaPlatos[i]) + 1;
		i++;
	}

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &cantPlatos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int j = 0; j < cantPlatos; j++){
		uint32_t tamanioNombre = strlen(listaPlatos[j]);

		memcpy(buffer->stream + offset, &tamanioNombre, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, listaPlatos[j], tamanioNombre);
		offset += tamanioNombre;

		memcpy(buffer->stream + offset, &precioPlato, sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

}

void serializar_respuesta_id(t_respuesta2 rta, uint32_t id, t_buffer* buffer){
	//1. Respuesta
	//2.ID

		buffer->size = sizeof(t_respuesta2) + sizeof(uint32_t);

		buffer->stream = malloc(buffer->size);
		int offset = 0;

		memcpy(buffer->stream + offset, &(rta), sizeof(t_respuesta2));
		offset += sizeof(t_respuesta2);

		memcpy(buffer->stream + offset, &(id), sizeof(uint32_t));
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

void serializar_respuesta_consultar_platos(respuesta_consultar_platos* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. Cantidad platos
	//2. Tamanios plato
	//3. Platos
	//4. Precios plato
	//-----------------------------

	buffer->size = sizeof(uint32_t);

	for(int i = 0; i < msg->cantPlatos; i++){
		buffer->size += sizeof(uint32_t)*2 + msg->tamaniosPlato[i];
	}

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &msg->cantPlatos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int j = 0; j < msg->cantPlatos; j++){

		memcpy(buffer->stream + offset, &msg->tamaniosPlato[j], sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, msg->nombresPlato[j], msg->tamaniosPlato[j]);
		offset += msg->tamaniosPlato[j];

		memcpy(buffer->stream + offset, &msg->preciosPlato[j], sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

}

void serializar_respuesta_consultar_pedido(respuesta_pedido* respuesta, char* nombreRestaurante, t_buffer* buffer){
	//------------ORDEN------------
	//1. Tamanio nombre
	//2. Nombre restaurante
	//3. Estado pedido
	//4. Cantidad platos
	//5. Tamanio plato
	//6. Nombre plato
	//7. Cantidad plato
	//8. Cantidad lista
	//-----------------------------

	uint32_t tamanioNombre = strlen(nombreRestaurante) + 1;

	buffer->size = sizeof(uint32_t)*2 + sizeof(estado_pedido) + tamanioNombre;

	for(int i = 0; i < respuesta->cantidadPlatos; i++){
		plato* plato = list_get(respuesta->platos,i);
		buffer->size += sizeof(uint32_t)*3 + plato->tamanioNombre;
	}

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &tamanioNombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, nombreRestaurante, tamanioNombre);
	offset += tamanioNombre;

	memcpy(buffer->stream + offset, &respuesta->estadoPedido, sizeof(estado_pedido));
	offset += sizeof(estado_pedido);

	memcpy(buffer->stream + offset, &respuesta->cantidadPlatos, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int j = 0; j < respuesta->cantidadPlatos; j++){

		plato* plato = list_get(respuesta->platos,j);

		memcpy(buffer->stream + offset, &plato->tamanioNombre, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, plato->nombrePlato, plato->tamanioNombre);
		offset += plato->tamanioNombre;

		memcpy(buffer->stream + offset, &plato->cantidadPlato, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &plato->cantidadLista, sizeof(uint32_t));

	}

}

//-----------ENVIO--------------//

void enviar_respuesta_id(t_respuesta2 respuesta, int id, int cliente){
	t_paquete* paquete_a_enviar = crear_paquete(CREAR_PEDIDO);

		serializar_respuesta_id(respuesta, id, paquete_a_enviar->buffer);

		int tamanio_a_enviar = 0;

		void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

		send(cliente, a_enviar, tamanio_a_enviar, 0);

		free(paquete_a_enviar->buffer);
		free(paquete_a_enviar);
		free(a_enviar);
}

void enviar_respuesta(t_respuesta2 respuesta, int cliente){
	t_paquete* paquete_a_enviar = crear_paquete(CONFIRMAR_PEDIDO);

	serializar_respuesta(respuesta, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(cliente, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_respuesta_platos(char** listaPlatos, int cliente){
	t_paquete* paquete_a_enviar = crear_paquete(CONFIRMAR_PEDIDO);

	serializar_respuesta_platos(listaPlatos, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(cliente, a_enviar, tamanio_a_enviar, 0);

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

void enviar_crear_pedido(uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(CREAR_PEDIDO);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
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
}

void enviar_guardar_pedido(guardar_pedido* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(GUARDAR_PEDIDO);

	serializar_guardar_pedido(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_aniadir_plato(aniadir_plato* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(ANIADIR_PLATO);

	serializar_aniadir_plato(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_guardar_plato(guardar_plato* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(GUARDAR_PLATO);

	serializar_guardar_plato(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

respuesta_pedido* enviar_obtener_pedido(obtener_pedido* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_PEDIDO);

	serializar_obtener_pedido(msg, paquete_a_enviar->buffer);

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


	if (respuesta == OK2) {

		respuesta_pedido* respuesta = malloc(sizeof(respuesta_pedido));

		log_info(logger, "-Informacion del pedido-");

		uint32_t tamanioNombre = 0;

		uint32_t op_code;
		recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

		uint32_t buffer_size;
		recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

		estado_pedido estadoPedido;
		recv(socketConexion, &estadoPedido, sizeof(uint32_t), MSG_WAITALL);

		respuesta->estadoPedido = estadoPedido;

		uint32_t cantidad_elementos;
		recv(socketConexion, &cantidad_elementos, sizeof(uint32_t), MSG_WAITALL);

		respuesta->cantidadPlatos = cantidad_elementos;

		for (int i = 0; i < cantidad_elementos; i++) {
			int cantidadPlato;
			int cantidadLista;

			recv(socketConexion, &tamanioNombre, sizeof(uint32_t), MSG_WAITALL);
			char* nombrePlato = malloc(tamanioNombre);

			recv(socketConexion, &cantidadPlato, sizeof(uint32_t), MSG_WAITALL);

			recv(socketConexion, &cantidadLista, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "Plato: %s", nombrePlato);

			log_info(logger, "Cantidad pedida: %d", cantidadPlato);

			log_info(logger, "Cantidad lista: %d", cantidadLista);

			plato* plato = malloc(sizeof(plato));

			plato->tamanioNombre = tamanioNombre;
			plato->nombrePlato = nombrePlato;
			plato->cantidadPlato = cantidadPlato;
			plato->cantidadLista = cantidadLista;

			list_add(respuesta->platos, plato);
		}

		return respuesta;

	} else {
		log_info(logger,"Error al obtener pedido");

		return NULL;
	}
}

void enviar_restaurantes_conectados(uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(CONFIRMAR_PEDIDO);

	serializar_consultar_restaurantes(paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_finalizar_pedido(t_pcb* pcb){
	t_paquete* paquete_a_enviar = crear_paquete(FINALIZAR_PEDIDO);

	serializar_finalizar_pedido(pcb, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(pcb->socketCliente, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_finalizar_pedido_comanda(t_pcb* pcb, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(FINALIZAR_PEDIDO);

	serializar_finalizar_pedido(pcb, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);

}

void enviar_plato_listo(plato_listo* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(PLATO_LISTO);

	serializar_plato_listo(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_consultar_platos(uint32_t socketConexion){
	log_info(logger, "RESTAURANTE :: ENVIO MENSAJE: CONSULTAR_PLATOS");
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_PLATOS);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_respuesta_consultar_platos(respuesta_consultar_platos* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_PLATOS);

	serializar_respuesta_consultar_platos(msg,paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

bool respuesta_obtener_pedido(obtener_pedido* msg, uint32_t socketConexion){

	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_PEDIDO);

	serializar_obtener_pedido(msg, paquete_a_enviar->buffer);

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


	if (respuesta == OK2) {
		uint32_t tamanioNombre = 0;

		uint32_t op_code;
		recv(socketConexion, &op_code, sizeof(uint32_t), MSG_WAITALL);

		uint32_t buffer_size;
		recv(socketConexion, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

		uint32_t estadoPedido;
		recv(socketConexion, &estadoPedido, sizeof(uint32_t), MSG_WAITALL);

		uint32_t cantidad_elementos;
		recv(socketConexion, &cantidad_elementos, sizeof(uint32_t), MSG_WAITALL);

		bool platosTerminados = true;

		for (int i = 0; i < cantidad_elementos; i++) {
			int cantidadPlato;
			int cantidadLista;

			recv(socketConexion, &tamanioNombre, sizeof(uint32_t), MSG_WAITALL);
			char* nombrePlato = malloc(tamanioNombre);

			recv(socketConexion, nombrePlato, tamanioNombre, MSG_WAITALL);

			recv(socketConexion, &cantidadPlato, sizeof(uint32_t), MSG_WAITALL);

			recv(socketConexion, &cantidadLista, sizeof(uint32_t), MSG_WAITALL);

			if (cantidadLista < cantidadPlato)
				platosTerminados = false;

			free(nombrePlato);
		}
		return platosTerminados;
	} else {
			log_info(logger,"Error al obtener pedido");
			return false;
		}
}

void enviar_respuesta_consultar_pedido (respuesta_pedido* respuesta, char* nombreRestaurante, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_PEDIDO);

	serializar_respuesta_consultar_pedido(respuesta, nombreRestaurante, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}


uint32_t obtener_respuesta_crear_pedido(uint32_t socketConexion){
return 0;
}

