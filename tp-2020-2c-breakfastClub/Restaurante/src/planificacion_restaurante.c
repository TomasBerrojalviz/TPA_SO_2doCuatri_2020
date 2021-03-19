#include "planificacion_restaurante.h"

void crear_pcbs_pedido(obtener_pedido_respuesta* pedido, uint32_t id_pedido, uint32_t socketSolicitante) {

	log_info(logger, "RESTAURANTE :: EMPIEZO A CREAR LOS PCBS DEL PEDIDO %d", id_pedido);

	int cantPlatos;

	for (int i = 0; i < list_size(pedido->listaPlatos); i++) {
		log_info(logger, "RESTAURANTE :: ENTRAMOS AL FOR QUE RECORRE PLATOS DEL PEDIDO");
		cantPlatos = (uint32_t) list_get(pedido->listaCantidadDePedido, i) - (uint32_t) list_get(pedido->listaCantidadLista, i);
		log_info(logger, "RESTAURANTE :: Cant: %d - Listos: %d => %d", (uint32_t) list_get(pedido->listaCantidadDePedido, i),(uint32_t) list_get(pedido->listaCantidadLista, i), cantPlatos);
		for (int k = 0; k < cantPlatos; k++) {
			log_info(logger, "RESTAURANTE :: ENTRAMOS AL FOR QUE RECORRE PLATOS SI ESTAN LISTOS");
			t_pcb_plato* nuevo_pcb = malloc(sizeof(t_pcb_plato));
			nuevo_pcb->estado = NEW;
			nuevo_pcb->id_pedido = id_pedido;
			pthread_mutex_lock(&mutex_idPcb);
			nuevo_pcb->id_pcb = id_pcb++;
			pthread_mutex_unlock(&mutex_idPcb);
			nuevo_pcb->plato = string_duplicate((char*) list_get(pedido->listaPlatos, i));
			nuevo_pcb->pasosARealizar = list_create();
			nuevo_pcb->socketSolicitante = socketSolicitante;
			nuevo_pcb->finDeQuantum = false;
			uint32_t socketSindicato = conectar_sindicato();
			enviar_sindicato_obtener_receta(nuevo_pcb->plato, socketSindicato);

			uint32_t buffer_size;

			recv(socketSindicato, &buffer_size, sizeof(uint32_t), MSG_WAITALL);
			recv(socketSindicato, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

			log_info(logger, "RESTAURANTE :: SINDICATO YA MANDO LA RECETA");

			t_receta_respuesta* recetaObtenida = deserializar_obtener_receta_respuesta(socketSindicato);
			log_info(logger, "RESTAURANTE :: SE OBTUVO LA RECETA");
			for (int j = 0; j < list_size(recetaObtenida->listaPasos); j++) {
				pasoReceta* paso_receta = malloc(sizeof(pasoReceta));
				paso_receta->nombrePaso = string_duplicate(	(char*) list_get(recetaObtenida->listaPasos,j));
				paso_receta->tiempoRestante = malloc(sizeof(uint32_t));
				*paso_receta->tiempoRestante = list_get(recetaObtenida->listaTiempoPasos, j);
				list_add(nuevo_pcb->pasosARealizar, paso_receta);
			}


//			else {
//				log_info(logger, "RESTAURANTE :: NO SE OBTUVO LA RECETA");
//				return;
//			}

			list_destroy(recetaObtenida->listaPasos);
			list_destroy(recetaObtenida->listaTamanioPasos);
			list_destroy(recetaObtenida->listaTiempoPasos);
			free(recetaObtenida);

			log_info(logger, "SE CREO EL PCB (id: %d) EN READY, PARA EL PLATO: %s",	nuevo_pcb->id_pcb, nuevo_pcb->plato);
			list_add(lista_pcbs_platos, nuevo_pcb);
			pthread_mutex_lock(&mutex_new);
			queue_push(cola_new, nuevo_pcb);
			pthread_mutex_unlock(&mutex_new);
			sem_post(&semaforo_nuevo_pcb);
		}
	}
}

void crear_colas_planificacion(){
	int cantidad_veces_afinidades_repetidas(char* afinidad) {
		int contador = 0;
		for(int j = 0; j< list_size(afinidades); j++) {
			if(strcmp(afinidad, (char *) list_get(afinidades, j)) == 0) {
				contador++;
			}
		}
		return contador;
	}
	bool sin_repetidos(void* argument) {
		return cantidad_veces_afinidades_repetidas((char*) argument) == 1;
	}
	/*------- CREACION COLAS READY------*/
//	t_list* afinidades_sin_repetidos = list_filter(afinidades, sin_repetidos);
//	int cantidadDeColasDeAfinidades = list_size(afinidades_sin_repetidos);
	int cantidadDeColasDeAfinidades = list_size(afinidades);
	listaDeColasReady = list_create();
	log_info(logger, "SE CREAN %d COLAS DE AFINIDADES", cantidadDeColasDeAfinidades);
	for(int i = 0; i < cantidadDeColasDeAfinidades; i ++) { // Para crear los hilos de planificacion quizas agregar campo en colaReady con la cant de cocineros para un semaforo contador
		colaReady* colaDeReady = malloc(sizeof(colaReady));
		colaDeReady->cola = queue_create();
//		colaDeReady->nombrePlato = malloc(strlen((char*) list_get(afinidades_sin_repetidos,i) + 1));
//		strcpy(colaDeReady->nombrePlato, (char*) list_get(afinidades_sin_repetidos, i));
//		colaDeReady->nombrePlato = malloc(strlen((char*) list_get(afinidades,i))+ 1); // TODO descomentar?
		colaDeReady->nombrePlato = string_duplicate((char*) list_get(afinidades, i));
//		sem_init(&colaDeReady->cocinerosDisponibles, 0, cantidad_veces_afinidades_repetidas((char*) list_get(afinidades_sin_repetidos, i)));
		sem_init(&colaDeReady->cocinerosDisponibles, 0, cantidad_veces_afinidades_repetidas((char*) list_get(afinidades, i)));
		sem_init(&colaDeReady->platosEncolados, 0, 0);
		pthread_mutex_init(&colaDeReady->mutex_ready, NULL);
		list_add(listaDeColasReady, colaDeReady);
		pthread_t planificar_corto;
		pthread_create(&planificar_corto, NULL, (void*) planificacion_corto_plazo, colaDeReady);
		pthread_detach(planificar_corto);
	}
	log_info(logger, "SE CREARON LAS COSAS DE AFINIDADES");

	int cantidadCocinerosDefault;
	cantidadCocinerosDefault = cantidadDeCocineros - list_size(afinidades);
	colaReady* colaReadyDefault = malloc(sizeof(colaReady));
	colaReadyDefault->cola = queue_create();
	colaReadyDefault->nombrePlato = malloc(strlen("PlatosDefault") + 1);
	strcpy(colaReadyDefault->nombrePlato, "PlatosDefault");
	sem_init(&colaReadyDefault->cocinerosDisponibles, 0, cantidadCocinerosDefault);
	sem_init(&colaReadyDefault->platosEncolados, 0, 0);
	pthread_mutex_init(&colaReadyDefault->mutex_ready, NULL);
	list_add(listaDeColasReady, colaReadyDefault);

	pthread_t planificar_corto_default;
//	pthread_create(&planificar_corto_default, NULL, (void*) planificacion_corto_plazo, colaReadyDefault);
	pthread_create(&planificar_corto_default, NULL, (void*) planificacion_corto_plazo, colaReadyDefault);
	pthread_detach(planificar_corto_default);

	/*------- CREACION COLAS ENTRADA/SALIDA (HORNOS)------*/

	for(int j = 0; j < cantidadDeHornos; j++) {
		pthread_t planificador_hornos;
		pthread_create(&planificador_hornos, NULL, (void*) manejar_horno, NULL);
	}
}

// LARGO PLAZO
void planificar_largo_plazo(void) {

	while(1){
		sem_wait(&semaforo_nuevo_pcb);
		pthread_mutex_lock(&mutex_new);
		t_pcb_plato* pcb = queue_pop(cola_new);
		pthread_mutex_unlock(&mutex_new);
		asignar_cola_segun_plato(pcb);
	}
}

void* planificacion_corto_plazo(colaReady* colaDeReady) {

	while (1) {
		sem_wait(&colaDeReady->platosEncolados);
		sem_wait(&colaDeReady->cocinerosDisponibles);
		pthread_t cocinar;
		pthread_create(&cocinar, NULL, (void*) empezar_a_cocinar, colaDeReady);
		pthread_detach(cocinar);
	}
}

//void* planificacion_segun_RR(colaReady* colaDeReady) {
//	return NULL;
//}

bool quedan_pasos(t_pcb_plato* pcb) {
	return list_size(pcb->pasosARealizar) > 0;
}

void asignar_cola_segun_plato(t_pcb_plato* pcb) {
	bool colaReadyConElMismoPlatoA(void* colaReadyDeLista) {
		return strcmp(((colaReady*)colaReadyDeLista)->nombrePlato, (char*) pcb->plato) == 0;
	}

	bool esColaDefault(void* colaReadyDeLista) {
		//colaReady* colaReadyDeLista = (colaReady*) parametro;
		return strcmp(((colaReady*)colaReadyDeLista)->nombrePlato, "PlatosDefault") == 0;
	}

	colaReady* colarReadyAAsignar = list_find(listaDeColasReady, colaReadyConElMismoPlatoA);
	if(colarReadyAAsignar == NULL) {
		colarReadyAAsignar = list_find(listaDeColasReady, esColaDefault);
	}
	pcb->estado = READY;
	pcb->finDeQuantum = false;

	pthread_mutex_lock(&mutex_cola_ready);
	queue_push(colarReadyAAsignar->cola, pcb);
	pthread_mutex_unlock(&mutex_cola_ready);

	log_info(logger, "PCB (id: %d) en READY", pcb->id_pcb);

	sem_post(&colarReadyAAsignar->platosEncolados);
}



void manejar_horno() {
	pthread_mutex_t mutex_entrada_salida;
	pthread_mutex_init(&mutex_entrada_salida, NULL);
	while(1) {
		sem_wait(&semaforo_hay_plato_para_el_horno);
		pthread_mutex_lock(&mutex_entrada_salida);
		pthread_mutex_lock(&mutex_cola_hornos);
		t_pcb_plato* pcb = (t_pcb_plato*) queue_pop(cola_hornos);
		pthread_mutex_unlock(&mutex_cola_hornos);
		hacer_ciclos(pcb);
		pthread_mutex_unlock(&mutex_entrada_salida);
		elegir_destino(pcb);
	}
}

void elegir_destino(t_pcb_plato* pcb) {
	if (quedan_pasos(pcb)) {
		asignar_cola_segun_plato(pcb);
	} else {
		finalizar_plato(pcb);
	}
}

bool necesitaReposar(char* paso) {
	return strcmp(paso, "Reposar") == 0;
}

bool necesitaHornear(char* paso) {
		return strcmp(paso, "Hornear") == 0;
	}

bool necesitaPreparar(char* paso) {
	return !necesitaReposar(paso) && !necesitaHornear(paso);
}

void empezar_a_cocinar(colaReady* colaDeReady) {

	pthread_mutex_lock(&colaDeReady->mutex_ready);
	t_pcb_plato* pcb = (t_pcb_plato*) queue_pop(colaDeReady->cola);
	pthread_mutex_unlock(&colaDeReady->mutex_ready);

	log_info(logger, "EJECUTANDO PCB: %d", pcb->id_pcb);
	pcb->estado = EXEC;

	while(quedan_pasos(pcb) && necesitaPreparar((char*) ((pasoReceta*) list_get(pcb->pasosARealizar,0))->nombrePaso) && !pcb->finDeQuantum) {
		hacer_ciclos(pcb);
	}

	sem_post(&colaDeReady->cocinerosDisponibles);

	if(!quedan_pasos(pcb)) {
		finalizar_plato(pcb);
	} else {
		if(necesitaReposar((char*) ((pasoReceta*) list_get(pcb->pasosARealizar,0))->nombrePaso)) {
		reposar(pcb);
		return;
		} else {
			if (necesitaHornear((char*) ((pasoReceta*) list_get(pcb->pasosARealizar, 0))->nombrePaso)) {
					pthread_mutex_lock(&mutex_cola_hornos);
					queue_push(cola_hornos, pcb);
					pthread_mutex_unlock(&mutex_cola_hornos);
					sem_post(&semaforo_hay_plato_para_el_horno);
			}
		}
	}
}

void hacer_ciclos(t_pcb_plato* pcb) {
	pasoReceta* paso = (pasoReceta*) list_get(pcb->pasosARealizar, 0);
	if (!strcmp(algoritmo_planificacion, "FIFO") || !necesitaPreparar(paso->nombrePaso) || (*paso->tiempoRestante <= quantum)) {
		list_remove(pcb->pasosARealizar, 0);
		log_info(logger, "El PLATO: %s DEL PCB: %d DEL PEDIDO: %d COMIENZA A %s", pcb->plato,pcb->id_pcb, pcb->id_pedido, paso->nombrePaso);
		sleep((uint32_t) (*paso->tiempoRestante) * retardo_cpu);
		log_info(logger, "El PLATO: %s DEL PCB: %d DEL PEDIDO: %d TERMINO DE %s", pcb->plato,pcb->id_pcb, pcb->id_pedido, paso->nombrePaso);

		free(paso->nombrePaso);
		free(paso->tiempoRestante);
		free(paso);
	} else if (*paso->tiempoRestante > quantum) {
		log_info(logger, "El PLATO: %s DEL PCB: %d DEL PEDIDO: %d COMIENZA A %s", pcb->plato,pcb->id_pcb, pcb->id_pedido, paso->nombrePaso);
		sleep(quantum * retardo_cpu);
		log_info(logger, "El PLATO: %s DEL PCB: %d DEL PEDIDO: %d TERMINO SU QUANTUM DE %s", pcb->plato,pcb->id_pcb, pcb->id_pedido, paso->nombrePaso);
		*paso->tiempoRestante -= quantum;
		pcb->finDeQuantum = true;

		pthread_t asignar_cola;
		pthread_create(&asignar_cola, NULL, (void*) asignar_cola_segun_plato, pcb);
		pthread_detach(asignar_cola);
	}
}

void reposar(t_pcb_plato* pcb) {
	pcb->estado = BLOCKED;
	hacer_ciclos(pcb);
	elegir_destino(pcb);
}

void finalizar_plato(t_pcb_plato* pcb) {
	uint32_t pedido = pcb->id_pedido;
	bool pedidoNoFinalizado(t_pcb_plato* pcbRecibido){
		return pcbRecibido->id_pedido == pedido;
	}
	bool esElPCB(t_pcb_plato* pcbRecibido){
		return pcbRecibido->id_pcb == pcb->id_pcb;
	}

	pcb->estado = EXIT;
	log_info(logger,"EL PLATO %s DEL PCB %d Y PEDIDO %d FUE FINALIZADO", pcb->plato, pcb->id_pcb, pcb->id_pedido);

	uint32_t socketSindicato = conectar_sindicato();
	uint32_t socketSolicitante = pcb->socketSolicitante;

	plato_listo* platoListo = malloc(sizeof(plato_listo));
	platoListo->id_pedido = pcb->id_pedido;
	platoListo->tamanioNombre = strlen(nombreRestaurante) + 1;
	platoListo->nombreRestaurante = string_duplicate(nombreRestaurante);
	platoListo->tamanioNombrePlato = strlen(pcb->plato) + 1;
	platoListo->nombrePlato = string_duplicate(pcb->plato);

	enviar_sindicato_plato_listo(platoListo, socketSindicato);


	log_info(logger, "RESTAURANTE :: ENVIO MENSAJE: PLATO_LISTO");
	uint32_t buffer_size2;
	t_respuesta2 respuesta;
	recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);
	recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

	recv(socketSindicato, &respuesta, sizeof(uint32_t), 0);

	if(respuesta == OK2){
		log_info(logger,"RESTAURANTE :: PLATO LISTO AGREGADO.");
		enviar_sindicato_plato_listo(platoListo, socketConApp);
	}
	else{
		log_info(logger,"RESTAURANTE :: PLATO LISTO NO AGREGADO.");
	}

	free(platoListo->nombrePlato);
	free(platoListo->nombreRestaurante);
	free(platoListo);

	list_remove_and_destroy_by_condition(lista_pcbs_platos, esElPCB, liberar_pcb);

	if(!list_any_satisfy(lista_pcbs_platos, pedidoNoFinalizado))
		finalizar_pedido(pedido,socketSolicitante);
}
void finalizar_pedido(uint32_t id_pedido, uint32_t socketSolicitante){
	log_info(logger,"EL PEDIDO %d FUE FINALIZADO", id_pedido);

	uint32_t socketSindicato = conectar_sindicato();

	terminar_pedido* terminarPedido = malloc(sizeof(terminar_pedido));
	terminarPedido->id_pedido = id_pedido;
	terminarPedido->tamanioNombre = strlen(nombreRestaurante) + 1;
	terminarPedido->nombreRestaurante = string_duplicate(nombreRestaurante);

	enviar_sindicato_terminar_pedido(terminarPedido, socketSindicato);

	log_info(logger, "RESTAURANTE :: ENVIO MENSAJE: TERMINAR_PEDIDO");
	uint32_t buffer_size2;
	t_respuesta2 respuesta;
	recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);
	recv(socketSindicato, &buffer_size2, sizeof(uint32_t), MSG_WAITALL);

	recv(socketSindicato, &respuesta, sizeof(uint32_t), 0);

	if(respuesta == OK2){
		log_info(logger,"RESTAURANTE :: EL PEDIDO SE TERMINO CORRECTAMENTE.");
		//TODO terminar pedido ok
	}
	else{
		log_info(logger,"RESTAURANTE :: EL PEDIDO NO SE TERMINO CORRECTAMENTE.");
		//TODO terminar pedido fail
	}

	if(socketSolicitante != socketConApp)
		enviar_finalizar_pedido(id_pedido, socketSolicitante);
}

//void* convertir(char* algoritmo_nombre) {
//	if (!strcmp(algoritmo_nombre, "FIFO")) return (void*) planificacion_segun_FIFO;
//	if (!strcmp(algoritmo_nombre, "RR")) return (void*) planificacion_segun_RR;
//
//	return NULL;
//}

void liberar_pcb(t_pcb_plato* pcb){
	list_destroy(pcb->pasosARealizar);
	free(pcb->plato);
	free(pcb);
}



