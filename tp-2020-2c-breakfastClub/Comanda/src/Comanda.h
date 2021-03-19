#ifndef SRC_COMANDA_H_
#define SRC_COMANDA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils_comanda.h"

t_log* iniciar_logger(void);
void leer_config();
void terminar_programa();
void inicializar_modulo(void);

#endif /* SRC_COMANDA_H_ */
