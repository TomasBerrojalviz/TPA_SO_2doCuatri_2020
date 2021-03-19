#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Cliente.h"

int main(int argc, char* argv[]) {
	inicializar_cliente();

	if(argc == 4){
		strcpy(idCliente,argv[1]);
		posicionX = atoi(argv[2]);
		posicionY = atoi(argv[3]);
		log_info(logger,"Posicion del %s (%d,%d)",idCliente,posicionX,posicionY);
	}

	leer_consola();
}

t_log* iniciar_logger(void) {
	return log_create("/home/utnso/tp-2020-2c-breakfastClub/Cliente/cliente.log", "Cliente.c", 1, LOG_LEVEL_INFO);
}

void inicializar_cliente(void) {
	leer_config();
	logger = iniciar_logger();

	socketConexionSindicato = crear_conexion(ipSindicato, puertoSindicato);
	if (socketConexionSindicato == -1) {
		log_info(logger, "No me pude conectar al Sindicato");
	} else {
		log_info(logger, "Me conecte exitosamente al Sindicato");
	}

	socketConexionComanda = crear_conexion(ipComanda, puertoComanda);
	if (socketConexionComanda == -1) {
		log_info(logger, "No me pude conectar a la Comanda");
	} else {
		log_info(logger, "Me conecte exitosamente a la Comanda");
		enviar_cerrar_conexion(socketConexionComanda);
	}

	socketConexionRestaurante = crear_conexion(ipRestaurante,puertoRestaurante);
	if (socketConexionRestaurante == -1) {
	 	log_info(logger, "No me pude conectar al Restaurante");
	} else {
		log_info(logger, "Me conecte exitosamente al Restaurante");
	}

	socketConexionApp = crear_conexion(ipApp,puertoApp);
	if (socketConexionApp == -1) {
	 	log_info(logger, "No me pude conectar a la App");
	} else {
		log_info(logger, "Me conecte exitosamente a la App");
	}

}

void leer_config(void) {
	config_cliente = config_create("/home/utnso/tp-2020-2c-breakfastClub/Cliente/cliente.config");

	ipComanda = config_get_string_value(config_cliente, "IP_COMANDA");
	puertoComanda = config_get_string_value(config_cliente, "PUERTO_COMANDA");
	ipRestaurante = config_get_string_value(config_cliente, "IP_RESTAURANTE");
	puertoRestaurante = config_get_string_value(config_cliente, "PUERTO_RESTAURANTE");
	ipSindicato = config_get_string_value(config_cliente, "IP_SINDICATO");
	puertoSindicato = config_get_string_value(config_cliente, "PUERTO_SINDICATO");
	ipApp = config_get_string_value(config_cliente, "IP_APP");
	puertoApp = config_get_string_value(config_cliente, "PUERTO_APP");
	ipComanda = config_get_string_value(config_cliente, "IP_COMANDA");
	puertoComanda = config_get_string_value(config_cliente, "PUERTO_COMANDA");
	posicionX = config_get_int_value(config_cliente, "POSICION_X");
	posicionY = config_get_int_value(config_cliente, "POSICION_Y");
	idCliente = config_get_string_value(config_cliente, "ID_CLIENTE");
}

void terminar_programa() {
	log_destroy(logger);
}
