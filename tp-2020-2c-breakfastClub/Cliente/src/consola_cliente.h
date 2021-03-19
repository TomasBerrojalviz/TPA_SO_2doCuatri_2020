#ifndef SRC_CONSOLA_CLIENTE_H_
#define SRC_CONSOLA_CLIENTE_H_

#include<stdio.h>
#include<stdlib.h>
#include "utils_cliente.h"

void leer_consola(void);
int contar_elementos_array(char**);
void confirmar_pedido_app(void* msg);
void confirmar_pedido_restaurante(void* msg);
t_receta_respuesta* deserializar_obtener_receta_respuesta(uint32_t socket_cliente);

#endif /* SRC_CONSOLA_CLIENTE_H_ */
