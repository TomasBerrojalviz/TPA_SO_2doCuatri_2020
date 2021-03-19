#include "Comanda.h"

int main(void) {

	inicializar_modulo();

	pthread_create(&hiloReceive, NULL, (void*) iniciar_servidor, NULL);
	pthread_join(hiloReceive, NULL);

	terminar_programa();

	return EXIT_SUCCESS;
}

void inicializar_modulo(void){
	leer_config();	//Carga las config
	logger = iniciar_logger();	//Carga el logger
	dic_restaurante_tabla_segmentos = dictionary_create();
	tablaDeMarcos = list_create();
	entradas_swap = list_create();
	pedidosCreados = list_create();

	pthread_mutex_init(&mutexTablaMarcos, NULL);
	pthread_mutex_init(&mutexEntradasSwap, NULL);
	pthread_mutex_init(&mutexMemoria, NULL);
	pthread_mutex_init(&mutexSwap, NULL);
	pthread_mutex_init(&mutexBuscarSegmento, NULL);
	pthread_mutex_init(&mutexBuscarPagina, NULL);
	pthread_mutex_init(&mutexBuscarInfoPedido, NULL);

	iniciar_memoria();

	// Leer Algoritmo Seleccion de Victima
	if ((seleccionar_victima = convertir(algoritmo_reemplazo)) == NULL) {
		log_info(logger, "Error al leer el algoritmo de Reemplazo");
	} else {log_info(logger, "El algoritmo de reemplazo seleccionado es: %s", algoritmo_reemplazo);}
}

t_log* iniciar_logger(void) {
	return log_create("/home/utnso/tp-2020-2c-breakfastClub/Comanda/archivo_log", "Comanda.c", 1, LOG_LEVEL_INFO);
}



void leer_config(void) {
	config = config_create("/home/utnso/tp-2020-2c-breakfastClub/Comanda/comanda.config");

	pthread_mutex_init(&mutexTablaSegmentos, NULL);
	pthread_mutex_init(&mutexInfoPedidoAEnviar, NULL);
	pthread_mutex_init(&mutexInfoPedidoAEnviar, NULL);
	pthread_mutex_init(&mutexEscribirMemoria, NULL);
	pthread_mutex_init(&mutexEliminarDeMemoria, NULL);
	pthread_mutex_init(&mutexPedidosCreados, NULL);
	pthread_mutex_init(&mutexFree, NULL);
	pthread_mutex_init(&mutexVictima, NULL);

	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	tamanio_swap = config_get_int_value(config, "TAMANO_SWAP");
	tamanio_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
	tamanio_nombre_plato_maximo = config_get_int_value(config, "TAMANIO_NOMBRE_PLATO");
	ip_comanda = config_get_string_value(config, "IP_COMANDA");
	algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	archivo_log = config_get_string_value(config, "ARCHIVO_LOG");
}

void terminar_programa() {
	log_destroy(logger);

	config_destroy(config);

	close(socketConexionApp);
}
