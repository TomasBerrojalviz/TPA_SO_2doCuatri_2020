#include "utils_Restaurante.h"
#include "utils_en_comun.h"

void iniciar_servidor(void) {
	int socket_servidor;

	//char* ip_restaurante = "127.0.0.1";

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip_restaurante, puerto_escucha, &hints, &servinfo);

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
//	free(socketQuenosPAsan);

	int cod_op;

	while (1) {
		int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);

		if (i != 4) {
			liberar_conexion(socket);
			cod_op = -1;
		} else {
			procesar_mensaje_recibido(cod_op, socket);
//			liberar_conexion(socket);
//			pthread_exit(NULL);
		}
	}

}

void procesar_mensaje_recibido(int cod_op, int cliente_fd) {

	uint32_t socketSindicato;
	t_respuesta2 respuesta;

	uint32_t buffer_size;
	uint32_t buffer_size2;
	uint32_t buffer_size3;
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	//Procesar mensajes recibidos

//	Consultar Platos
//	Crear Pedido
//	Añadir Plato
//	Confirmar Pedido
//	Consultar Pedido

	switch (cod_op) {
		case CONSULTAR_PLATOS:
			log_info(logger, "RESTAURANTE :: ME LLEGO EL MENSAJE: CONSULTAR_PLATOS");

			t_restaurante* msg = malloc(sizeof(t_restaurante)); // todo memleak
			socketSindicato = conectar_sindicato();

			msg->nombreRestaurante = nombreRestaurante;
			msg->tamanioNombre = strlen(msg->nombreRestaurante) +1;

			enviar_sindicato_consultar_platos(msg, socketSindicato); // Es necesario?

			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);
			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

			consultar_platos_respuesta* consultarPlatoRespuesta = deserializar_consultar_platos_respuesta(socketSindicato);

			log_info(logger, "RESTAURANTE :: ME LLEGARON LOS DATOS DE LOS PLATOS");

			close(socketSindicato);

			enviar_app_consultar_platos_respuesta(consultarPlatoRespuesta, cliente_fd);

			log_info(logger, "RESTAURANTE :: SE ENVIARON LOS DATOS DE LOS PLATOS");

			break;
		case CREAR_PEDIDO:
			log_info(logger, "RESTAURANTE :: ME LLEGO EL MENSAJE: CREAR_PEDIDO");

			socketSindicato = conectar_sindicato();

			guardar_pedido* guardarPedido = malloc(sizeof(guardar_pedido));

			guardarPedido->id_pedido = ++pedido;
			guardarPedido->tamanioNombre  = strlen(nombreRestaurante) + 1;
			guardarPedido->nombreRestaurante = string_duplicate(nombreRestaurante);

			log_info(logger, "RESTAURANTE :: SE CREARA EL PEDIDO %d",guardarPedido->id_pedido);

			enviar_sindicato_guardar_pedido(guardarPedido, socketSindicato);

			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

			recv(socketSindicato, &respuesta, sizeof(t_respuesta2), 0);

			enviar_respuesta_pedido(respuesta, guardarPedido->id_pedido,cliente_fd);

			if (respuesta == FAIL2) {
				log_info(logger, "RESTAURANTE :: NO SE PUEDO GUARDAR EL PEDIDO");
				close(socketSindicato);
				free(guardarPedido->nombreRestaurante);
				free(guardarPedido);
				return;
			}

			log_info(logger, "RESTAURANTE :: EL PEDIDO FUE GUARDADO");

			close(socketSindicato);

			free(guardarPedido->nombreRestaurante);
			free(guardarPedido);

			break;
		case CONFIRMAR_PEDIDO:
			log_info(logger, "RESTAURANTE :: ME LLEGO EL MENSAJE: CONFIRMAR_PEDIDO");

			confirmar_pedido* confirmarPedidoObtenido = deserializar_confirmar_pedido(cliente_fd);

			socketSindicato = conectar_sindicato();

			enviar_sindicato_confirmar_pedido(confirmarPedidoObtenido, socketSindicato);

			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);
			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "RESTAURANTE :: ME LLEGO LA RESPUESTA SOBRE LA CONFIRMACION DEL PEDIDO DE PARTE DEL SINDICATO");

			recv(socketSindicato, &respuesta, sizeof(t_respuesta2), 0);

			enviar_respuesta(respuesta, cliente_fd, RESPUESTA_CONFIRMAR_PEDIDO);

			if (respuesta == FAIL2) {
				log_info(logger, "RESTAURANTE :: NO SE PUEDO CONFIRMAR EL PEDIDO");
				free(confirmarPedidoObtenido);
				close(socketSindicato);
				return;
			}

			log_info(logger, "RESTAURANTE :: EL PEDIDO FUE CONFIRMADO");

			enviar_sindicato_obtener_pedido(confirmarPedidoObtenido, socketSindicato);

			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);
			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "RESTAURANTE :: ME LLEGARON LOS DATOS DEL PEDIDO");

			obtener_pedido_respuesta* obtenerPedidoRespuesta = deserializar_obtener_pedido_respuesta(socketSindicato);

			close(socketSindicato);

			crear_pcbs_pedido(obtenerPedidoRespuesta, confirmarPedidoObtenido->id_pedido, cliente_fd); // Va a crear un pcb por cada plato del pedido

			free(confirmarPedidoObtenido->nombreRestaurante);
			free(confirmarPedidoObtenido);
			list_destroy(obtenerPedidoRespuesta->listaCantidadDePedido);
			list_destroy(obtenerPedidoRespuesta->listaCantidadLista);
			list_destroy(obtenerPedidoRespuesta->listaPlatos);
			list_destroy(obtenerPedidoRespuesta->listaTamanioPlato);
			free(obtenerPedidoRespuesta);

			break;
		case ANIADIR_PLATO:
			log_info(logger, "RESTAURANTE :: ME LLEGO EL MENSAJE: ANIADIR_PLATO");
			aniadir_plato* aniadir_plato = deserializar_aniadir_plato(cliente_fd);

			guardar_plato* guardarPlato = aniadir_plato_a_guardar_plato(aniadir_plato);

			socketSindicato = conectar_sindicato();
			enviar_sindicato_guardar_plato(guardarPlato, socketSindicato);


			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);
			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "RESTAURANTE :: ME LLEGO LA RESPUESTA SOBRE GUARDAR EL PLATO DE PARTE DEL SINDICATO");

			recv(socketSindicato, &respuesta, sizeof(uint32_t), 0);

			enviar_respuesta(respuesta, cliente_fd, RESPUESTA_ANIADIR_PLATO);

			if (respuesta == FAIL2) {
				log_info(logger, "RESTAURANTE :: NO SE PUEDO GUARDAR EL PLATO");
				free(msg);
				close(socketSindicato);
				return;
			}

			log_info(logger, "RESTAURANTE :: EL PLATO FUE GUARDADO");

			free(msg);
			close(socketSindicato);

			free(guardarPlato->nombrePlato);
			free(guardarPlato->nombreRestaurante);
			free(guardarPlato);
			free(aniadir_plato->plato);
			free(aniadir_plato);

			break;
		case CONSULTAR_PEDIDO:
			log_info(logger, "RESTAURANTE :: ME LLEGO EL MENSAJE: CONSULTAR_PEDIDO");
			socketSindicato = conectar_sindicato();

			consultar_pedido* consultarPedido = deserializar_consultar_pedido(cliente_fd);

			obtener_pedido* obtenerPedido = malloc(sizeof(obtener_pedido));
			obtenerPedido->id_pedido = consultarPedido->idPedido;
			obtenerPedido->tamanioNombre = strlen(nombreRestaurante) + 1;
			obtenerPedido->nombreRestaurante = string_duplicate(nombreRestaurante);

			enviar_sindicato_obtener_pedido(obtenerPedido, socketSindicato);

			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);
			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "RESTAURANTE :: ME LLEGARON LOS DATOS DEL PEDIDO");

			consultar_pedido_respuesta* consultarPedidoRespuesta =  deserializar_consultar_pedido_respuesta(socketSindicato);

			log_info(logger, "RESTAURANTE :: OBTUVE EL PEDIDO %d CON EL ESTADO EN %s Y UN PRECIO DE %d", consultarPedido->idPedido, dar_estado_pedido(consultarPedidoRespuesta->estado), consultarPedidoRespuesta->precioTotal);

			enviar_obtener_pedido_respuesta(consultarPedidoRespuesta, cliente_fd);

			log_info(logger, "RESTAURANTE :: ENVIO MENSAJE: OBTENER_PEDIDO_RESPUESTA");

			close(socketSindicato);

			break;
		default:
			break;
	}
}

