#include "memoria.h"

void iniciar_memoria(void){
	memoria = malloc(tamanio_memoria);
	if((espacio_swap = fopen("/home/utnso/tp-2020-2c-breakfastClub/espacio_swap.bin","wb+")) == NULL){
		log_info(logger, "Error, no se pudo crear el SWAP");
		return;
	}

	cantidad_de_marcos = 0;
	cantidad_de_marcos_swap = 0;

	for(int offset = 0; offset < tamanio_memoria -1; offset += tamanio_pagina){
		log_info(logger, "Desplazamiento: %d", offset);
		void* marco = memoria + offset;

		entradaTablaMarcos* nuevaEntrada = malloc(sizeof(entradaTablaMarcos));
		nuevaEntrada->indice = cantidad_de_marcos;
		nuevaEntrada->bitModificado = false;
		nuevaEntrada->bitUso = false;
		nuevaEntrada->idPedido = -1;
		nuevaEntrada->libre = true;
		nuevaEntrada->marco = marco;
		nuevaEntrada->restaurante = NULL;
		nuevaEntrada->timeStamp = NULL;

		list_add(tablaDeMarcos, nuevaEntrada);
		log_info(logger, "Marco numero: %d", nuevaEntrada->indice);
		cantidad_de_marcos++;
	}

	int indice = 0;

	for(int offset = 0; offset < tamanio_swap -1; offset += tamanio_pagina){
		void* marco = memoria + offset;

		entradaSwap* nuevaEntrada = malloc(sizeof(entradaSwap));
		nuevaEntrada->idPedido = -1;
		nuevaEntrada->libre = true;
		nuevaEntrada->indiceMarcoSwap = indice;
		indice ++;

		list_add(entradas_swap, nuevaEntrada);
		cantidad_de_marcos_swap++;
	}

	log_info(logger, "Cantidad de Marcos Libres: %d", cantidad_de_marcos);
	log_info(logger, "Cantidad de Marcos en SWAP: %d", cantidad_de_marcos_swap);
}

void* convertir(char* algoritmo_nombre) {
	if (!strcmp(algoritmo_nombre, "LRU")) return (void*) seleccionar_victima_LRU;
	if (!strcmp(algoritmo_nombre, "CLOCK_ME")) return (void*) seleccionar_victima_CLOCK_ME;

	return NULL;
}
/*
bool pagina_esta_en_memoria(t_pagina* pagina){
	t_list* marcosDelPedido = marcos_segun_pedido(pagina->idPedido);
	if (marcosDelPedido == NULL) {
		return false;
	} else {
		int cantidadMarcos = list_size(marcosDelPedido);
		bool encontrado = false;

		for (int i = 0; i < cantidadMarcos; i++) {
			void* marco = list_get(marcosDelPedido, i);
			int offset = 8;
			char* nombreEnMarco = malloc(24);
			memcpy(nombreEnMarco, (marco + offset), 24);

			if (!strcmp(pagina->nombrePlato, nombreEnMarco))
				encontrado = true;

			free(nombreEnMarco);
		}

		return encontrado;
	}
}
*/

