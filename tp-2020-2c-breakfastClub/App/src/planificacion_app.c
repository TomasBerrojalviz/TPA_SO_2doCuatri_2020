#include "planificacion_app.h"
#include "utils_app.h"

void crear_pcb(int id_pedido, char* nombreRestaurante, uint32_t cliente){
	t_pcb* nuevo_pcb = malloc(sizeof(t_pcb));
	nuevo_pcb->repartidor = malloc(sizeof(t_repartidor));
	nuevo_pcb->estado = NEW;
	nuevo_pcb->id_pedido = id_pedido;
	nuevo_pcb->id_pcb = id_pcb++;
	nuevo_pcb->posicion_restaurante = obtener_posicion_restaurante(nombreRestaurante);
	nuevo_pcb->nombreRestaurante = nombreRestaurante;
	nuevo_pcb->socketCliente = cliente;
	nuevo_pcb->posicion_cliente = clienteActual->posCliente;
	nuevo_pcb->repartidor = NULL;
	nuevo_pcb->recogidoDelRestaurante = false;
	nuevo_pcb->tiempo_espera = 0;
	nuevo_pcb->estimacion = estimacion_inicial;
	nuevo_pcb->estaEsperando = false;

	list_add(lista_pcbs, nuevo_pcb);

	// agregar semaforos mutex
	pthread_mutex_lock(&mutex_new);
	queue_push(cola_new, nuevo_pcb);
	pthread_mutex_unlock(&mutex_new);
	sem_post(&semaforo_nuevo_pcb);

	log_info(logger, "PCB (id: %d) CREADO", nuevo_pcb->id_pcb);
}

// LARGO PLAZO
void planificar_largo_plazo(void) {
	while(1){
		sem_wait(&semaforo_nuevo_pcb);
		sem_wait(&semaforo_repartidor_libre);
		asignar_repartidor_libre();
		sem_post(&semaforo_ready_pcb);
	}
}

void asignar_repartidor_libre() {
	pthread_mutex_lock(&mutex_new);
	t_pcb* pcb = queue_pop(cola_new);
	pthread_mutex_unlock(&mutex_new);
	pthread_mutex_lock(&repartidorCercano);
	pcb->repartidor = repartidor_libre_mas_cercano(pcb->posicion_restaurante.posX, pcb->posicion_restaurante.posY);
	pcb->repartidor->ocupado = true;
	pcb->repartidor->id_pedido = pcb->id_pedido;
	pthread_mutex_unlock(&repartidorCercano);
	pcb->estado = READY;

	pthread_mutex_lock(&mutex_cola_ready);
	list_add(lista_ready, pcb);
	pthread_mutex_unlock(&mutex_cola_ready);

	log_info(logger, "PCB (id: %d) en READY", pcb->id_pcb);
}

// CORTO PLAZO

void doNothing(void* nada){}

bool llego_a_destino(t_repartidor* repartidor, posicion posicion) {
	return repartidor->posicion.posX == posicion.posX && repartidor->posicion.posY == posicion.posY;
}

bool quedan_movimientos(uint32_t posicion1, uint32_t posicion2) {
	return posicion1 != posicion2;
}

void avanzar_hacia(t_repartidor* repartidor, posicion posicionDestino) {
	if (quedan_movimientos(repartidor->posicion.posX, posicionDestino.posX)) {
		if (repartidor->posicion.posX < posicionDestino.posX) {
			repartidor->posicion.posX++;
		} else {
			repartidor->posicion.posX--;
		}
	} else {

		if (quedan_movimientos(repartidor->posicion.posY, posicionDestino.posY)) {
			if (repartidor->posicion.posY < posicionDestino.posY) {
				repartidor->posicion.posY++;
			} else {
				repartidor->posicion.posY--;
			}
		}
	}
	log_info(logger, "El repartidor del pedido %d esta en la posicion %d,%d yendo a %d,%d",repartidor->id_pedido,repartidor->posicion.posX,repartidor->posicion.posY, posicionDestino.posX, posicionDestino.posY);
}

void bloquear_pcb(t_pcb* pcb) {
	pcb->estado = BLOCKED;
	pthread_mutex_lock(&mutex_block);
	list_add(lista_bloqueados, pcb);
	pthread_mutex_unlock(&mutex_block);
}

void bloquear_haciendo(void(*accion)(t_pcb*), t_pcb* pcb) {
	bloquear_pcb(pcb);
	pthread_t hilo_para_hacer_cositas_magicas;
	pthread_create(&hilo_para_hacer_cositas_magicas, NULL, (void*) accion,(t_pcb*) pcb);
	pthread_detach(hilo_para_hacer_cositas_magicas);
}

