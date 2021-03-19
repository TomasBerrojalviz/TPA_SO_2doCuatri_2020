#include "utils_Restaurante.h"

#ifndef SRC_PLANIFICACION_RESTAURANTE_H_
#define SRC_PLANIFICACION_RESTAURANTE_H_

void crear_colas_planificacion();
void* convertir(char* algoritmo_nombre);
void crear_pcb(obtener_pedido* pedido);
void planificar_largo_plazo(void);
void asignar_cola_segun_plato();
void* planificacion_corto_plazo(colaReady* colaDeReady);
//void* planificacion_segun_RR(colaReady* colaDeReady);
void crear_pcbs_pedido(obtener_pedido_respuesta* pedido, uint32_t id_pedido, uint32_t socketSolicitante);
bool necesitaReposar(char* paso);
bool necesitaHornear(char* paso);
bool necesitaPreparar(char* paso);
void empezar_a_cocinar(colaReady* colaReady);
void manejar_horno();
bool quedan_pasos(t_pcb_plato* pcb);
void hacer_ciclos(t_pcb_plato* pcb);
void reposar(t_pcb_plato* pcb);
void finalizar_plato(t_pcb_plato* pcb);
void finalizar_pedido(uint32_t id_pedido, uint32_t socketSolicitante);
void elegir_destino(t_pcb_plato* pcb);
void liberar_pcb(t_pcb_plato* pcb);


#endif /* SRC_PLANIFICACION_RESTAURANTE_H_ */
