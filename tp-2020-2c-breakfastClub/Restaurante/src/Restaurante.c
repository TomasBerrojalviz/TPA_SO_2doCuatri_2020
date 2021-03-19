#include "Restaurante.h"

int main(int argc, char* argv[]) {

	logger = iniciar_logger();

	if (argc == 2) {
		pathConfig = string_new();
		string_append(&pathConfig, "/home/utnso/tp-2020-2c-breakfastClub/Restaurante/");
		string_append(&pathConfig, argv[1]);
		log_info(logger, "Archivo de configuracion: %s", argv[1]);
	} else {
		strcpy(pathConfig, "/home/utnso/tp-2020-2c-breakfastClub/Restaurante/restaurante.config");
	}

	leer_config();	//Carga las config

	log_info(logger, "Se inicializara el Restaurante");
	inicializar_restaurante();

	log_info(logger, "Se crearan las colas de planif");
	crear_colas_planificacion();

	pthread_create(&planificar_largo, NULL, (void*) planificar_largo_plazo, NULL);
	pthread_detach(planificar_largo);

	log_info(logger, "Se inicializara el hilo con el servidor");
	pthread_create(&hiloReceive, NULL, (void*) iniciar_servidor, NULL);
	pthread_join(hiloReceive, NULL);

	log_info(logger, "Ya se paso el hilo con el servidor");

	return EXIT_SUCCESS;
}

void inicializar_restaurante() {
	uint32_t socketSindicato = conectar_sindicato();

	if(socketSindicato <= 0){
		log_info(logger, "RESTAURANTE: %s EL SINDICATO NO ESTA CONECTADO :: TERMINO EL PROCESO", nombreRestaurante);
		close(socketSindicato);
		exit(0);
	}
	else {
		log_info(logger, "RESTAURANTE: %s", nombreRestaurante);

			t_restaurante* obtenerRestaurante = malloc(sizeof(t_restaurante));
			obtenerRestaurante->tamanioNombre = strlen(nombreRestaurante) + 1;
			obtenerRestaurante->nombreRestaurante = string_duplicate(nombreRestaurante); // todo memleak

			log_info(logger, "obtener RESTAURANTE: %s", obtenerRestaurante->nombreRestaurante);

			enviar_sindicato_obtener_restaurante(obtenerRestaurante, socketSindicato);
			free(obtenerRestaurante->nombreRestaurante);
			free(obtenerRestaurante);

			uint32_t op_code;
			recv(socketSindicato, &op_code, sizeof(uint32_t), MSG_WAITALL);

			uint32_t buffer_size2;
			recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "RESTAURANTE :: ME LLEGO LA RESPUESTA SOBRE OBTENER RESTAURANTE DE PARTE DEL SINDICATO");

			obtener_restaurante_respuesta* obtenerRestauranteRespuesta = deserializar_obtener_restaurante_respuesta(socketSindicato);

			asignar_datos_restaurante(obtenerRestauranteRespuesta);

			log_info(logger, "RESTAURANTE :: SE OBTUVIERON LOS DATOS DEL RESTAURANTE");

			close(socketSindicato);


			pthread_t hiloReceive2;
			pthread_create(&hiloReceive2, NULL, (void*) conectar_app, NULL);
			pthread_detach(hiloReceive2);

			cola_new = queue_create();
			cola_exit = queue_create();
			cola_hornos = queue_create();
			lista_pcbs_platos = list_create();
			id_pcb = 0;

			sem_init(&semaforo_nuevo_pcb, 0, 0);
			sem_init(&semaforo_ready_pcb, 0, 0);
			sem_init(&semaforo_hay_plato_para_el_horno, 0, 0);
			pthread_mutex_init(&mutex_idPcb, NULL);
			pthread_mutex_init(&mutex_cola_hornos, NULL);
			pthread_mutex_init(&mutex_listaPcb, NULL);
			pthread_mutex_init(&mutex_new, NULL);
			pthread_mutex_init(&mutex_cola_ready, NULL);
			pthread_mutex_init(&mutex_exit, NULL);

			// Leer Algoritmo
//			if ((planificacion_corto_plazo = convertir(algoritmo_planificacion)) == NULL) {
			if(algoritmo_planificacion == NULL){
				log_info(logger, "Error al leer el algoritmo de Planificacion");
			} else {
				log_info(logger, "El algoritmo seleccionado es: %s", algoritmo_planificacion);
			}

			free(obtenerRestauranteRespuesta->nombre_restaurante);
			list_destroy(obtenerRestauranteRespuesta->listaTamanioAfinidades);
			list_destroy(obtenerRestauranteRespuesta->afinidad_cocineros);
			list_destroy(obtenerRestauranteRespuesta->listaTamanioPlatos);
			list_destroy(obtenerRestauranteRespuesta->platos);
			list_destroy(obtenerRestauranteRespuesta->precio_platos);
			free(obtenerRestauranteRespuesta);
	}
}

t_log* iniciar_logger(void){
	return log_create("/home/utnso/tp-2020-2c-breakfastClub/Restaurante/archivo_log","Restaurante.c", 1,LOG_LEVEL_INFO);
}

void leer_config(void) {
	config = config_create(pathConfig);

	puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	ip_sindicato = config_get_string_value(config,"IP_SINDICATO");
	puerto_sindicato = config_get_string_value(config,"PUERTO_SINDICATO");
	ip_app = config_get_string_value(config, "IP_APP");
	puerto_app = config_get_string_value(config, "PUERTO_APP");
	quantum = config_get_int_value(config, "QUANTUM");
	archivo_log = config_get_string_value(config,"ARCHIVO_LOG");
	algoritmo_planificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	ip_restaurante = config_get_string_value(config, "IP_RESTAURANTE");
	nombreRestaurante = config_get_string_value(config, "NOMBRE_RESTAURANTE");
	retardo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
}

void terminar_programa(){
	log_destroy(logger);

	config_destroy(config);

	close(socketConexionSindicato);
}
