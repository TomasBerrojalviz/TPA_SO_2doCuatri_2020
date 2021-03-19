
#ifndef SRC_APP_H_
#define SRC_APP_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/collections/list.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/utils_en_comun.h"
#include "utils_app.h"
#include "planificacion_app.h"

t_log* iniciar_logger(void);
void leer_config();
void terminar_programa();
int contar_elementos_array(char** array);
void inicializar_repartidores();
t_list* repartidores_libres();
int calcular_distancia(t_repartidor* repartidor, int x, int y);
void agregar_repartidor_a_lista(char* posiciones, char* frecuencia_descanso, char* tiempo_descanso);
t_repartidor* repartidor_libre_mas_cercano(int x, int y);
bool esta_mas_cerca(t_repartidor* repartidor1, t_repartidor* repartidor2, int x, int y);
void handshake_comanda(void);
void inicializar_modulo(void);


#endif /* SRC_APP_H_ */
