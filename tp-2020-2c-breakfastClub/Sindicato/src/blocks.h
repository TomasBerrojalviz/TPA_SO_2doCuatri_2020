/*
 * blocks.h
 *
 *  Created on: 28 nov. 2020
 *      Author: utnso
 */

#ifndef BLOCKS_H_
#define BLOCKS_H_

#include "utils_Sindicato.h"

t_bloque_FS* crear_bloque(char* stream, uint32_t siguiente);
bool tiene_dato(t_bloque_FS* bloque, char* key);
char* obtener_dato_string(t_bloque_FS* bloque, char *key);
uint32_t obtener_dato_int(t_bloque_FS* bloque, char *key);
char** obtener_dato_lista(t_bloque_FS* bloque, char* key);
char* obtener_stream_receta(t_receta_respuesta* msg);
char* obtener_stream_restaurante(obtener_restaurante_respuesta* msg);
char* obtener_stream_pedido(obtener_pedido_respuesta* msg);
t_archivo_FS* crear_data_archivo_FS(uint32_t tamanioTotal, uint32_t bloqueInicial);
char* obtener_stream_archivo(uint32_t tamanioTotal, uint32_t bloqueInicial);
uint32_t potencia(uint32_t n1, uint32_t n2);
/*
int config_keys_amount(t_config*);
void config_destroy(t_config *config);
void config_set_value(t_config*, char *key, char *value);
void config_remove_key(t_config*, char *key);
*/

#endif /* BLOCKS_H_ */
