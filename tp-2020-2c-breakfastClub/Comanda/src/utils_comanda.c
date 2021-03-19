#include <sys/time.h>
#include <sys/types.h>
#include "utils_comanda.h"
#include "memoria.h"

void iniciar_servidor(void) {
	int socket_servidor;

	//ip_comanda = "127.0.0.1";

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip_comanda, puerto_escucha, &hints, &servinfo);

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

	//while (1) {
		int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);

		if (i != 4) {
			liberar_conexion(socket);
			cod_op = -1;
		} else {
			procesar_mensaje_recibido(cod_op, socket);
		}
		// TODO deberiamos liberar la conexion jeje
		/*
		if (cod_op == TERMINAR) {
			liberar_conexion(socket);
			pthread_exit(NULL);
		}
		*/
	//}
}

void procesar_mensaje_recibido(int cod_op, int cliente_fd) {

	t_list* tabla_segmentos_restaurante;
	t_segmento* segmento_pedido;
	bool respuesta;
	t_list* infoPedido;

	obtener_pedido* obtener_pedido_nuevo;
	guardar_plato* guardar_plato_nuevo;
	guardar_pedido* guardar_pedido_nuevo;
	confirmar_pedido* confirmar_pedido_nuevo;
	plato_listo* plato_listo_nuevo;
	finalizar_pedido* finalizar_pedido_nuevo;

	uint32_t buffer_size;
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	switch (cod_op) {
	case GUARDAR_PEDIDO:
		log_info(logger, "COMANDA:: Me llego el mensaje: GUARDAR_PEDIDO");

		guardar_pedido_nuevo = deserializar_guardar_pedido(cliente_fd);

		log_info(logger, "Pedido: %d", guardar_pedido_nuevo->idPedido);

		tabla_segmentos_restaurante = dictionary_get(dic_restaurante_tabla_segmentos, guardar_pedido_nuevo->nombreRestaurante);

//		if (tabla_segmentos_restaurante == NULL) {
//			bool este_libre(void* parametro) {
//				return ((entradaTablaMarcos*) parametro)->libre;
//			}
//			bool esta_libre_swap(void* parametro) {
//				return ((entradaSwap*) parametro)->libre;
//			}
//			t_list* marcos_libres = list_filter(tablaDeMarcos, este_libre);
//			t_list* marcos_libres_swap = list_filter(entradas_swap, esta_libre_swap);
//
//			if(list_size(marcos_libres)>0 || list_size(marcos_libres_swap)>0){
//					log_info(logger, "No existe la tabla de segmentos para el Restaurante: %s", guardar_pedido_nuevo->nombreRestaurante);
//					// Se crea la tabla
//					tabla_segmentos_restaurante = list_create();
//					dictionary_put(dic_restaurante_tabla_segmentos,guardar_pedido_nuevo->nombreRestaurante,tabla_segmentos_restaurante);
//					log_info(logger,"Se creó correctamente la tabla de segmentos para el Restaurante: %s",guardar_pedido_nuevo->nombreRestaurante);
//
//					// Se crea el segmento
//					t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
//					segmento_nuevo->tabla_paginas = list_create();
//					segmento_nuevo->idPedido = guardar_pedido_nuevo->idPedido;
//					segmento_nuevo->estadoPedido = PENDIENTE;
//
//					pthread_mutex_lock(&mutexTablaSegmentos);
//					list_add(tabla_segmentos_restaurante, segmento_nuevo);
//					pthread_mutex_unlock(&mutexTablaSegmentos);
//
//					log_info(logger, "Se creo el segmento para el pedido: %d",guardar_pedido_nuevo->idPedido);
//
//					t_pedido* pedidoCreado = malloc(sizeof(t_pedido));
//					pedidoCreado->idPedido = guardar_pedido_nuevo->idPedido;
//					pedidoCreado->estadoPedido = PENDIENTE;
//					pedidoCreado->tamanioNombre = guardar_pedido_nuevo->tamanioNombre;
//					pedidoCreado->nombreRestaurante = guardar_pedido_nuevo->nombreRestaurante;
//
//
//					pthread_mutex_lock(&mutexPedidosCreados);
//					list_add(pedidosCreados, pedidoCreado);
//					pthread_mutex_unlock(&mutexPedidosCreados);
//
//					respuesta = true;
//				} else {
//					log_info(logger,"No hay espacio en la memoria, no se pudo crear el pedido");
//					respuesta = false;
//				}
//			}else {
//				log_info(logger, "Ya se encuentra creada");
//				respuesta = false;
//			}

		if (tabla_segmentos_restaurante == NULL) {
			log_info(logger, "No existe la tabla de segmentos para el Restaurante: %s", guardar_pedido_nuevo->nombreRestaurante);
			// Se crea la tabla
			tabla_segmentos_restaurante = list_create();
			dictionary_put(dic_restaurante_tabla_segmentos,	guardar_pedido_nuevo->nombreRestaurante, tabla_segmentos_restaurante);
			log_info(logger, "Se creó correctamente la tabla de segmentos para el Restaurante: %s", guardar_pedido_nuevo->nombreRestaurante);
		}

		bool este_libre(void* parametro) {
			return ((entradaTablaMarcos*) parametro)->libre;
		}
		bool esta_libre_swap(void* parametro) {
			return ((entradaSwap*) parametro)->libre;
		}
		t_list* marcos_libres = list_filter(tablaDeMarcos, este_libre);
		t_list* marcos_libres_swap = list_filter(entradas_swap, esta_libre_swap);

		if (list_size(marcos_libres) > 0 || list_size(marcos_libres_swap) > 0) {

			// Se crea el segmento
			t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
			segmento_nuevo->tabla_paginas = list_create();
			segmento_nuevo->idPedido = guardar_pedido_nuevo->idPedido;
			segmento_nuevo->estadoPedido = PENDIENTE;

			pthread_mutex_lock(&mutexTablaSegmentos);
			list_add(tabla_segmentos_restaurante, segmento_nuevo);
			pthread_mutex_unlock(&mutexTablaSegmentos);

			log_info(logger, "Se creo el segmento para el pedido: %d", guardar_pedido_nuevo->idPedido);

			t_pedido* pedidoCreado = malloc(sizeof(t_pedido));
			pedidoCreado->idPedido = guardar_pedido_nuevo->idPedido;
			pedidoCreado->estadoPedido = PENDIENTE;
			pedidoCreado->tamanioNombre = guardar_pedido_nuevo->tamanioNombre;
			pedidoCreado->nombreRestaurante =guardar_pedido_nuevo->nombreRestaurante;

			pthread_mutex_lock(&mutexPedidosCreados);
			list_add(pedidosCreados, pedidoCreado);
			pthread_mutex_unlock(&mutexPedidosCreados);

			respuesta = true;
		} else {
			log_info(logger, "No hay espacio en la memoria, no se pudo crear el pedido");
			respuesta = false;
		}

		enviar_respuesta(respuesta, cliente_fd);

		free(guardar_pedido_nuevo);
		list_destroy(marcos_libres);
		list_destroy(marcos_libres_swap);
		break;
	case GUARDAR_PLATO:
		log_info(logger, "COMANDA:: Me llego el mensaje: GUARDAR_PLATO");

		guardar_plato_nuevo = deserializar_guardar_plato(cliente_fd);

		log_info(logger, "Pedido: %d", guardar_plato_nuevo->idPedido);

		tabla_segmentos_restaurante = dictionary_get(dic_restaurante_tabla_segmentos, guardar_plato_nuevo->nombreRestaurante);

		if (tabla_segmentos_restaurante == NULL) {
			log_info(logger, "No existe la tabla de segmentos");
			respuesta = false;
		} else {

			pthread_mutex_lock(&mutexBuscarSegmento);
			segmento_pedido = buscar_segmento(tabla_segmentos_restaurante, guardar_plato_nuevo->idPedido);
			pthread_mutex_unlock(&mutexBuscarSegmento);

			if (segmento_pedido == NULL) {
				log_info(logger, "No existe el segmento");
				respuesta = false;
			} else {

				pthread_mutex_lock(&mutexBuscarPagina);
				t_pagina* pagina_plato = buscar_pagina(segmento_pedido->tabla_paginas, guardar_plato_nuevo->nombrePlato);
				pthread_mutex_unlock(&mutexBuscarPagina);

				if (pagina_plato == NULL) {
					bool esta_libre(void* parametro) {
						return ((entradaSwap*) parametro)->libre;
					}

					log_info(logger, "No existe la pagina");

					t_list* marcosSwapLibres = list_filter(entradas_swap, esta_libre);

					if(list_size(marcosSwapLibres) == 0){
						log_info(logger, "No hay espacio en swap para agregar mas paginas");
						respuesta = false;
					} else {
						pagina_plato = malloc(sizeof(t_pagina));
						pagina_plato->cantidadLista = 0;
						pagina_plato->cantidadPlato = guardar_plato_nuevo->cantidadPlato;
						pagina_plato->nombrePlato = malloc(tamanio_nombre_plato_maximo);
						memcpy(pagina_plato->nombrePlato, guardar_plato_nuevo->nombrePlato, guardar_plato_nuevo->tamanioNombrePlato);

						list_add(segmento_pedido->tabla_paginas, pagina_plato);

						pthread_mutex_lock(&mutexEscribirMemoria);
						escribir_en_memoria(pagina_plato, false, guardar_plato_nuevo->idPedido, false);
						escribir_en_swap(pagina_plato, guardar_plato_nuevo->idPedido);
						pthread_mutex_unlock(&mutexEscribirMemoria);

						respuesta = true;
					}

					list_destroy(marcosSwapLibres);

				} else {
					if (!esta_en_memoria(pagina_plato, segmento_pedido->idPedido)) {
						bool entrada_buscada(void* parametro) {
							entradaSwap* entrada = (entradaSwap *) parametro;
							return (segmento_pedido->idPedido == entrada->idPedido) && !strcmp(pagina_plato->nombrePlato,entrada->nombrePlato);
						}
						t_pagina* paginaBuscada = malloc(sizeof(t_pagina));
						paginaBuscada->nombrePlato = malloc(24);

						entradaSwap* pagina_swap = list_find(entradas_swap, entrada_buscada);

						pthread_mutex_lock(&mutexBuscarSwap);
						buscar_en_swap(pagina_swap->indiceMarcoSwap,paginaBuscada);
						pthread_mutex_unlock(&mutexBuscarSwap);

						pthread_mutex_lock(&mutexEscribirMemoria);
						escribir_en_memoria(paginaBuscada, false, segmento_pedido->idPedido, true);
						pthread_mutex_unlock(&mutexEscribirMemoria);
					}

					log_info(logger, "El plato ya existe. Anexando la cantidad a cocinar.");
					pagina_plato->cantidadPlato += guardar_plato_nuevo->cantidadPlato;


					pthread_mutex_lock(&mutexEscribirMemoria);
					escribir_en_memoria(pagina_plato, true, guardar_plato_nuevo->idPedido, true);
					pthread_mutex_unlock(&mutexEscribirMemoria);

					respuesta = true;
				}
			}
		}

		enviar_respuesta(respuesta, cliente_fd);

		free(guardar_plato_nuevo->nombrePlato);
		free(guardar_plato_nuevo->nombreRestaurante);
		free(guardar_plato_nuevo);

		break;
	case OBTENER_PEDIDO:
		log_info(logger, "COMANDA:: Me llego el mensaje: OBTENER_PEDIDO");

		obtener_pedido_nuevo = deserializar_obtener_pedido(cliente_fd);

		log_info(logger, "Pedido: %d", obtener_pedido_nuevo->idPedido);

		tabla_segmentos_restaurante = dictionary_get(dic_restaurante_tabla_segmentos, obtener_pedido_nuevo->nombreRestaurante);

		if (tabla_segmentos_restaurante == NULL) {
			log_info(logger, "No existe la tabla de segmentos");
			respuesta = false;
		} else {

			pthread_mutex_lock(&mutexBuscarSegmento);
			segmento_pedido = buscar_segmento(tabla_segmentos_restaurante, obtener_pedido_nuevo->idPedido);
			pthread_mutex_unlock(&mutexBuscarSegmento);

			if (segmento_pedido == NULL) {
				log_info(logger, "No existe el segmento");
				respuesta = false;
			} else {
				bool entrada_buscada(void* parametro) {
					entradaSwap* entrada = (entradaSwap *) parametro;
					return (segmento_pedido->idPedido == entrada->idPedido);
				}

				infoPedido = obtener_info_pedido(segmento_pedido);

				t_list* marcosEnSwap = list_filter(entradas_swap, entrada_buscada);

				for(int i=0; i < list_size(marcosEnSwap); i++){
					entradaSwap* entrada = list_get(marcosEnSwap,i);

					void* esta_agregado(){
						bool el_que_quiero(void* elem){
							t_pagina* param = (t_pagina*) elem;
							return !strcmp(param->nombrePlato, entrada->nombrePlato);
						}
						return list_find(infoPedido, el_que_quiero);
					}

					if(esta_agregado() == NULL){
						t_pagina* buscada = malloc(sizeof(t_pagina));
						buscada->nombrePlato = malloc(24);

						pthread_mutex_lock(&mutexBuscarSwap);
						buscar_en_swap(entrada->indiceMarcoSwap, buscada);
						pthread_mutex_unlock(&mutexBuscarSwap);

						pthread_mutex_lock(&mutexEscribirMemoria);
						escribir_en_memoria(buscada, false, segmento_pedido->idPedido, false);
						pthread_mutex_unlock(&mutexEscribirMemoria);

						list_add(infoPedido, buscada);
					}
				}


				respuesta = true;

				list_destroy(marcosEnSwap);
			}
		}

		enviar_respuesta(respuesta, cliente_fd);

		free(obtener_pedido_nuevo->nombreRestaurante);
		free(obtener_pedido_nuevo);

		if(respuesta){
			log_info(logger, "Pedido obtenido con exito");
			enviar_datos_pedido_comanda(segmento_pedido->estadoPedido, infoPedido, cliente_fd);
		// Free de todos los elementos de la lista
//		int cant = list_size(infoPedido);
//		for (int i = 0; i < cant; i++) {
//			t_pagina* elem = list_get(infoPedido, i);
//			free(elem->nombrePlato);
//			free(elem);
//		}

		list_destroy(infoPedido);
		}


		break;
	case CONFIRMAR_PEDIDO:
		log_info(logger, "COMANDA:: Me llego el mensaje: CONFIRMAR_PEDIDO");

		respuesta = true;

		confirmar_pedido_nuevo = deserializar_confirmar_pedido(cliente_fd);

		log_info(logger, "Pedido: %d", confirmar_pedido_nuevo->idPedido);

		tabla_segmentos_restaurante = dictionary_get(dic_restaurante_tabla_segmentos, confirmar_pedido_nuevo->nombreRestaurante);

		if (tabla_segmentos_restaurante == NULL) {
			log_info(logger, "No existe la tabla de segmentos");
			respuesta = false;
		} else {

			pthread_mutex_lock(&mutexBuscarSegmento);
			segmento_pedido = buscar_segmento(tabla_segmentos_restaurante, confirmar_pedido_nuevo->idPedido);
			pthread_mutex_unlock(&mutexBuscarSegmento);

			if (segmento_pedido == NULL) {
				log_info(logger, "No existe el segmento");
				respuesta = false;
			} else {
				if (segmento_pedido->estadoPedido == PENDIENTE) {
					segmento_pedido->estadoPedido = CONFIRMADO;
					respuesta = true;
				}
			}
		}

		enviar_respuesta(respuesta, cliente_fd);

		break;
	case PLATO_LISTO:
		log_info(logger, "COMANDA:: Me llego el mensaje: PLATO_LISTO");

		plato_listo_nuevo = deserializar_plato_listo(cliente_fd);

		log_info(logger, "Pedido: %d", plato_listo_nuevo->idPedido);

		tabla_segmentos_restaurante = dictionary_get(dic_restaurante_tabla_segmentos, plato_listo_nuevo->nombreRestaurante);

		if (tabla_segmentos_restaurante == NULL) {
			log_info(logger, "No existe la tabla de segmentos para el restaurante");
			respuesta = false;
		} else {

			pthread_mutex_lock(&mutexBuscarSegmento);
			segmento_pedido = buscar_segmento(tabla_segmentos_restaurante,plato_listo_nuevo->idPedido);
			pthread_mutex_unlock(&mutexBuscarSegmento);

			if (segmento_pedido == NULL) {
				log_info(logger, "No existe el pedido");
				respuesta = false;
			} else {

				pthread_mutex_lock(&mutexBuscarPagina);
				t_pagina* pagina_plato = buscar_pagina(segmento_pedido->tabla_paginas,plato_listo_nuevo->nombrePlato);
				pthread_mutex_unlock(&mutexBuscarPagina);

				if (pagina_plato == NULL) {
					log_info(logger, "No existe el plato");
					respuesta = false;
				} else {
					if (!esta_en_memoria(pagina_plato,segmento_pedido->idPedido)) {
						bool entrada_buscada(void* parametro) {
							entradaSwap* entrada = (entradaSwap *) parametro;
							return (segmento_pedido->idPedido == entrada->idPedido) && !strcmp(pagina_plato->nombrePlato,entrada->nombrePlato);
						}
						entradaSwap* pagina_swap = list_find(entradas_swap, entrada_buscada);


						pthread_mutex_lock(&mutexBuscarSwap);
						buscar_en_swap(pagina_swap->indiceMarcoSwap, pagina_plato);
						pthread_mutex_unlock(&mutexBuscarSwap);

						pthread_mutex_lock(&mutexEscribirMemoria);
						escribir_en_memoria(pagina_plato, false, segmento_pedido->idPedido, true);
						pthread_mutex_unlock(&mutexEscribirMemoria);
					}
					if (segmento_pedido->estadoPedido == CONFIRMADO) {
						log_info(logger, "El plato ya existe. Anexando la cantidad lista.");
						pagina_plato->cantidadLista += 1;

						pthread_mutex_lock(&mutexEscribirMemoria);
						escribir_en_memoria(pagina_plato, true, plato_listo_nuevo->idPedido, true);
						pthread_mutex_unlock(&mutexEscribirMemoria);

						if(platos_terminados(segmento_pedido)){
							segmento_pedido->estadoPedido = TERMINADO;
						}

						respuesta = true;
					} else {
						log_info(logger, "No confirmado");
						respuesta = false;
					}
				}
			}
		}

		enviar_respuesta(respuesta, cliente_fd);

		free(plato_listo_nuevo->nombrePlato);
		free(plato_listo_nuevo->nombreRestaurante);
		free(plato_listo_nuevo);

		break;
	case FINALIZAR_PEDIDO:
		log_info(logger, "COMANDA:: Me llego el mensaje: FINALIZAR_PEDIDO");

		finalizar_pedido_nuevo = deserializar_finalizar_pedido(cliente_fd);

		log_info(logger, "Pedido: %d", finalizar_pedido_nuevo->idPedido);

		log_info(logger, "Restaurante: %s", finalizar_pedido_nuevo->nombreRestaurante);

		tabla_segmentos_restaurante = dictionary_get(dic_restaurante_tabla_segmentos, finalizar_pedido_nuevo->nombreRestaurante);

		if (tabla_segmentos_restaurante == NULL) {
			log_info(logger, "No existe la tabla de segmentos");
			respuesta = false;
		} else {

			pthread_mutex_lock(&mutexBuscarSegmento);
			segmento_pedido = buscar_segmento(tabla_segmentos_restaurante, finalizar_pedido_nuevo->idPedido);
			pthread_mutex_unlock(&mutexBuscarSegmento);

			if (segmento_pedido == NULL) {
				log_info(logger, "No existe el segmento");
				respuesta = false;
			} else {
				// TODO mutex si estoy eliminando, tengo q esperar antes de Crear un Pedido, porque de lo contrario puedo llegar a rechazar la creación porque esta llena la memoria, pero 2ms despues sí hay lugar...
				pthread_mutex_lock(&mutexEliminarDeMemoria);
				// Elimina el pedido de memoria
				eliminar_entradas_por_pedido(finalizar_pedido_nuevo->idPedido);

				// Elimina el pedido de swap
				eliminar_pedido_de_swap(finalizar_pedido_nuevo->idPedido);

				// Elimina el pedido de la tabla de segmentos
				eliminar_pedido(segmento_pedido, tabla_segmentos_restaurante);
				pthread_mutex_unlock(&mutexEliminarDeMemoria);

				respuesta = true;
			}
		}

		enviar_respuesta(respuesta, cliente_fd);

//		for(int i=0; i < 32; i++){
//			t_pagina* prueba = malloc(sizeof(t_pagina));
//
//			buscar_en_swap(i, prueba);
//
//			log_info(logger,"Pagina %d de SWAP",i);
//			log_info(logger,"Nombre del plato: %s",prueba->nombrePlato);
//			log_info(logger,"Cantidad pedida: %d", prueba->cantidadPlato);
//			log_info(logger,"Cantidad lista: %d", prueba->cantidadLista);
//		}

		break;
	case CERRAR_CONEXION: // DEBERIA IR OBTENER RECETA
			log_info(logger, "COMANDA:: CERRAR CONEXION");
			close(cliente_fd);
			break;
	case -1: // DEBERIA IR OBTENER RECETA
		log_info(logger, "COMANDA:: ERROR OP_CODE RECIBIDO");
		pthread_exit(NULL);
		break;
	default:
		break;
	}
}

