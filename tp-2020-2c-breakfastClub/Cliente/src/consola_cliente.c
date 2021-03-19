#include<stdio.h>
#include<stdlib.h>
#include "consola_cliente.h"

#define ERROR_ARGUMENTOS

void leer_consola(void) {
	char* leido = readline(">");

	while (strcmp(leido, "\0")) {
		char** palabras_del_mensaje = string_split(leido, " ");

		if (!strcmp(palabras_del_mensaje[0], "consultarRestaurantes")) {
			if (contar_elementos_array(palabras_del_mensaje) != 1) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {
				if (socketConexionApp > 0) {
					enviar_consultar_restaurantes(socketConexionApp);
				} else {log_info(logger, "No esta conectada la app");}
			}
		}

		if (!strcmp(palabras_del_mensaje[0], "seleccionarRestaurante")) { // seleccionarRestaurante nombreRestaurante
			if (contar_elementos_array(palabras_del_mensaje) != 2) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				seleccionar_restaurante* msg = malloc(sizeof(seleccionar_restaurante));
				msg->nombreRestaurante = palabras_del_mensaje[1];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;

				enviar_datos_cliente(socketConexionApp);
				enviar_seleccionar_restaurante(msg, socketConexionApp);

				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "obtenerRestaurante")) { // obtenerRestaurante nombreRestaurante
			if (contar_elementos_array(palabras_del_mensaje) != 2) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				obtener_restaurante* msg = malloc(sizeof(obtener_restaurante));
				msg->nombreRestaurante = palabras_del_mensaje[1];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;

				enviar_obtener_restaurante(msg, socketConexionSindicato);

				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "consultarPlatos")) { // consultarPlatos destinatario nombreRestaurante
			if (contar_elementos_array(palabras_del_mensaje) != 3) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				consultar_platos* msg = malloc(sizeof(consultar_platos));
				msg->nombreRestaurante = palabras_del_mensaje[2];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;

				uint32_t socket;

				if (!strcmp("app", palabras_del_mensaje[1]))
					socket = socketConexionApp;
				if (!strcmp("restaurante", palabras_del_mensaje[1]))
					socket = socketConexionRestaurante;
				if (!strcmp("sindicato", palabras_del_mensaje[1]))
					socket = socketConexionSindicato;

				enviar_consultar_platos(msg, socket);

				free(msg);
			}
		}

		if (!strcmp(palabras_del_mensaje[0], "crearPedido")) { // crearPedido destinatario
			if (contar_elementos_array(palabras_del_mensaje) != 2) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				uint32_t socket;

				if (!strcmp("app", palabras_del_mensaje[1])){
					socket = socketConexionApp;
					enviar_datos_cliente(socketConexionApp);
				}
				if (!strcmp("restaurante", palabras_del_mensaje[1]))
					socket = socketConexionRestaurante;

				enviar_crear_pedido(socket);

			}

		}

		if (!strcmp(palabras_del_mensaje[0], "guardarPedido")) { // guardarPedido destinatario nombreRestaurante idPedido
			if (contar_elementos_array(palabras_del_mensaje) != 4) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				guardar_pedido* msg = malloc(sizeof(guardar_pedido));
				msg->nombreRestaurante = palabras_del_mensaje[2];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;
				msg->id_pedido = atoi(palabras_del_mensaje[3]);

				if (!strcmp("comanda", palabras_del_mensaje[1])){
					socketConexionComanda = crear_conexion(ipComanda, puertoComanda);
					if (socketConexionComanda == -1) {
						log_info(logger, "La Comanda no esta conectada");
					} else if (socketConexionComanda > 0) {
						log_info(logger, "La Comanda esta conectada");
						enviar_guardar_pedido(msg, socketConexionComanda);
						close(socketConexionComanda);
					}
				}
				if (!strcmp("sindicato", palabras_del_mensaje[1]))
					enviar_guardar_pedido(msg, socketConexionSindicato);


				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "aniadirPlato")) { // aniadirPlato destinatario plato idPedido
			if (contar_elementos_array(palabras_del_mensaje) != 4) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				aniadir_plato* msg = malloc(sizeof(aniadir_plato));
				msg->plato = palabras_del_mensaje[2];
				msg->tamanioPlato = strlen(msg->plato) + 1;
				msg->idPedido = atoi(palabras_del_mensaje[3]);

				uint32_t socket;

				if (!strcmp("app", palabras_del_mensaje[1])){
					socket = socketConexionApp;
					enviar_datos_cliente(socketConexionApp);
				}
				if (!strcmp("restaurante", palabras_del_mensaje[1]))
					socket = socketConexionRestaurante;

				enviar_aniadir_plato(msg, socket);

				free(msg);
			}
		}

		if (!strcmp(palabras_del_mensaje[0], "guardarPlato")) { // guardarPlato destinatario nombreRestaurante nombrePlato cantidad idPedido
			if (contar_elementos_array(palabras_del_mensaje) != 6) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				guardar_plato* msg = malloc(sizeof(guardar_plato));
				msg->nombreRestaurante = palabras_del_mensaje[2];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;
				msg->id_pedido = atoi(palabras_del_mensaje[5]);
				msg->nombrePlato = palabras_del_mensaje[3];
				msg->tamanioNombrePlato = strlen(msg->nombrePlato) + 1;
				msg->cantidad_platos = atoi(palabras_del_mensaje[4]);

				uint32_t socket;

				if (!strcmp("comanda", palabras_del_mensaje[1])){
					socketConexionComanda = crear_conexion(ipComanda, puertoComanda);
					if (socketConexionComanda == -1) {
						log_info(logger, "La Comanda no esta conectada");
					} else if (socketConexionComanda > 0) {
						log_info(logger, "La Comanda esta conectada");
						enviar_guardar_plato(msg, socketConexionComanda);
						close(socketConexionComanda);
					}
				}
				if (!strcmp("sindicato", palabras_del_mensaje[1]))
					enviar_guardar_plato(msg, socketConexionSindicato);

				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "confirmarPedido")) { // confirmarPedido destinatario idPedido nombreRestaurante
			if (contar_elementos_array(palabras_del_mensaje) != 4) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				confirmar_pedido* msg = malloc(sizeof(confirmar_pedido));
				msg->id_pedido = atoi(palabras_del_mensaje[2]);
				msg->nombreRestaurante = palabras_del_mensaje[3];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;


				if (!strcmp("app", palabras_del_mensaje[1])) {
					pthread_t hiloPed;
					confirmar_pedido* parametro = malloc(sizeof(confirmar_pedido));

					memcpy(&parametro->id_pedido, &(msg->id_pedido), sizeof(uint32_t));

					memcpy(&parametro->tamanioNombre, &(msg->tamanioNombre), sizeof(uint32_t));
					parametro->nombreRestaurante = malloc(parametro->tamanioNombre);

					memcpy(parametro->nombreRestaurante, (msg->nombreRestaurante), msg->tamanioNombre);

					pthread_create(&hiloPed, NULL, (void*) confirmar_pedido_app,(void*) parametro);
					pthread_detach(hiloPed);
				}
				if (!strcmp("restaurante", palabras_del_mensaje[1])) {

					pthread_t hiloPedRestaurante;
					confirmar_pedido* parametro = malloc(sizeof(confirmar_pedido));

					memcpy(&parametro->id_pedido, &(msg->id_pedido), sizeof(uint32_t));

					memcpy(&parametro->tamanioNombre, &(msg->tamanioNombre), sizeof(uint32_t));
					parametro->nombreRestaurante = malloc(parametro->tamanioNombre);

					memcpy(parametro->nombreRestaurante, (msg->nombreRestaurante), msg->tamanioNombre);

					pthread_create(&hiloPedRestaurante, NULL, (void*) confirmar_pedido_restaurante,(void*) parametro);
					pthread_detach(hiloPedRestaurante);
				}
				if (!strcmp("comanda", palabras_del_mensaje[1])){
					socketConexionComanda = crear_conexion(ipComanda, puertoComanda);
					if (socketConexionComanda == -1) {
						log_info(logger, "La Comanda no esta conectada");
					} else if (socketConexionComanda > 0) {
						log_info(logger, "La Comanda esta conectada");
						enviar_confirmar_pedido(msg, socketConexionComanda);
						close(socketConexionComanda);
					}
				}
				if (!strcmp("sindicato", palabras_del_mensaje[1]))
					enviar_confirmar_pedido(msg, socketConexionSindicato);

				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "platoListo")) { // platoListo destinatario nombreRestaurante nombrePlato idPedido
			if (contar_elementos_array(palabras_del_mensaje) != 5) {
				log_info(logger, "Cantidad incorrecta de argumentos");
				return ERROR_ARGUMENTOS;
			} else {

				plato_listo* msg = malloc(sizeof(plato_listo));
				msg->nombreRestaurante = palabras_del_mensaje[2];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;
				msg->id_pedido = atoi(palabras_del_mensaje[4]);
				msg->nombrePlato = palabras_del_mensaje[3];
				msg->tamanioNombrePlato = strlen(msg->nombrePlato) + 1;


				if (!strcmp("app", palabras_del_mensaje[1]))
					enviar_plato_listo(msg,socketConexionApp);
				if (!strcmp("comanda", palabras_del_mensaje[1])){
					socketConexionComanda = crear_conexion(ipComanda, puertoComanda);
					if (socketConexionComanda == -1) {
						log_info(logger, "La Comanda no esta conectada");
					} else if (socketConexionComanda > 0) {
						log_info(logger, "La Comanda esta conectada");
						enviar_plato_listo(msg, socketConexionComanda);
						close(socketConexionComanda);
					}
				}
				if (!strcmp("sindicato", palabras_del_mensaje[1]))
					enviar_plato_listo(msg, socketConexionSindicato);


				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "consultarPedido")) { // consultarPedido destinatario idPedido
			if (contar_elementos_array(palabras_del_mensaje) != 3) {
				log_info(logger, "Cantidad incorrecta de argumentos");
				return ERROR_ARGUMENTOS;
			} else {

				consultar_pedido* msg = malloc(sizeof(consultar_pedido));
				msg->id_pedido = atoi(palabras_del_mensaje[2]);

				uint32_t socket;

				if (!strcmp("app", palabras_del_mensaje[1])){
					socket = socketConexionApp;
					enviar_datos_cliente(socketConexionApp);
				}
				if (!strcmp("restaurante", palabras_del_mensaje[1]))
					socket = socketConexionRestaurante;

				enviar_consultar_pedido(msg, socket);

				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "obtenerPedido")) { // obtenerPedido destinatario nombreRestaurante idPedido
			if (contar_elementos_array(palabras_del_mensaje) != 4) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				obtener_pedido* msg = malloc(sizeof(obtener_pedido));
				msg->nombreRestaurante = palabras_del_mensaje[2];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;
				msg->id_pedido = atoi(palabras_del_mensaje[3]);


				if (!strcmp("comanda", palabras_del_mensaje[1])){
					socketConexionComanda = crear_conexion(ipComanda, puertoComanda);
					if (socketConexionComanda == -1) {
						log_info(logger, "La Comanda no esta conectada");
					} else if (socketConexionComanda > 0) {
						log_info(logger, "La Comanda esta conectada");
						enviar_obtener_pedido(msg, socketConexionComanda);
						close(socketConexionComanda);
					}
				}

				if (!strcmp("sindicato", palabras_del_mensaje[1]))
					enviar_obtener_pedido(msg, socketConexionSindicato);


				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "finalizarPedido")) { // finalizarPedido nombreRestaurante idPedido
			if (contar_elementos_array(palabras_del_mensaje) != 3) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				finalizar_pedido* msg = malloc(sizeof(finalizar_pedido));
				msg->nombreRestaurante = palabras_del_mensaje[1];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;
				msg->id_pedido = atoi(palabras_del_mensaje[2]);

				socketConexionComanda = crear_conexion(ipComanda, puertoComanda);
				if (socketConexionComanda == -1) {
					log_info(logger, "La Comanda no esta conectada");
				} else if (socketConexionComanda > 0) {
					log_info(logger, "La Comanda esta conectada");
					enviar_finalizar_pedido(msg, socketConexionComanda);
					close(socketConexionComanda);
				}

				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "terminarPedido")) { // terminarPedido nombreRestaurante idPedido
			if (contar_elementos_array(palabras_del_mensaje) != 3) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			} else {

				terminar_pedido* msg = malloc(sizeof(terminar_pedido));
				msg->nombreRestaurante = palabras_del_mensaje[1];
				msg->tamanioNombre = strlen(msg->nombreRestaurante) + 1;
				msg->id_pedido = atoi(palabras_del_mensaje[2]);

				enviar_terminar_pedido(msg, socketConexionSindicato);

				free(msg);

			}
		}

		if (!strcmp(palabras_del_mensaje[0], "obtenerReceta")) { // obtenerReceta nombreReceta
			if (contar_elementos_array(palabras_del_mensaje) != 2) {
				log_info(logger, "Cantidad incorrecta de argumentos");
				return ERROR_ARGUMENTOS;
			} else {

				obtener_receta* msg = malloc(sizeof(terminar_pedido));
				msg->nombreReceta = palabras_del_mensaje[1];
				msg->tamanioNombre = strlen(msg->nombreReceta) + 1;

				enviar_obtener_receta(msg, socketConexionSindicato);

				free(msg);

			}
		}

		int i = 0;
		while(palabras_del_mensaje[i] != NULL){
			free(palabras_del_mensaje[i]);
			i++;
		}
		free(palabras_del_mensaje);
		free(leido);
		leido = readline(">");
	}

	free(leido);
}

int crear_socket_app() {
	int socketConexionApp = crear_conexion(ipApp, puertoApp);
	if (socketConexionApp == -1) {
		log_info(logger, "La App no esta conectada");
	}
	return socketConexionApp;
}

