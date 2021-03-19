#include "utils_Sindicato.h"

void iniciar_servidor(void) {
	int socket_servidor;

	//char* ip_sindicato = "127.0.0.1";

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip_sindicato, puerto_escucha, &hints, &servinfo);

	int activado = 1;

	if ((socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		log_info(logger, "No se pudo crear el server correctamente. Levante el modulo nuevamente. Gracias, vuelvas prontos.");
		exit(-1);
	}

	// int i = setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
		log_info(logger, "No se pudo crear el server correctamente. Levante el modulo nuevamente. Gracias, vuelvas prontos.");
		exit(-1);
	}

	if(listen(socket_servidor, SOMAXCONN)){
		log_info(logger, "No se pudo crear el server correctamente. Levante el modulo nuevamente. Gracias, vuelvas prontos.");
		exit(-1);
	}

	freeaddrinfo(servinfo);

	while (1) {
		esperar_cliente(socket_servidor);
	}
}

void esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;
	pthread_t thread;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	int *socket_paraPasar = malloc(sizeof(int));
	*socket_paraPasar = socket_cliente;

	pthread_create(&thread, NULL, (void*) serve_client, socket_paraPasar);
	pthread_detach(thread);
}

void serve_client(int* socketQuenosPAsan) {
	int socket = *socketQuenosPAsan;
	free(socketQuenosPAsan);

	int cod_op;

	while (1) {
		int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);

		if (i != 4) {
			liberar_conexion(socket);
			cod_op = -1;
			liberar_conexion(socket);
			pthread_exit(NULL);
		}
		else {
			procesar_mensaje_recibido(cod_op, socket);
//			liberar_conexion(socket);
//			pthread_exit(NULL);
		}
		// TODO deberiamos liberar la conexion jeje
		/*
		if (cod_op == TERMINAR) {
			liberar_conexion(socket);
			pthread_exit(NULL);
		}
		*/
	}


}
void inicializar_semaforos(){

	mutexBlocks = malloc(sizeof(pthread_mutex_t)*blocks);
	for(int i=0;i<blocks;i++)
		pthread_mutex_init(&mutexBlocks[i], NULL);

	pthread_mutex_init(&mutexConsultarPlatos, NULL);
	pthread_mutex_init(&mutexGuardarPedido, NULL);
	pthread_mutex_init(&mutexGuardarPlato, NULL);
	pthread_mutex_init(&mutexConfirmarPedido, NULL);
	pthread_mutex_init(&mutexObtenerPedido, NULL);
	pthread_mutex_init(&mutexObtenerRestaurante, NULL);
	pthread_mutex_init(&mutexPlatoListo, NULL);
	pthread_mutex_init(&mutexObtenerReceta, NULL);
	pthread_mutex_init(&mutexTerminarPedido, NULL);
}