bool esta_en_memoria(t_pagina* pagina, uint32_t idPedido) {
	t_list* marcosEnMemoria = entradas_segun_pedido(idPedido);
	int cantidadMarcos = list_size(marcosEnMemoria);
	int offset = 0;
	bool encontrado = false;

	for (int i = 0; i < cantidadMarcos && !encontrado; i++) {
		entradaTablaMarcos* entrada = list_get(marcosEnMemoria, i);
		uint32_t cantidadLista;
		uint32_t cantidadPlato;
		char* nombrePlato = malloc(24);

		memcpy(&cantidadLista, entrada->marco + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(&cantidadPlato, entrada->marco + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(nombrePlato, entrada->marco + offset, 24);

		if (!strcmp(pagina->nombrePlato, nombrePlato)) {
			encontrado = true;
		} else {
			encontrado = false;
		}

		offset = 0;

		free(nombrePlato);
	}

	list_destroy(marcosEnMemoria);

	return encontrado;
}

t_list* obtener_info_pedido(t_segmento* segmentoPedido){
	t_list* entradasDelPedido = entradas_segun_pedido(segmentoPedido->idPedido);

	int cantMarcos = list_size(entradasDelPedido);

	t_list* infoDelPedido = list_create();

	for (int i = 0; i < cantMarcos; i++) {
		t_pagina* info = malloc(sizeof(t_pagina));
		info->nombrePlato = malloc(24);
		int offset = 0;
		entradaTablaMarcos* entrada = list_get(entradasDelPedido, i);

		if(!strcmp(algoritmo_reemplazo, "LRU"))
			entrada->timeStamp = temporal_get_string_time();
		if(!strcmp(algoritmo_reemplazo, "CLOCK_ME"))
			entrada->bitUso = true;


		memcpy(&info->cantidadLista, entrada->marco + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(&info->cantidadPlato, entrada->marco + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(info->nombrePlato, entrada->marco+offset, 24);

		list_add(infoDelPedido, info);

//        free(info->nombrePlato);
//        free(info); // TODO no falta el free del nombre?
	}

	list_destroy(entradasDelPedido);

	return infoDelPedido;
}

bool platos_terminados(t_segmento* segmentoPedido) {
	bool entrada_buscada(void* parametro) {
		entradaSwap* entrada = (entradaSwap *) parametro;
		return (segmentoPedido->idPedido == entrada->idPedido);
	}

	t_list* info = obtener_info_pedido(segmentoPedido);

	t_list* marcosEnSwap = list_filter(entradas_swap, entrada_buscada);

	for (int i = 0; i < list_size(marcosEnSwap); i++) {
		entradaSwap* entrada = list_get(marcosEnSwap, i);

		void* esta_agregado() {
			bool el_que_quiero(void* elem) {
				t_pagina* param = (t_pagina*) elem;
				return !strcmp(param->nombrePlato, entrada->nombrePlato);
			}
			return list_find(info, el_que_quiero);
		}

		if (esta_agregado() == NULL) {
			t_pagina* buscada = malloc(sizeof(t_pagina));
			buscada->nombrePlato = malloc(24);

			pthread_mutex_lock(&mutexBuscarSwap);
			buscar_en_swap(entrada->indiceMarcoSwap, buscada);
			pthread_mutex_unlock(&mutexBuscarSwap);

			//pthread_mutex_lock(&mutexEscribirMemoria);
			//escribir_en_memoria(buscada, false, segmentoPedido->idPedido, false);
			//pthread_mutex_unlock(&mutexEscribirMemoria);

			list_add(info, buscada);
		}

	}
	bool terminados = true;

	for (int i = 0; i < list_size(info); i++) {
		t_pagina* aux = list_get(info, i);
		if (aux->cantidadLista < aux->cantidadPlato)
			terminados = false;
	}

	list_destroy(marcosEnSwap);

	list_destroy(info);

	return terminados;
}

t_list* entradas_segun_pedido(uint32_t idPedido){
	bool buscar_entradas_pedido(void* parametro){
		entradaTablaMarcos* entradaEnMem = (entradaTablaMarcos*) parametro;
		return idPedido == entradaEnMem->idPedido;
	}

	t_list* entradas = list_filter(tablaDeMarcos, buscar_entradas_pedido);

	return entradas;
}

bool el_pedido_esta_en_memoria(t_segmento* segmentoPedido){
	uint32_t idPedido = segmentoPedido->idPedido;

	t_list* tablaPaginas = segmentoPedido->tabla_paginas;

	return list_all_satisfy(tablaPaginas, esta_en_memoria);
}

t_segmento* buscar_segmento(t_list* tabla_segmentos, uint32_t idPedido){
	bool es_el_segmento_que_quiero(void* parametro) {
		t_segmento* segmentoDeLista = (t_segmento *) parametro;
		return idPedido == segmentoDeLista->idPedido;
	}

	return list_find(tabla_segmentos, es_el_segmento_que_quiero);
}

t_pagina* buscar_pagina(t_list* tabla_paginas, char* nombrePlato){
	bool es_la_pagina_que_quiero(void* parametro) {
		t_pagina* paginaDeLista = (t_pagina *) parametro;
		return !strcmp(nombrePlato, paginaDeLista->nombrePlato);
	}

	return list_find(tabla_paginas, es_la_pagina_que_quiero);
}

void eliminar_entradas_por_pedido(int idPedido){
	int cantEntradas = list_size(tablaDeMarcos);

	for (int i = 0; i < cantEntradas; i++) {
		entradaTablaMarcos* entrada = list_get(tablaDeMarcos, i);
		if(entrada->idPedido == idPedido) entrada->libre = true;
	}
}

void eliminar_pedido(t_segmento* segmentoPedido, t_list* tablaSegmentos){
	bool es_el_segmento_que_quiero(void* parametro) {
		t_segmento* segmentoDeLista = (t_segmento *) parametro;
		return segmentoPedido->idPedido == segmentoDeLista->idPedido;
	}

	void eliminar_pagina(t_pagina* pag) {
		free(pag);
	}

	void eliminar_segmento(t_segmento* segmento) {
		list_destroy(segmento->tabla_paginas);
		free(segmento);
	}

	for(int i=0; i < list_size(segmentoPedido->tabla_paginas); i++){
		free(((t_pagina *) list_get(segmentoPedido->tabla_paginas,i))->nombrePlato);
		list_remove_and_destroy_element(segmentoPedido->tabla_paginas,i,eliminar_pagina);
	}

	list_remove_and_destroy_by_condition(tablaSegmentos,es_el_segmento_que_quiero, eliminar_segmento);

}

void buscar_en_swap(int pos, t_pagina* pagina){
	log_info(logger,"Buscando pagina en swap");
	fseek(espacio_swap, pos * sizeof(t_pagina), SEEK_SET);
	fread(pagina,sizeof(t_pagina),1,espacio_swap);
}

void escribir_en_swap(t_pagina* pagina, uint32_t idPedido){
	bool esta_libre(void* parametro) {
		return ((entradaSwap*) parametro)->libre;
	}

	log_info(logger,"Escribiendo pagina en swap");

	t_list* marcosSwapLibres = list_filter(entradas_swap, esta_libre);

	entradaSwap* entradaLibre = list_get(marcosSwapLibres, 0);

	fseek(espacio_swap, entradaLibre->indiceMarcoSwap * sizeof(t_pagina), SEEK_SET);
	fwrite(pagina,sizeof(t_pagina),1,espacio_swap);

	entradaLibre->libre = false;
	entradaLibre->idPedido = idPedido;
	entradaLibre->nombrePlato = pagina->nombrePlato;

	list_destroy(marcosSwapLibres);
}

void borrar_de_swap(uint32_t pos){
	t_pagina* paginaVacia = malloc(sizeof(t_pagina));
	paginaVacia->cantidadLista = -1;
	paginaVacia->cantidadPlato = -1;
	paginaVacia->nombrePlato = NULL;

	fseek(espacio_swap, pos * sizeof(t_pagina), SEEK_SET);
	fwrite(paginaVacia,sizeof(t_pagina),1,espacio_swap);

	free(paginaVacia);
}

void eliminar_pedido_de_swap(uint32_t idPedido){
	bool entradas_del_pedido(void* parametro) {
		entradaSwap* entrada = (entradaSwap *) parametro;
		return idPedido == entrada->idPedido;
	}
	t_list* marcosEnSwap = list_filter(entradas_swap, entradas_del_pedido);

	for(int i=0; i < list_size(marcosEnSwap); i++){
		entradaSwap* entrada = list_get(marcosEnSwap,i);
		entrada->libre = true;

		borrar_de_swap(entrada->indiceMarcoSwap);
	}

}