void descansar(t_pcb* pcb) {
	bool es_el_pcb_que_quiero(void* parametro) {
		t_pcb* pcbDeLista = (t_pcb *) parametro;
		return pcb->id_pcb == pcbDeLista->id_pcb;
	}
	sleep(pcb->repartidor->tiempo_descanso * retardo_ciclo_cpu);
	pcb->repartidor->pasosDados = 0;

	pthread_mutex_lock(&mutex_block);
	list_remove_by_condition(lista_bloqueados, es_el_pcb_que_quiero);
	pthread_mutex_unlock(&mutex_block);

	pcb->estado = READY;
	log_info(logger, "El repartidor del pedido %d ya descanso", pcb->id_pedido);
	pthread_mutex_lock(&mutex_cola_ready);
	list_add(lista_ready, pcb);
	pthread_mutex_unlock(&mutex_cola_ready);
	sem_post(&semaforo_ready_pcb);
}

bool platos_terminados(t_pcb* pcb) {
	if (!strcmp(pcb->nombreRestaurante, "Default")) {
		return true;
	} else {
		socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
		if (socketConexionComanda == -1) {
			log_info(logger, "No me pude conectar a la Comanda");
			return false;
		} else if (socketConexionComanda > 0) {
			log_info(logger, "Me conecte exitosamente a la Comanda");
			obtener_pedido* msg = malloc(sizeof(obtener_pedido));
			msg->id_pedido = pcb->id_pedido;
			msg->tamanioNombre = strlen(pcb->nombreRestaurante) + 1;
			msg->nombreRestaurante = pcb->nombreRestaurante;

			bool respuesta = respuesta_obtener_pedido(msg, socketConexionComanda);

			close(socketConexionComanda);

			return respuesta;
		}
	}
}

void planificacion_bloqueados(void){
	while(1){

	}
}

void* convertir(char* algoritmo_nombre) {
	if (!strcmp(algoritmo_nombre, "FIFO")) return (void*) planificacion_segun_FIFO;
	if (!strcmp(algoritmo_nombre, "HRRN")) return (void*) planificacion_segun_HRRN;
	if (!strcmp(algoritmo_nombre, "SJF_SD")) return (void*) planificacion_segun_SJF_SD;

	return NULL;
}

void planificacion_segun_FIFO() {

	pthread_mutex_t mutex_exec;
	pthread_mutex_init(&mutex_exec, NULL);

	while (1) {
		sem_wait(&semaforo_ready_pcb);
		pthread_mutex_lock(&mutex_cola_ready);
		t_pcb* pcb = list_remove(lista_ready, 0);
		pthread_mutex_unlock(&mutex_cola_ready);

		pthread_mutex_lock(&mutex_exec);
		//avanzar_fifo(pcb);
		avanzar_generico(pcb, doNothing, doNothing);
		pthread_mutex_unlock(&mutex_exec);
	}
}

float response_ratio(t_pcb* pcb){
	float x = pcb->tiempo_espera;
	float y = pcb->estimacion;

	return (x + y) / y;
}

void planificacion_segun_HRRN() {
	bool hay_pcbs_en_ready(void) {
		return !list_is_empty(lista_ready);
	}

	bool mas_viejo(void* unElem, void* otroElem){
		return response_ratio(unElem) > response_ratio(otroElem);
	}

	pthread_mutex_t mutex_exec;
	pthread_mutex_init(&mutex_exec, NULL);

	while (1) {
		sem_wait(&semaforo_ready_pcb);
		pthread_mutex_lock(&mutex_cola_ready);
		list_sort(lista_ready, mas_viejo);
		t_pcb* pcb = list_remove(lista_ready, 0);
		log_info(logger, "El RR del PCB %d es %f", pcb->id_pcb, response_ratio(pcb));
		log_info(logger, "%d", pcb->repartidor->frecuencia_descanso);
		log_info(logger, "%d", pcb->tiempo_espera);
		pcb->tiempo_espera = 0;
		pthread_mutex_unlock(&mutex_cola_ready);

		pthread_mutex_lock(&mutex_exec);
		//avanzar_hrrn(pcb);
		avanzar_generico(pcb, avejentar_pcbs, reestimar);
		pthread_mutex_unlock(&mutex_exec);
	}
}