void escribir_en_memoria(t_pagina* pagina, bool esta_en_memoria, int idPedido, bool modificado) {
	entradaTablaMarcos* entradaDeLaTabla;

	if (esta_en_memoria) {

		// Busco el marco de ese plato/pagina
		t_list* entradasDelPedido = entradas_segun_pedido(idPedido);

		int cantidadMarcos = list_size(entradasDelPedido);
		bool encont = false;
		entradaTablaMarcos* entrada;

		for (int i = 0; i < cantidadMarcos && !encont; i++) {
			entradaDeLaTabla = list_get(entradasDelPedido, i);
			int offset = 8;
			char* nombreEnMarco = malloc(24);
			memcpy(nombreEnMarco, (entradaDeLaTabla->marco + offset), 24);

			if (!strcmp(pagina->nombrePlato, nombreEnMarco))
				encont = true;
			free(nombreEnMarco);
		}

		if (!strcmp(algoritmo_reemplazo, "LRU")) {
			entradaDeLaTabla->timeStamp = temporal_get_string_time();
		}

		if (!strcmp(algoritmo_reemplazo, "CLOCK_ME")) {
			entradaDeLaTabla->bitUso = true;
			entradaDeLaTabla->bitModificado = modificado;
		}
	} else {
		entradaDeLaTabla = asignar_entrada_marco_libre();
		entradaDeLaTabla->libre = false;
		entradaDeLaTabla->idPedido = idPedido;

		log_info(logger, "Marco seleccionado: %d", entradaDeLaTabla->indice);

		// Agrego a lista de timestamp por marco
		if (!strcmp(algoritmo_reemplazo, "LRU")) {
			entradaDeLaTabla->timeStamp = temporal_get_string_time();
			log_info(logger, "El timestamp del marco numero %d se ha inicializado: %s", entradaDeLaTabla->indice, entradaDeLaTabla->timeStamp);
		}

		// Agrego a lista de timestamp por marco
		if (!strcmp(algoritmo_reemplazo, "CLOCK_ME")) {
			entradaDeLaTabla->bitModificado = modificado;
			entradaDeLaTabla->bitUso = true;
			log_info(logger, "Los bits del marco numero %d se han inicializados: bit de uso: %d - bit de modificado %d .", entradaDeLaTabla->indice, entradaDeLaTabla->bitUso, entradaDeLaTabla->bitModificado);
		}

//		// Agrego el marco a la lista de marcos del pedido
//		t_list* marcos_del_pedido_en_memoria = entradas_segun_pedido(idPedido);
//		list_add(marcos_del_pedido_en_memoria, marco_libre);
//		log_info(logger, "Se agrego el marco a la lista de marcos del pedido");
	}

	int offset = 0;

	// Escribo
	memcpy(entradaDeLaTabla->marco + offset, &(pagina->cantidadLista), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(entradaDeLaTabla->marco + offset, &(pagina->cantidadPlato), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(entradaDeLaTabla->marco + offset, pagina->nombrePlato, 24);

	log_info(logger, "Escrito con exito");
}

entradaTablaMarcos* buscar_entrada(void* marco){
	bool el_que_quiero(void* parametro){
		return ((entradaTablaMarcos*)parametro)->marco == marco;
	}

	return list_find(tablaDeMarcos, el_que_quiero);
}

entradaTablaMarcos* asignar_entrada_marco_libre(void){
	bool este_libre(void* parametro){
		return ((entradaTablaMarcos*)parametro)->libre;
	}

	if (!hay_marcos_libres()) {
		log_info(logger, "No hay marcos libres. Seleccionando victima.");

		pthread_t hilo;

		pthread_mutex_lock(&mutexVictima);
		pthread_create(&hilo, NULL, (void*) seleccionar_victima, NULL);
		pthread_join(hilo, 0);
		pthread_mutex_unlock(&mutexVictima);
	}

	t_list* entradas_marcos_libres = list_filter(tablaDeMarcos, este_libre);

	entradaTablaMarcos* buscado = list_get(entradas_marcos_libres, 0);

	list_destroy(entradas_marcos_libres);

	return buscado;
}

void seleccionar_victima_LRU(void){
	bool mas_viejo(void* unElem, void* otroElem){
		entradaTablaMarcos* elem1 = unElem;
		entradaTablaMarcos* elem2 = otroElem;

		char ** timeElem1;
		char ** timeElem2;

		int horaElem1;
		int horaElem2;
		int minElem1;
		int minElem2;
		int segElem1;
		int segElem2;
		int milElem1;
		int milElem2;

		timeElem1 = string_split(elem1->timeStamp, ":");
		timeElem2 = string_split(elem2->timeStamp, ":");

		horaElem1 = atoi(timeElem1[0]);
		horaElem2 = atoi(timeElem2[0]);

		minElem1 = atoi(timeElem1[1]);
		minElem2 = atoi(timeElem2[1]);

		segElem1 = atoi(timeElem1[2]);
		segElem2 = atoi(timeElem2[2]);

		milElem1 = atoi(timeElem1[3]);
		milElem2 = atoi(timeElem2[3]);

		if (horaElem1 < horaElem2) {
			return horaElem1 < horaElem2;
		} else if (horaElem1 == horaElem2) {
			if (minElem1 < minElem2) {
				return minElem1 < minElem2;
			} else if (minElem1 == minElem2) {
				if (segElem1 < segElem2) {
					return segElem1 < segElem2;
				} else if (segElem1 == segElem2) {
					if (milElem1 <= milElem2) {
						return milElem1 <= milElem2;
					}
				}
			}
		}
	}


	pthread_mutex_lock(&mutexFree);

	list_sort(tablaDeMarcos, mas_viejo);
	entradaTablaMarcos* entrada_mas_vieja = list_get(tablaDeMarcos, 0);

	log_info(logger, "Victima seleccionada: %d", entrada_mas_vieja->indice);

	//todo FALTA VER QUE PASA CON SWAP ACA

	entrada_mas_vieja->libre = true;

	pthread_mutex_unlock(&mutexFree);
}

void seleccionar_victima_CLOCK_ME(void){
	int siguiente_posicion(int* posicion_actual){
		return (*posicion_actual + 1) % cantidad_de_marcos;
	}

	if(punteroMarcoClock == NULL){
		// Apunta a la primer entrada de la Tabla de Marcos
		punteroMarcoClock = list_get(tablaDeMarcos, 0);
	}

	int posicion_puntero_actual = indice_elemento(tablaDeMarcos, punteroMarcoClock);

	bool victima_seleccionada = false;

	while (!victima_seleccionada) {
		if (!victima_seleccionada) {
			for (int i = 0; i < cantidad_de_marcos && !victima_seleccionada; i++) {
				if (ambos_bits_apagados(punteroMarcoClock)) victima_seleccionada = true;
				if(!victima_seleccionada){
					// referencio a la siguiente entrada
					punteroMarcoClock = list_get(tablaDeMarcos, ((posicion_puntero_actual + 1) % cantidad_de_marcos));

					// aumento el indice del puntero
					posicion_puntero_actual = ((posicion_puntero_actual + 1) % cantidad_de_marcos);
				}
			}
		}

		if (!victima_seleccionada) {
			for (int i = 0; i < cantidad_de_marcos && !victima_seleccionada; i++) {
				if (!bit_uso(punteroMarcoClock) && bit_modificado(punteroMarcoClock)) {
					victima_seleccionada = true;
				} else {
					punteroMarcoClock->bitUso = false;
				}
				if(!victima_seleccionada){
					// referencio a la siguiente entrada
					punteroMarcoClock = list_get(tablaDeMarcos, ((posicion_puntero_actual + 1) % cantidad_de_marcos));

					// aumento el indice del puntero
					posicion_puntero_actual = ((posicion_puntero_actual + 1) % cantidad_de_marcos);
				}
			}
		}
	}


	pthread_mutex_lock(&mutexFree);

	log_info(logger, "Victima seleccionada: %d", punteroMarcoClock->indice);

	// todo Escribo en SWAP

	((entradaTablaMarcos*)list_get(tablaDeMarcos, posicion_puntero_actual))->libre = true;

	pthread_mutex_unlock(&mutexFree);
}

bool ambos_bits_apagados(entradaTablaMarcos* entrada){
	return (!entrada->bitModificado) && (!entrada->bitUso);
}

bool bit_uso(entradaTablaMarcos* entrada){
	return entrada->bitUso;
}

bool bit_modificado(entradaTablaMarcos* entrada){
	return entrada->bitModificado;
}

int indice_elemento(t_list* lista, void* elemento){
	int indice = 0;
	bool encontrado = false;

	for (int i = 0; i < list_size(lista) && !encontrado; i++) {
		if(list_get(lista, i) == elemento) encontrado = true;
		indice = i;
	}

	return indice;
}

bool hay_marcos_libres(void){
	bool este_libre(void* parametro){
		return ((entradaTablaMarcos*)parametro)->libre;
	}

	t_list* marcos_libres = list_filter(tablaDeMarcos, este_libre);

	int tamanio = list_size(marcos_libres);

	list_destroy(marcos_libres);

	return tamanio > 0;
}
