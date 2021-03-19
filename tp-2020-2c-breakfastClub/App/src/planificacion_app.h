
#ifndef SRC_PLANIFICACION_APP_H_
#define SRC_PLANIFICACION_APP_H_

#include "utils_app.h"
#include "/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/utils_en_comun.h"

void planificar(void);
void crear_pcb(int id_pedido, char* nombreRestaurante, uint32_t cliente);
void asignar_repartidor_libre();
t_list* repartidores_libres();

void* convertir(char* algoritmo_nombre);
void descansar(t_pcb* pcb);
void planificar_largo_plazo(void);
void crear_planificacion_segun_algoritmo();
void planificacion_segun_FIFO();
void planificacion_segun_HRRN();
void planificacion_segun_SJF_SD();
void avanzar_fifo(t_pcb* pcb);
void avanzar_hrrn(t_pcb* pcb);
void avanzar_sjf(t_pcb* pcb);
void avejentar(t_pcb* unPcb);
void avejentar_pcbs(t_pcb* pcb);
void reestimar(t_pcb* pcb);
bool llego_a_destino(t_repartidor* repartidor, posicion posicion);
void avanzar_hacia(t_repartidor* repartidor, posicion posicion);
bool quedan_movimientos(uint32_t posicion1, uint32_t posicion2);
void planificacion_bloqueados(void);
bool platos_terminados(t_pcb* pcb);
void bloquear_pcb(t_pcb* pcb);
void pasar_a_exit(t_pcb* pcb);

#endif /* SRC_PLANIFICACION_APP_H_ */