uint32_t conectar_sindicato() {
    // Se conecta con el Sindicato
    uint32_t socketConSindicato = crear_conexion(ip_sindicato, puerto_sindicato);

    if (socketConSindicato == -1)
        log_info(logger, "No me pude conectar al Sindicato");
    else
        log_info(logger, "Me conecte exitosamente al Sindicato");

    return socketConSindicato;
}

uint32_t conectar_app() {
	// Se conecta con la App
	socketConApp = crear_conexion(ip_app, puerto_app); // TODO Seguir usando la var global?

	if (socketConApp == -1)
		log_info(logger, "No me pude conectar a la App");
	else{
		log_info(logger, "Me conecte exitosamente a la App");
		enviar_app_nombre_restaurante(socketConApp);
		serve_client(&socketConApp);
	}

	return 0;
}

void asignar_datos_restaurante(obtener_restaurante_respuesta* obtenerRestauranteRespuesta){

    cantidadDeCocineros = obtenerRestauranteRespuesta->cantidad_cocineros;
    posicionRestaurante.posX = obtenerRestauranteRespuesta->posicion.posX;
    posicionRestaurante.posY = obtenerRestauranteRespuesta->posicion.posY;
    afinidades = list_create(); // todo memleak
    afinidades = list_duplicate(obtenerRestauranteRespuesta->afinidad_cocineros);
    platos = list_create();// todo memleak
    platos = list_duplicate(obtenerRestauranteRespuesta->platos);
    preciosPlatos = list_create();// todo memleak
    preciosPlatos = list_duplicate(obtenerRestauranteRespuesta->precio_platos);
    cantidadDeHornos = obtenerRestauranteRespuesta->cantidad_hornos;
    pedido = obtenerRestauranteRespuesta->cantidadPedidos;

    log_info(logger, "RESTAURANTE :: Cocineros: %d - Hornos: %d - Pedidos: %d", cantidadDeCocineros, cantidadDeHornos, pedido);
}

guardar_plato* aniadir_plato_a_guardar_plato(aniadir_plato* aniadirPlato){

	guardar_plato* guardarPlato = malloc(sizeof(guardar_plato));// todo memleak

	guardarPlato->cantidad_platos = 1;
	guardarPlato->id_pedido = aniadirPlato->id_pedido;
	guardarPlato->tamanioNombre = strlen(nombreRestaurante) + 1;
	guardarPlato->nombreRestaurante = string_duplicate(nombreRestaurante);
	guardarPlato->tamanioNombrePlato = aniadirPlato->tamanioPlato;
	guardarPlato->nombrePlato = string_duplicate(aniadirPlato->plato);

	return guardarPlato;
}

void destruir_elementos(void* elemento) {
	free(elemento);
}
