#include <sys/time.h>
#include <sys/types.h>
#include "utils_app.h"
#include "planificacion_app.h"

void iniciar_servidor(void) {
	int socket_servidor;

	//char* ip_app = "127.0.0.1";

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip_app, puerto_escucha, &hints, &servinfo);

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

void serve_client(int* socketQueNosPasan) {
	int socket = *socketQueNosPasan;
	free(socketQueNosPasan);
	int cod_op;

	while (1) {
		int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);

		if (i != 4) {
			liberar_conexion(socket);
			cod_op = -1;
		} else {
			procesar_mensaje_recibido(cod_op, socket);
		}

		/*
		if (cod_op == TERMINAR) {
			liberar_conexion(socket);
			pthread_exit(NULL);
		}
		*/
	}
}

void procesar_mensaje_recibido(int cod_op, int cliente_fd) {
//
	uint32_t buffer_size;
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	seleccionar_restaurante* seleccionarRestaurante;
	consultar_platos* consultarPlatos;
	guardar_pedido* guardar_pedido_recibido;
	aniadir_plato* aniadirPlato;
	plato_listo* plato_listo_recibido;
	consultar_pedido* consultarPedido;
	t_respuesta2 respuesta_comanda;

	//Procesar mensajes recibidos
	switch (cod_op) {
	case CONSULTAR_RESTAURANTES:
		log_info(logger, "APP:: Me llego el mensaje: CONSULTAR_RESTAURANTES");

		enviar_restaurantes_conectados(cliente_fd);

		break;
	case SELECCIONAR_RESTAURANTE:
		log_info(logger, "APP:: Me llego el mensaje: SELECCIONAR_RESTAURANTE");

		t_respuesta2 respuestaSeleccionar;

		bool esta_vinculado(){
			bool es_el_que_quiero(void* parametro){
				vinResCli* elem = (vinResCli*) parametro;
				return !strcmp(elem->idCliente, clienteActual->idCliente);
			}

			pthread_mutex_lock(&mutexClienteActual);
			if (list_find(restaurantesPorCliente, es_el_que_quiero) == NULL) {
				pthread_mutex_unlock(&mutexClienteActual);
				return false;
			} else {
				pthread_mutex_unlock(&mutexClienteActual);
				return true;
			}
		}

		seleccionarRestaurante = deserializar_seleccionar_restaurante(cliente_fd);

		bool restaurante_que_busco(void* parametro) {
			restaurante_conectado* elem = (restaurante_conectado*) parametro;
			return (!strcmp(elem->nombreRestaurante, seleccionarRestaurante->nombreRestaurante));
		}


			pthread_mutex_lock(&restConectados);
		if (list_find(restaurantes_conectados, restaurante_que_busco) != NULL || !strcmp(seleccionarRestaurante->nombreRestaurante,"Default")) {
			pthread_mutex_unlock(&restConectados);
			if (esta_vinculado()) {
				pthread_mutex_lock(&vinculos);
				pthread_mutex_lock(&mutexClienteActual);
				romper_asociacion(clienteActual);
				pthread_mutex_unlock(&mutexClienteActual);
				pthread_mutex_unlock(&vinculos);
			}

			asociar_restaurante(seleccionarRestaurante->nombreRestaurante);

			respuestaSeleccionar = OK2;
		} else {
			pthread_mutex_unlock(&restConectados);
			respuestaSeleccionar = FAIL2;
		}

		enviar_respuesta(respuestaSeleccionar, cliente_fd);

		break;
	case CONSULTAR_PLATOS:
		log_info(logger, "APP:: Me llego el mensaje: CONSULTAR_PLATOS");

		uint32_t sockRestaurante;

		consultarPlatos = deserializar_consultar_platos(cliente_fd);

		if(!strcmp(consultarPlatos->nombreRestaurante,"Default")){
			enviar_respuesta_platos(platos_default,cliente_fd);
		}
		else{
			bool restaurante_buscado(void* parametro) {
				restaurante_conectado* elem = (restaurante_conectado*) parametro;
				return (!strcmp(elem->nombreRestaurante, consultarPlatos->nombreRestaurante));
			}
			pthread_mutex_lock(&restConectados);
			restaurante_conectado* restaurante = list_find(restaurantes_conectados, restaurante_buscado);
			pthread_mutex_unlock(&restConectados);
			sockRestaurante = restaurante->socketRestaurante;
			enviar_consultar_platos(sockRestaurante);

			uint32_t opcode;
			recv(sockRestaurante, &opcode, sizeof(uint32_t), MSG_WAITALL);

			uint32_t buffersize;
			recv(sockRestaurante, &buffersize, sizeof(uint32_t), MSG_WAITALL);

			respuesta_consultar_platos* respuesta = deserializar_respuesta_consultar_platos(sockRestaurante);

			enviar_respuesta_consultar_platos(respuesta,cliente_fd);
		}

		break;
	case CREAR_PEDIDO:
		log_info(logger, "APP:: Me llego el mensaje: CREAR_PEDIDO");

		int correcto;
		uint32_t idPedido;

		if (!restaurante_esta_conectado()) {
			log_info(logger,"El cliente no ha seleccionado ningun restaurante");
			enviar_respuesta_id(FAIL2, -1, cliente_fd);
		} else {
			if (!strcmp(obtener_restaurante_del_cliente(), "Default")) {
				pthread_mutex_lock(&idPedidoG);
				idPedido = idPedidoDefault++;
				pthread_mutex_unlock(&idPedidoG);
				guardar_pedido_recibido = malloc(sizeof(guardar_pedido));
				guardar_pedido_recibido->idPedido = idPedido;
				guardar_pedido_recibido->tamanioNombre = 8;
				guardar_pedido_recibido->nombreRestaurante = malloc(8);
				strcpy(guardar_pedido_recibido->nombreRestaurante, "Default");

				socketConexionComanda = crear_conexion(ip_comanda,puerto_comanda);
				if (socketConexionComanda == -1) {
					log_info(logger, "No me pude conectar a la Comanda");
					respuesta_comanda = FAIL2;
				} else if (socketConexionComanda > 0) {
					log_info(logger, "Me conecte exitosamente a la Comanda");
					enviar_guardar_pedido(guardar_pedido_recibido,socketConexionComanda);
					respuesta_comanda = recibir_respuesta(socketConexionComanda);
					close(socketConexionComanda);
				}


				if (respuesta_comanda == OK2) {
					log_info(logger, "¡Pedido creado con Exito!");
					enviar_respuesta_id(respuesta_comanda,guardar_pedido_recibido->idPedido, cliente_fd);
				} else {
					log_info(logger,"El pedido no pudo ser creado. Intente nuevamente.");
					enviar_respuesta_id(respuesta_comanda, -1, cliente_fd);
				}
			} else {
				bool restaurante_buscado(void* parametro) {
					vinResCli* elem = (vinResCli*) parametro;
					return !strcmp(elem->idCliente, clienteActual->idCliente);
				}
				pthread_mutex_lock(&vinculos);
				pthread_mutex_lock(&mutexClienteActual);
				vinResCli* buscado = list_find(restaurantesPorCliente,restaurante_buscado);
				pthread_mutex_unlock(&mutexClienteActual);
				pthread_mutex_unlock(&vinculos);

				enviar_crear_pedido(buscado->socketRestaurante);

			}
		}


		break;
	case ANIADIR_PLATO:
		log_info(logger, "APP:: Me llego el mensaje: ANIADIR_PLATO");


		aniadirPlato = deserializar_aniadir_plato(cliente_fd);

		pthread_mutex_lock(&mutexAniadirPlato);

		mensaje = malloc(sizeof(guardar_plato));
		mensaje->nombreRestaurante = obtener_restaurante_del_cliente();
		mensaje->tamanioNombre = strlen(mensaje->nombreRestaurante) + 1;
		mensaje->tamanioNombrePlato = strlen(aniadirPlato->nombrePlato) + 1;
		mensaje->nombrePlato = aniadirPlato->nombrePlato;
		mensaje->cantidad_platos = 1;
		mensaje->id_pedido = aniadirPlato->idPedido;

		if (strcmp("Default", mensaje->nombreRestaurante) != 0) {
			if (list_size(restaurantes_conectados)) {
				bool restaurante_buscado(void* parametro) {
					restaurante_conectado* elem = (restaurante_conectado*) parametro;
					return (!strcmp(elem->nombreRestaurante, mensaje->nombreRestaurante));
				}
				pthread_mutex_lock(&restConectados);
				restaurante_conectado* restaurante = list_find(restaurantes_conectados, restaurante_buscado);
				pthread_mutex_unlock(&restConectados);
				aniadirPlato->idPedido -= (restaurante->codigoRestaurante*offsetCodRestaurante);
				enviar_aniadir_plato(aniadirPlato,restaurante->socketRestaurante);
			}
		} else {
			pthread_mutex_unlock(&mutexAniadirPlato);
			socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
			if (socketConexionComanda == -1) {
				log_info(logger, "No me pude conectar a la Comanda");
				respuesta_comanda = FAIL2;
			} else if (socketConexionComanda > 0) {
				log_info(logger, "Me conecte exitosamente a la Comanda");
				enviar_guardar_plato(mensaje,socketConexionComanda);
				respuesta_comanda = recibir_respuesta(socketConexionComanda);
				close(socketConexionComanda);
			}

			pthread_mutex_lock(&mutexClienteActual);
			if(respuesta_comanda == OK2){
				log_info(logger, "¡Plato aniadido con Exito!");
				enviar_respuesta(respuesta_comanda,clienteActual->socket);
			} else {
				log_info(logger, "El plato no pudo ser aniadido");
				enviar_respuesta(respuesta_comanda,clienteActual->socket);
			}
			pthread_mutex_unlock(&mutexClienteActual);
		}

		break;
	case PLATO_LISTO:
		log_info(logger, "APP:: Me llego el mensaje: PLATO_LISTO");

		plato_listo_recibido = deserializar_plato_listo(cliente_fd);

		bool restaurante_buscado(void* parametro) {
			restaurante_conectado* elem = (restaurante_conectado*) parametro;
			return (!strcmp(elem->nombreRestaurante, plato_listo_recibido->nombreRestaurante));
			}
		pthread_mutex_lock(&restConectados);
		restaurante_conectado* restaurante = list_find(restaurantes_conectados, restaurante_buscado);
		pthread_mutex_unlock(&restConectados);

		plato_listo_recibido->id_pedido += (restaurante->codigoRestaurante * offsetCodRestaurante);

		obtener_pedido* envio_obtener_pedido = malloc(sizeof(obtener_pedido));
		envio_obtener_pedido->tamanioNombre = plato_listo_recibido->tamanioNombre;
		envio_obtener_pedido->nombreRestaurante = plato_listo_recibido->nombreRestaurante;
		envio_obtener_pedido->id_pedido = plato_listo_recibido->id_pedido;

		socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
		if (socketConexionComanda == -1) {
			log_info(logger, "No me pude conectar a la Comanda");
			respuesta_comanda = FAIL2;
		} else if (socketConexionComanda > 0) {
			log_info(logger, "Me conecte exitosamente a la Comanda");

			enviar_plato_listo(plato_listo_recibido, socketConexionComanda);

			socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
			if (socketConexionComanda == -1) {
				log_info(logger, "No me pude conectar a la Comanda");
				respuesta_comanda = FAIL2;
			} else if (socketConexionComanda > 0) {
				log_info(logger, "Me conecte exitosamente a la Comanda");

				bool resp = respuesta_obtener_pedido(envio_obtener_pedido,socketConexionComanda);
				if (resp) {
					bool pcb_buscado(void* parametro) {
						t_pcb* elem = (t_pcb*) parametro;
						return elem->id_pedido == envio_obtener_pedido->id_pedido;
					}
					t_pcb* pcb = list_find(lista_pcbs, pcb_buscado);

					bool es_el_pcb_que_quiero(void* parametro) {
						t_pcb* pcbDeLista = (t_pcb *) parametro;
						return pcb->id_pcb == pcbDeLista->id_pcb;
					}

					log_info(logger,"Los platos del pedido %d ya estan listos",pcb->id_pedido);

					if(pcb->estaEsperando){
						if (pcb->estado = BLOCKED) {
							pthread_mutex_lock(&mutex_block);
							list_remove_by_condition(lista_bloqueados,es_el_pcb_que_quiero);
							pthread_mutex_unlock(&mutex_block);

							pcb->estado = READY;
							pthread_mutex_lock(&mutex_cola_ready);
							list_add(lista_ready, pcb);
							pthread_mutex_unlock(&mutex_cola_ready);
							sem_post(&semaforo_ready_pcb);
							pcb->estaEsperando = false;
						}
					}
					close(socketConexionComanda);
				}
			}
		}



		break;
	case CONFIRMAR_PEDIDO:
		log_info(logger, "APP:: Me llego el mensaje: CONFIRMAR_PEDIDO"); // deberiamos loguear el id...

		pthread_mutex_lock(&mutexConfirmarPedido);

		confirmar_pedido_recibido = deserializar_confirmar_pedido(cliente_fd);
		uint32_t idPed = confirmar_pedido_recibido->id_pedido;

		if (!strcmp(confirmar_pedido_recibido->nombreRestaurante, "Default")) {

			pthread_mutex_unlock(&mutexConfirmarPedido);

			socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
			if (socketConexionComanda == -1) {
				log_info(logger, "No me pude conectar a la Comanda");
				respuesta_comanda = FAIL2;
			} else if (socketConexionComanda > 0) {
				log_info(logger, "Me conecte exitosamente a la Comanda");
				enviar_confirmar_pedido(confirmar_pedido_recibido,socketConexionComanda);
				respuesta_comanda = recibir_respuesta(socketConexionComanda);
				close(socketConexionComanda);
			}


			if (respuesta_comanda == OK2) {
				pthread_mutex_lock(&mutex_idPcb);
				crear_pcb(confirmar_pedido_recibido->id_pedido,confirmar_pedido_recibido->nombreRestaurante,cliente_fd);
				pthread_mutex_unlock(&mutex_idPcb);
			} else {
				log_info(logger, "No existe el Pedido");
			}

			enviar_respuesta(respuesta_comanda, cliente_fd);
		} else {
			if(list_size(restaurantes_conectados)){
				bool restaurante_buscado(void* parametro) {
					restaurante_conectado* elem = (restaurante_conectado*) parametro;
					return (!strcmp(elem->nombreRestaurante, confirmar_pedido_recibido->nombreRestaurante));
					}
				pthread_mutex_lock(&restConectados);
				restaurante_conectado* aux = list_find(restaurantes_conectados,restaurante_buscado);
				pthread_mutex_unlock(&restConectados);
				if(aux != NULL){
					confirmar_pedido_recibido->id_pedido -= (aux->codigoRestaurante * offsetCodRestaurante);
					enviar_confirmar_pedido(confirmar_pedido_recibido, aux->socketRestaurante);
					confirmar_pedido_recibido->id_pedido += (aux->codigoRestaurante * offsetCodRestaurante);
				} else {
					log_info(logger,"El restaurante no esta conectado");

					pthread_mutex_unlock(&mutexConfirmarPedido);

					enviar_respuesta(FAIL2, cliente_fd);
				}

			}
		}
		break;
	case CONSULTAR_PEDIDO:
		log_info(logger, "APP:: Me llego el mensaje: CONSULTAR_PEDIDO");

		consultarPedido = deserializar_consultar_pedido(cliente_fd);

		obtener_pedido* msg2 = malloc(sizeof(obtener_pedido));
		msg2->id_pedido = consultarPedido->idPedido;
		msg2->tamanioNombre = strlen(obtener_restaurante_del_cliente()) + 1;
		msg2->nombreRestaurante = malloc(msg2->tamanioNombre);
		msg2->nombreRestaurante = obtener_restaurante_del_cliente();

		respuesta_pedido* respuesta;

		socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
		if (socketConexionComanda == -1) {
			log_info(logger, "No me pude conectar a la Comanda");
		} else if (socketConexionComanda > 0) {
			log_info(logger, "Me conecte exitosamente a la Comanda");
			respuesta = enviar_obtener_pedido(msg2, socketConexionComanda);
			close(socketConexionComanda);
		}


		enviar_respuesta_consultar_pedido(respuesta, msg2->nombreRestaurante, cliente_fd);

		free(msg2->tamanioNombre);
		free(msg2);

		break;

	case NOMBRE_RESTAURANTE:
		log_info(logger,"Se conecto un restaurante");

		pthread_mutex_lock(&conexionRestaurantes);
		restaurante_conectado* resto = obtener_restaurante_conectado(cliente_fd);
		resto->codigoRestaurante = idRestaurantes;
		resto->socketRestaurante = cliente_fd;
		idRestaurantes ++;
		pthread_mutex_unlock(&conexionRestaurantes);
		log_info(logger,"Nombre: %s\n Posicion:(%d,%d)", resto->nombreRestaurante, resto->posicion.posX, resto->posicion.posY);

		pthread_mutex_lock(&restConectados);
		list_add(restaurantes_conectados, resto);
		pthread_mutex_unlock(&restConectados);

		break;
	case DATOS_CLIENTE:
		;

		pthread_mutex_lock(&mutexClienteActual);
		clienteActual = deserializar_datos_cliente(cliente_fd);
		clienteActual->socket = cliente_fd;
		pthread_mutex_unlock(&mutexClienteActual);

		break;
	case RESPUESTA_CREAR_PEDIDO:
		log_info(logger, "APP:: Me llego el mensaje: RESPUESTA_CREAR_PEDIDO");

		t_respuesta2 rta;
		recv(cliente_fd, &rta, sizeof(t_respuesta2), MSG_WAITALL);

		uint32_t id_pedido;
		recv(cliente_fd, &id_pedido, sizeof(uint32_t), MSG_WAITALL);

		bool restaurante_que_quiero(void* parametro) {
			restaurante_conectado* elem =(restaurante_conectado*) parametro;
			return elem->socketRestaurante == cliente_fd;
		}


		if (id_pedido != -1) {
			pthread_mutex_lock(&restConectados);
			restaurante_conectado* rest = list_find(restaurantes_conectados, restaurante_que_quiero);
			pthread_mutex_unlock(&restConectados);
			uint32_t idPed = (rest->codigoRestaurante* offsetCodRestaurante) + id_pedido;
			guardar_pedido_recibido = malloc(sizeof(guardar_pedido));
			guardar_pedido_recibido->idPedido = idPed;
			guardar_pedido_recibido->tamanioNombre =rest->tamanioNombre;
			guardar_pedido_recibido->nombreRestaurante =rest->nombreRestaurante;

			socketConexionComanda = crear_conexion(ip_comanda,puerto_comanda);
			if (socketConexionComanda == -1) {
				log_info(logger, "No me pude conectar a la Comanda");
				respuesta_comanda = FAIL2;
			} else if (socketConexionComanda > 0) {
				log_info(logger, "Me conecte exitosamente a la Comanda");
				enviar_guardar_pedido(guardar_pedido_recibido,socketConexionComanda);
				respuesta_comanda = recibir_respuesta(socketConexionComanda);
				close(socketConexionComanda);
			}

			pthread_mutex_lock(&mutexClienteActual);
			if (respuesta_comanda == OK2) {
				log_info(logger, "¡Pedido creado con Exito!");
				enviar_respuesta_id(respuesta_comanda,idPed, clienteActual->socket);
			} else {
				log_info(logger,"El pedido no pudo ser creado. Intente nuevamente.");
				enviar_respuesta_id(respuesta_comanda, -1, clienteActual->socket);
			}
			pthread_mutex_unlock(&mutexClienteActual);
		} else {
			log_info(logger,"El pedido no pudo ser creado");
			pthread_mutex_lock(&mutexClienteActual);
			enviar_respuesta_id(FAIL2, -1, clienteActual->socket);
			pthread_mutex_unlock(&mutexClienteActual);
		}

		break;
	case RESPUESTA_ANIADIR_PLATO:
		log_info(logger, "APP:: Me llego el mensaje: RESPUESTA_ANIADIR_PLATO");

		t_respuesta2 rta2;
		recv(cliente_fd, &rta2, sizeof(t_respuesta2), MSG_WAITALL);

		if (rta2 == OK2) {
			socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
			if (socketConexionComanda == -1) {
				log_info(logger, "No me pude conectar a la Comanda");
				respuesta_comanda = FAIL2;
			} else if (socketConexionComanda > 0) {
				log_info(logger, "Me conecte exitosamente a la Comanda");
				enviar_guardar_plato(mensaje, socketConexionComanda);
				respuesta_comanda = recibir_respuesta(socketConexionComanda);
				close(socketConexionComanda);
			}

			pthread_mutex_lock(&mutexClienteActual);
			if (respuesta_comanda == OK2) {
				log_info(logger, "¡Plato aniadido con Exito!");
				enviar_respuesta(respuesta_comanda, clienteActual->socket);
			} else {
				log_info(logger, "El plato no pudo ser aniadido");
				enviar_respuesta(respuesta_comanda, clienteActual->socket);
			}
			pthread_mutex_unlock(&mutexClienteActual);
		} else {
			log_info(logger, "Error del restaurante");
			pthread_mutex_lock(&mutexClienteActual);
			enviar_respuesta(FAIL2, clienteActual->socket);
			pthread_mutex_unlock(&mutexClienteActual);
		}

		free(mensaje);
		pthread_mutex_unlock(&mutexAniadirPlato);

		break;
	case RESPUESTA_CONFIRMAR_PEDIDO:
		log_info(logger, "APP:: Me llego el mensaje: RESPUESTA_CONFIRMAR_PEDIDO");

		t_respuesta2 rta3;
		recv(cliente_fd, &rta3, sizeof(t_respuesta2), MSG_WAITALL);

		if (rta3 == OK2) {
			socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
			if (socketConexionComanda == -1) {
				log_info(logger, "No me pude conectar a la Comanda");
				respuesta_comanda = FAIL2;
			} else if (socketConexionComanda > 0) {
				log_info(logger, "Me conecte exitosamente a la Comanda");
				enviar_confirmar_pedido(confirmar_pedido_recibido,socketConexionComanda);
				respuesta_comanda = recibir_respuesta(socketConexionComanda);
				close(socketConexionComanda);
			}


			if (respuesta_comanda == OK2) {
				pthread_mutex_lock(&mutexClienteActual);
				pthread_mutex_lock(&mutex_idPcb);
				crear_pcb(confirmar_pedido_recibido->id_pedido,confirmar_pedido_recibido->nombreRestaurante,clienteActual->socket);
				pthread_mutex_unlock(&mutex_idPcb);
				pthread_mutex_unlock(&mutexClienteActual);
			} else {
				log_info(logger, "No existe el Pedido");
			}
		} else {
			log_info(logger, "Error del restaurante");
			respuesta_comanda = FAIL2;
		}
		pthread_mutex_lock(&mutexClienteActual);
		enviar_respuesta(respuesta_comanda, clienteActual->socket);
		pthread_mutex_unlock(&mutexClienteActual);

		pthread_mutex_unlock(&mutexConfirmarPedido);

		break;
	case -1: // DEBERIA IR OBTENER RECETA
		log_info(logger, "APP:: ERROR OP_CODE RECIBIDO");
		pthread_exit(NULL);
		break;
	default:
		break;
	}
}