void procesar_mensaje_recibido(int cod_op, int cliente_fd) {

	t_respuesta2 confirmado;

	uint32_t buffer_size;
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	// logguear quien se me conecto: quiza hay que agregarle a los paquetes el nombre del modulo que envió el paquete, no lo sé

	//Procesar mensajes recibidos
	switch (cod_op) {
		case CONSULTAR_PLATOS:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:: CONSULTAR_PLATOS"); // REFACTOREAR PARA QUE EN VEZ DE DECIR "DEL RESTAURANTE" DIGA DEL MODULO, QUIZÁ ALGUN ID PARA MAYOR AUDITORIA

			t_restaurante* consultarPlatosRecibido = deserializar_consultar_platos(cliente_fd);

			pthread_mutex_lock(&mutexConsultarPlatos);

			consultar_platos_respuesta* consultarPlatosRespuesta = consultar_platos_FS(consultarPlatosRecibido);

			pthread_mutex_unlock(&mutexConsultarPlatos);

			enviar_restaurante_consultar_platos_respuesta(consultarPlatosRespuesta, cliente_fd);

			free(consultarPlatosRecibido->nombreRestaurante);
			free(consultarPlatosRecibido);
			list_destroy(consultarPlatosRespuesta->listaTamanioPlatos);
			list_destroy(consultarPlatosRespuesta->platos);
			list_destroy(consultarPlatosRespuesta->precio_platos);
			free(consultarPlatosRespuesta);
			break;
		case GUARDAR_PEDIDO:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:: GUARDAR_PEDIDO");
			guardar_pedido* guardarPedidoRecibido = deserializar_guardar_pedido(cliente_fd);

			pthread_mutex_lock(&mutexGuardarPedido);

			confirmado = crear_archivo_pedido(guardarPedidoRecibido);
			pthread_mutex_unlock(&mutexGuardarPedido);

			if (confirmado == OK2)
				log_info(logger, "Pedido %d creado", guardarPedidoRecibido->id_pedido);
			else
				log_info(logger, "Error al crear pedido");

			enviar_respuesta(confirmado, cliente_fd);

			log_info(logger, "SINDICATO :: ENVIAMOS LA RESPUESTA A GUARDAR PEDIDO");

			free(guardarPedidoRecibido->nombreRestaurante);
			free(guardarPedidoRecibido);
			break;
		case GUARDAR_PLATO:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:: GUARDAR_PLATO");
			guardar_plato* guardarPlatoRecibido = deserializar_guardar_plato(cliente_fd);

			pthread_mutex_lock(&mutexGuardarPlato);

			confirmado = agregar_plato_a_pedido(guardarPlatoRecibido);

			pthread_mutex_unlock(&mutexGuardarPlato);

			enviar_respuesta(confirmado, cliente_fd);

			free(guardarPlatoRecibido->nombrePlato);
			free(guardarPlatoRecibido->nombreRestaurante);
			free(guardarPlatoRecibido);
			break;
		case CONFIRMAR_PEDIDO:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:: CONFIRMAR_PEDIDO");
			confirmar_pedido* confirmarPedidoRecibido = deserializar_confirmar_pedido(cliente_fd);

			pthread_mutex_lock(&mutexConfirmarPedido);

			confirmado = confirmar_pedido_FS(confirmarPedidoRecibido);

			pthread_mutex_unlock(&mutexConfirmarPedido);

			enviar_respuesta(confirmado, cliente_fd);

			free(confirmarPedidoRecibido->nombreRestaurante);
			free(confirmarPedidoRecibido);
			break;
		case OBTENER_PEDIDO:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:; OBTENER_PEDIDO");
			obtener_pedido* obtenerPedidoRecibido = deserializar_obtener_pedido(cliente_fd);

			pthread_mutex_lock(&mutexObtenerPedido);

			obtener_pedido_respuesta* msg_a_enviar = obtener_pedido_FS(obtenerPedidoRecibido->nombreRestaurante, obtenerPedidoRecibido->id_pedido);

			pthread_mutex_unlock(&mutexObtenerPedido);

			enviar_restaurante_obtener_pedido_respuesta(msg_a_enviar, cliente_fd);

			log_info(logger,"SINDICATO :: SALIMOS DEL enviar_restaurante_obtener_pedido_respuesta");

			list_destroy(msg_a_enviar->listaCantidadDePedido);
			list_destroy(msg_a_enviar->listaCantidadLista);
			list_destroy(msg_a_enviar->listaTamanioPlato);
			list_destroy(msg_a_enviar->platos);
			free(msg_a_enviar);
			free(obtenerPedidoRecibido->nombreRestaurante);
			free(obtenerPedidoRecibido);
			break;
		case OBTENER_RESTAURANTE:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:: OBTENER_RESTAURANTE");

			t_restaurante* obtenerRestauranteRecibido = deserializar_obtener_restaurante(cliente_fd);

			pthread_mutex_lock(&mutexObtenerRestaurante);

			obtener_restaurante_respuesta* msg = obtener_restaurante_FS(obtenerRestauranteRecibido);

			pthread_mutex_unlock(&mutexObtenerRestaurante);

			if(msg){
				log_info(logger, "SINDICATO :: ENVIAMOS LA RESPUESTA A OBTENER RESTAURANTE");
				enviar_restaurante_obtener_restaurante_respuesta(msg, cliente_fd);
				list_destroy(msg->afinidad_cocineros);
				list_destroy(msg->listaTamanioAfinidades);
				list_destroy(msg->listaTamanioPlatos);
				list_destroy(msg->platos);
				list_destroy(msg->precio_platos);
				free(msg->nombre_restaurante);
				free(msg);
			}
			else{
				log_info(logger, "SINDICATO :: NO SE PUDO OBTENER EL RESTAURANTE");
				enviar_obtener_restaurante_FAIL(cliente_fd);
			}

			free(obtenerRestauranteRecibido->nombreRestaurante);
			free(obtenerRestauranteRecibido);

			break;
		case PLATO_LISTO:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:: PLATO_LISTO");
			plato_listo* platoListoRecibido = deserializar_plato_listo(cliente_fd);

			pthread_mutex_lock(&mutexPlatoListo);

			confirmado = plato_listo_FS(platoListoRecibido);

			pthread_mutex_unlock(&mutexPlatoListo);

			enviar_respuesta(confirmado, cliente_fd);

			free(platoListoRecibido->nombrePlato);
			free(platoListoRecibido->nombreRestaurante);
			free(platoListoRecibido);
			break;
		case OBTENER_RECETA:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:: OBTENER_RECETA");
			obtener_receta* obtenerRecetaRecibido = deserializar_obtener_receta(cliente_fd);

			pthread_mutex_lock(&mutexObtenerReceta);

			t_receta_respuesta* obtenerRecetaRespuesta = obtener_receta_FS(obtenerRecetaRecibido);
			log_info(logger, "SINDICATO :: BAJAMOS LA RECETA DEL FS");

			pthread_mutex_unlock(&mutexObtenerReceta);

			enviar_obtener_receta_respuesta(obtenerRecetaRespuesta,cliente_fd);
			log_info(logger, "SINDICATO :: ENVIAMOS LA RESPUESTA AL CLIENTE CON LA RECETA");

			list_destroy(obtenerRecetaRespuesta->listaPasos);
			list_destroy(obtenerRecetaRespuesta->listaTamanioPasos);
			list_destroy(obtenerRecetaRespuesta->listaTiempoPasos);
			free(obtenerRecetaRespuesta);
			break;
		case TERMINAR_PEDIDO:
			log_info(logger, "SINDICATO ::ME LLEGO EL MENSAJE:: TERMINAR_PEDIDO");
			terminar_pedido* terminarPedidoRecibido = deserializar_terminar_pedido(cliente_fd);

			pthread_mutex_lock(&mutexTerminarPedido);

			confirmado = terminar_pedido_FS(terminarPedidoRecibido);

			pthread_mutex_unlock(&mutexTerminarPedido);

			enviar_respuesta(confirmado, cliente_fd);

			free(terminarPedidoRecibido->nombreRestaurante);
			free(terminarPedidoRecibido);
			break;
		case -1:
			pthread_exit(NULL);
			break;
		default:
			break;
	}
}

obtener_pedido_respuesta* dar_nuevo_pedido(){
	obtener_pedido_respuesta* nuevoPedido = malloc(sizeof(obtener_pedido_respuesta));

	nuevoPedido->estado = PENDIENTE;
	nuevoPedido->listaTamanioPlato = list_create();
	nuevoPedido->platos = list_create();
	nuevoPedido->listaCantidadDePedido = list_create();
	nuevoPedido->listaCantidadLista = list_create();
	nuevoPedido->precioTotal = 0;

	return nuevoPedido;
}
