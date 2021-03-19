/*
 * Sindicato.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef SINDICATO_H_
#define SINDICATO_H_

#include<stdio.h>
#include<stdlib.h>
#include "datos_sindicato.h"
#include "consola_sindicato.h"

void iniciar_sindicato();
t_log* iniciar_logger(void);
void leer_config();
void FS_RESET();
void terminar_programa();
void crear_punto_de_montaje();

#endif /* SINDICATO_H_ */