t_respuesta2 recibir_respuesta(int socket) {

	t_respuesta2 respuesta;
	uint32_t buffer_size;
	uint32_t cod_operacion = 0;

	recv(socket, &cod_operacion, sizeof(op_code), MSG_WAITALL);
	recv(socket, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	recv(socket, &respuesta, sizeof(t_respuesta2), MSG_WAITALL);

	return respuesta;
}


posicion obtener_posicion_restaurante(char* nombre_restaurante){
	if(!strcmp(nombre_restaurante, "Default")){
		log_info(logger, "Posicion del Restaurante Default");
		return posicionDefault;
	} else {
		bool restaurante_buscado(void* parametro) {
			restaurante_conectado* elem = (restaurante_conectado*) parametro;
			return !strcmp(elem->nombreRestaurante,nombre_restaurante);
		}
		log_info(logger, "Posicion del Restaurante Elegido");
		pthread_mutex_lock(&restConectados);
		restaurante_conectado* aux = list_find(restaurantes_conectados,restaurante_buscado);
		pthread_mutex_unlock(&restConectados);
		return aux->posicion;
	}
}

bool repartidor_esta_libre(t_repartidor* repartidor){
	return (repartidor->ocupado == false);
}

t_list* repartidores_libres() {
	return list_filter(lista_repartidores, repartidor_esta_libre);
}

bool restaurante_esta_conectado(){

	pthread_mutex_lock(&vinculos);

	vinResCli* vinculo = NULL;//list_find(restaurantesPorCliente, es_el_que_quiero);
	bool encontrado = false;

	for (int i = 0; i < list_size(restaurantesPorCliente) && !encontrado; i++) {
		vinculo = list_get(restaurantesPorCliente, i);
		pthread_mutex_lock(&mutexClienteActual);
		if (!strcmp(vinculo->idCliente,clienteActual->idCliente)) {
			encontrado = true;
		}
		pthread_mutex_unlock(&mutexClienteActual);
	}

	if(encontrado){
		pthread_mutex_unlock(&vinculos);
		return true;
	} else {
		pthread_mutex_unlock(&vinculos);
		return false;
	}
}

char* obtener_restaurante_del_cliente(){
	bool es_el_que_quiero(void* parametro){
		vinResCli* elem = (vinResCli*) parametro;
		return !strcmp(elem->idCliente,clienteActual->idCliente);
	}

	if (list_size(restaurantes_conectados)) {
		pthread_mutex_lock(&vinculos);
		pthread_mutex_lock(&mutexClienteActual);
		vinResCli* aux = list_find(restaurantesPorCliente, es_el_que_quiero);
		pthread_mutex_unlock(&mutexClienteActual);
		pthread_mutex_unlock(&vinculos);

		bool restaurante_buscado(void* parametro) {
			restaurante_conectado* elem = (restaurante_conectado*) parametro;
			return elem->socketRestaurante == aux->socketRestaurante;
		}

		pthread_mutex_lock(&restConectados);
		restaurante_conectado* restaurante = list_find(restaurantes_conectados, restaurante_buscado);
		pthread_mutex_unlock(&restConectados);

		return restaurante->nombreRestaurante;
	} else {
		return "Default";
	}
}

restaurante_conectado* obtener_restaurante_conectado(int socketRestaurante){

	restaurante_conectado* restaurante = malloc(sizeof(restaurante_conectado));

	recv(socketRestaurante, &(restaurante->tamanioNombre), sizeof(uint32_t), MSG_WAITALL);
	restaurante->nombreRestaurante = malloc(restaurante->tamanioNombre);

	recv(socketRestaurante, restaurante->nombreRestaurante, restaurante->tamanioNombre, MSG_WAITALL);

	recv(socketRestaurante, &(restaurante->posicion.posX),sizeof(uint32_t), MSG_WAITALL);

	recv(socketRestaurante, &(restaurante->posicion.posY),sizeof(uint32_t), MSG_WAITALL);

	return restaurante;
}

void asociar_restaurante(char* nombreRestaurante){
	bool restaurante_buscado(void* parametro){
		restaurante_conectado* elem = (restaurante_conectado*) parametro;
		return (!strcmp(elem->nombreRestaurante,nombreRestaurante));
	}

	pthread_mutex_lock(&vinculos);
	pthread_mutex_lock(&mutexClienteActual);

	if(strcmp(nombreRestaurante,"Default")) {
		pthread_mutex_lock(&restConectados);
		restaurante_conectado* rest= list_find(restaurantes_conectados,restaurante_buscado);
		pthread_mutex_unlock(&restConectados);


		if(rest != NULL){
			vinResCli* nodo = malloc(sizeof(vinResCli));
			nodo->idCliente = clienteActual->idCliente;
			nodo->socketRestaurante = rest->socketRestaurante;
			list_add(restaurantesPorCliente, nodo);
		} else{
			log_info(logger, "El restaurante %s no esta conectado", nombreRestaurante);
		}

	} else {
		vinResCli* nodo = malloc(sizeof(vinResCli));
		nodo->idCliente = clienteActual->idCliente;
		nodo->socketRestaurante = -1;
		list_add(restaurantesPorCliente, nodo);
		//free(nodo);
	}
	log_info(logger, "Se asocio al %s con el restaurante %s", clienteActual->idCliente, nombreRestaurante);

	pthread_mutex_unlock(&mutexClienteActual);
	pthread_mutex_unlock(&vinculos);
	return;
}

void romper_asociacion(datos_cliente* cliente){
	bool es_el_que_quiero(void* parametro){
		vinResCli* elem = (vinResCli*) parametro;
		return !strcmp(elem->idCliente,cliente->idCliente);
	}

	if(list_size(restaurantesPorCliente)){
		if(list_find(restaurantesPorCliente, es_el_que_quiero)){
			list_remove_by_condition(restaurantesPorCliente, es_el_que_quiero);
		}
	}
}


char enteroACaracter(int numero){
    return numero + '0';
}
