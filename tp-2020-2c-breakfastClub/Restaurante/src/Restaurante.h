#ifndef RESTAURANTE_SRC_RESTAURANTE_H_
#define RESTAURANTE_SRC_RESTAURANTE_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils_Restaurante.h"
#include "planificacion_restaurante.h"

t_log* iniciar_logger(void);
void leer_config();
void terminar_programa();
void inicializar_restaurante();

#endif /* RESTAURANTE_SRC_RESTAURANTE_H_ */
