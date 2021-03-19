#include "App.h"
#include "planificacion_app.h"
#include "utils_app.h"

int main(void) {
	inicializar_modulo();

	// Alto Test Unitario
	// t_repartidor* prueba = repartidor_libre_mas_cercano(9,8);
	// log_info(logger, "El repartidor mas cercano a [9:8] esta en la posicion [%d,%d]",prueba->posicion.posX, prueba->posicion.posY);



	pthread_create(&planificar_largo, NULL, (void*) planificar_largo_plazo, NULL);
	pthread_detach(planificar_largo);

	for (int i = 0; i < grado_multiprocesamiento; i++) {
		pthread_t another_thread;
		pthread_create(&another_thread, NULL, (void*) planificar_corto_plazo, NULL);
		pthread_detach(another_thread);
	}

	/* test HRRN
	 for (int i = 0; i < 3; i++) {
	 posicion rest1;
	 rest1.posX = 6 + i - 1;
	 rest1.posY = 8 + 1 - 1;

	 posicion cliente;
	 cliente.posX = 1 + i;
	 cliente.posY = 0 + i * 2;

	 pthread_mutex_lock(&mutex_idPcb);
	 crear_pcb(i + 7, rest1, cliente);
	 pthread_mutex_unlock(&mutex_idPcb);
	 sleep(5);
	 }

	 */


	pthread_create(&hiloReceive, NULL, (void*) iniciar_servidor, NULL);
	pthread_join(hiloReceive, NULL);



	return EXIT_SUCCESS;
}

void inicializar_modulo(void) {
	void handshake_comanda(void) {
		socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
		if (socketConexionComanda == -1) {
			log_info(logger, "La Comanda no esta conectada");
		} else if (socketConexionComanda > 0) {
			log_info(logger, "La Comanda esta conectada");
			enviar_cerrar_conexion(socketConexionComanda);
			close(socketConexionComanda);
		}
	}

	idPedidoDefault = 1;
	idRestaurantes = 1;
	offsetCodRestaurante = 10000;
	logger = iniciar_logger();	//Carga el logger
	leer_config();	//Carga las config
	posicionDefault.posX = posicion_rest_default_x;
	posicionDefault.posY = posicion_rest_default_y;
	id_pcb = 0;

	restaurantesPorCliente = list_create();

	restaurantes_conectados = list_create();

	// Inicialización de la Planificación
	lista_pcbs = list_create();
	lista_bloqueados = list_create();
	cola_new = queue_create();
	lista_ready = list_create();

	// Inicializar la lista de repartidores
	lista_repartidores = list_create();
	inicializar_repartidores();

	grado_multiprogramacion = list_size(lista_repartidores);

	// Semaforos
	sem_init(&semaforo_planificacion, 0, grado_multiprogramacion);
	sem_init(&semaforo_nuevo_pcb, 0, 0);
	sem_init(&semaforo_ready_pcb, 0, 0);
	sem_init(&semaforo_repartidor_libre, 0, lista_repartidores->elements_count);
	pthread_mutex_init(&mutex_idPcb, NULL);
	pthread_mutex_init(&mutex_block, NULL);
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_cola_ready, NULL);
	pthread_mutex_init(&idPedidoG, NULL);
	pthread_mutex_init(&repartidorCercano, NULL);
	pthread_mutex_init(&restConectados, NULL);
	pthread_mutex_init(&vinculos, NULL);
	pthread_mutex_init(&mutexAniadirPlato, NULL);
	pthread_mutex_init(&mutexConfirmarPedido, NULL);
	pthread_mutex_init(&conexionRestaurantes, NULL);
	pthread_mutex_init(&mutexClienteActual, NULL);

	// Handshake
	handshake_comanda();

	// Leer Algoritmo
	if ((planificar_corto_plazo = convertir(algoritmo_planificacion)) == NULL) {
		log_info(logger, "Error al leer el algoritmo de Planificacion");
	} else {log_info(logger, "El algoritmo seleccionado es: %s", algoritmo_planificacion);}
}

t_log* iniciar_logger(void) {
	return log_create("/home/utnso/tp-2020-2c-breakfastClub/App/archivo_log", "App.c", 1, LOG_LEVEL_INFO);
}

void inicializar_repartidores() {
	for (int i = 0; i < contar_elementos_array(repartidores); i++) {
		agregar_repartidor_a_lista(repartidores[i], frecuencia_de_descanso[i],	tiempo_de_descanso[i]);
	}
}

void agregar_repartidor_a_lista(char* posiciones, char* frecuencia_descanso, char* tiempo_descanso) {
	t_repartidor* repartidor = malloc(sizeof(t_repartidor));
	char** posicion = string_split(posiciones, "|");
	repartidor->posicion.posX = atoi(posicion[0]);
	repartidor->posicion.posY = atoi(posicion[1]);
	repartidor->frecuencia_descanso = atoi(frecuencia_descanso);
	repartidor->tiempo_descanso = atoi(tiempo_descanso);
	repartidor->ocupado = false;
	repartidor->pasosDados = 0;
	pthread_mutex_lock(&repartidorCercano);
	list_add(lista_repartidores, repartidor);
	pthread_mutex_unlock(&repartidorCercano);

	log_info(logger, "El repartidor esta en la posicion [%d,%d]", repartidor->posicion.posX, repartidor->posicion.posY);
}

t_repartidor* repartidor_libre_mas_cercano(int x, int y) {
	int calcular_distancia(t_repartidor* repartidor, int x, int y) {
		return abs(repartidor->posicion.posX - x) + abs(repartidor->posicion.posY - y);
	}

	bool esta_mas_cerca(t_repartidor* repartidor1, t_repartidor* repartidor2, int x, int y) {
		return calcular_distancia(repartidor1, x, y) <= calcular_distancia(repartidor2, x, y);
	}

	t_list* lista = repartidores_libres();
	t_repartidor* repartidor = list_get(lista, 0);
	t_repartidor* aux;

	for (int i = 1; i < list_size(lista); i++) {
		aux = list_get(lista, i);
		if (esta_mas_cerca(aux, repartidor, x, y))
			repartidor = aux;
	}

	list_destroy(lista);
	return repartidor;
}

void leer_config(void) {
	config = config_create("/home/utnso/tp-2020-2c-breakfastClub/App/app.config");

	ip_comanda = config_get_string_value(config, "IP_COMANDA");
	puerto_comanda = config_get_string_value(config, "PUERTO_COMANDA");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	retardo_ciclo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
	grado_multiprocesamiento = config_get_int_value(config,"GRADO_DE_MULTIPROCESAMIENTO");
	algoritmo_planificacion = config_get_string_value(config,"ALGORITMO_DE_PLANIFICACION");
	alpha = config_get_double_value(config, "ALPHA");
	estimacion_inicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	repartidores = config_get_array_value(config, "REPARTIDORES");
	frecuencia_de_descanso = config_get_array_value(config,"FRECUENCIA_DE_DESCANSO");
	tiempo_de_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	archivo_log = config_get_string_value(config, "ARCHIVO_LOG");
	platos_default = config_get_array_value(config, "PLATOS_DEFAULT");
	ip_app = config_get_string_value(config, "IP_APP");
	posicion_rest_default_x = config_get_int_value(config,"POSICION_REST_DEFAULT_X");
	posicion_rest_default_y = config_get_int_value(config,"POSICION_REST_DEFAULT_Y");
}

void terminar_programa() {
	log_destroy(logger);
	config_destroy(config);
}