void planificacion_segun_SJF_SD() {
	bool hay_pcbs_en_ready(void) {
		return !list_is_empty(lista_ready);
	}

	bool mas_corto(void* unElem, void* otroElem){
		return ((t_pcb*)unElem)->estimacion < ((t_pcb*)otroElem)->estimacion;
	}

	pthread_mutex_t mutex_exec;
	pthread_mutex_init(&mutex_exec, NULL);

	while (1) {
		sem_wait(&semaforo_ready_pcb);
		pthread_mutex_lock(&mutex_cola_ready);
		list_sort(lista_ready, mas_corto);
		t_pcb* pcb = list_remove(lista_ready, 0);
		pthread_mutex_unlock(&mutex_cola_ready);

		printf("\n");
		printf("ESTIMACION DEL PCB %d ELEGIDO: %f", pcb->id_pcb, pcb->estimacion);
		printf("\n");

		pthread_mutex_lock(&mutex_exec);
		//avanzar_sjf(pcb);
		avanzar_generico(pcb, doNothing, reestimar);
		pthread_mutex_unlock(&mutex_exec);

		printf("PROXIMA ESTIMACION DEL PCB %d ELEGIDO: %f", pcb->id_pcb, pcb->estimacion);
		printf("\n");

	}
}



void avejentar_pcbs(t_pcb* unPcb){
	void avejentar(t_pcb* unPcb){
		unPcb->tiempo_espera++;
	}

	list_iterate(lista_ready, avejentar);
}

// Quizá deberíamos pasarla a utils en comun porque se usa en Restaurante
void reestimar(t_pcb* pcb){
	printf("Pasos dados: %d", pcb->repartidor->pasosDados);
	printf("\n");
	pcb->estimacion = pcb->repartidor->pasosDados * alpha + (1-alpha)*pcb->estimacion;
	printf("Nueva estimacion: %f", pcb->estimacion);
	printf("\n");

	printf("\n");
}

void pasar_a_exit(t_pcb* pcb) {
	bool es_el_pcb_que_quiero(void* parametro) {
		t_pcb* pcbDeLista = (t_pcb *) parametro;
		return pcb->id_pcb == pcbDeLista->id_pcb;
	}

	pcb->repartidor->ocupado = false;
	pcb->estado = EXIT;
	sem_post(&semaforo_repartidor_libre);

	pthread_mutex_lock(&mutex_block);
	list_remove_by_condition(lista_bloqueados, es_el_pcb_que_quiero);
	pthread_mutex_unlock(&mutex_block);

	log_info(logger,"El pedido %d fue entregado",pcb->id_pedido);

	enviar_finalizar_pedido(pcb);

	socketConexionComanda = crear_conexion(ip_comanda, puerto_comanda);
	if (socketConexionComanda == -1) {
		log_info(logger, "No me pude conectar a la Comanda");
	} else if (socketConexionComanda > 0) {
		log_info(logger, "Me conecte exitosamente a la Comanda");
		enviar_finalizar_pedido_comanda(pcb, socketConexionComanda);
	}

	close(socketConexionComanda);
}

void avanzar_generico(t_pcb* pcb, void(*accionMientrasCamino)(t_pcb*), void(*accionDespuesDeCaminar)(t_pcb*)){
	log_info(logger, "Ejecutando PCB: %d", pcb->id_pcb);

	pcb->estado = EXEC;
	t_repartidor* repartidor = pcb->repartidor;
	posicion posicionDestino;

	bool esta_cansado() {
		return repartidor->frecuencia_descanso <= (pcb->repartidor->pasosDados);
	}

	// Avanzar hacia donde
	if (!pcb->recogidoDelRestaurante) {
		posicionDestino = pcb->posicion_restaurante;
		log_info(logger, "Repartidor del pedido %d yendo a RESTAURANTE", pcb->id_pedido);
	} else {
		posicionDestino = pcb->posicion_cliente;
		log_info(logger, "Repartidor del pedido %d yendo a CLIENTE", pcb->id_pedido);
	}

	// Donde se ejecuta, propiamente dicho
	while (!llego_a_destino(repartidor, posicionDestino) && !esta_cansado()) {
		avanzar_hacia(repartidor, posicionDestino);
		pcb->repartidor->pasosDados++;
		accionMientrasCamino(pcb);
		sleep(retardo_ciclo_cpu);
	}

	accionDespuesDeCaminar(pcb);

	if (llego_a_destino(pcb->repartidor, pcb->posicion_cliente)) {
		pasar_a_exit(pcb);
	} else if (esta_cansado()) {
		log_info(logger, "El repartidor del pedido %d esta descansando", pcb->id_pedido);
		bloquear_haciendo(descansar, pcb);
	} else if (llego_a_destino(pcb->repartidor, pcb->posicion_restaurante)) {
			log_info(logger,"El repartidor del pedido %d llego al restaurante", repartidor->id_pedido);
		if (platos_terminados(pcb)) {
			pcb->recogidoDelRestaurante = true;
			log_info(logger,"El repartidor del pedido %d ya tiene los platos del restaurante", repartidor->id_pedido);
			avanzar_generico(pcb, accionMientrasCamino, accionDespuesDeCaminar);
		} else {
			pcb->estaEsperando = true;
			bloquear_pcb(pcb);
		}
	}
}
