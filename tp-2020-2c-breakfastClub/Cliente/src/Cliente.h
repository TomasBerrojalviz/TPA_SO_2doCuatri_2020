/*
 * Cliente.h
 *
 *  Created on: 31 ago. 2020
 *      Author: utnso
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_

#include<stdio.h>
#include<stdlib.h>
#include "utils_cliente.h"
#include "consola_cliente.h"

t_log* iniciar_logger(void);
void inicializar_cliente();
void leer_config();
void terminar_programa();


#endif /* CLIENTE_H_ */
