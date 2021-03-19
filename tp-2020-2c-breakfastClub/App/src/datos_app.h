#ifndef SRC_DATOS_APP_H_
#define SRC_DATOS_APP_H_

#include "utils_app.h"
#include "/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/utils_en_comun.h"
#include <commons/collections/list.h>

//-----------DESERIALIZACION--------------//

confirmar_pedido* deserializar_confirmar_pedido(int socket_cliente);
seleccionar_restaurante* deserializar_seleccionar_restaurante(int socket_cliente);
guardar_pedido* deserializar_guardar_pedido(int socket_cliente);
consultar_platos* deserializar_consultar_platos(int socket_cliente);
aniadir_plato*  deserializar_aniadir_plato(int socket_cliente);
respuesta_consultar_platos* deserializar_respuesta_consultar_platos(int socket_cliente);
plato_listo* deserializar_plato_listo(int socket_cliente);

//-----------SERIALIZACION---------------//
void serializar_confirmar_pedido(confirmar_pedido* msg, t_buffer* buffer);
void serializar_consultar_restaurantes(t_buffer* buffer);
void serializar_aniadir_plato(aniadir_plato* msg, t_buffer* buffer);
void serializar_guardar_pedido(guardar_pedido* msg, t_buffer* buffer);
void serializar_guardar_plato(guardar_plato* msg, t_buffer* buffer);
void serializar_obtener_pedido(obtener_pedido* msg, t_buffer* buffer);
void serializar_plato_listo(plato_listo* msg, t_buffer* buffer);
void serializar_respuesta(t_respuesta2 rta, t_buffer* buffer);
void serializar_respuesta_platos(char** listaPlatos, t_buffer* buffer);
void serializar_respuesta_id(t_respuesta2 rta, uint32_t id, t_buffer* buffer);
void serializar_finalizar_pedido(t_pcb* pcb, t_buffer* buffer);
void serializar_consultar_platos(consultar_platos* msg, t_buffer* buffer);
void serializar_respuesta_consultar_platos(respuesta_consultar_platos* msg, t_buffer* buffer);
void serializar_respuesta_consultar_pedido(respuesta_pedido* respuesta, char* nombreRestaurante, t_buffer* buffer);

//-----------ENVIO--------------//

void enviar_respuesta(t_respuesta2 respuesta, int cliente);
void enviar_respuesta_platos(char** listaPlatos, int cliente);
void enviar_cerrar_conexion(uint32_t socketConexion);
void enviar_crear_pedido(uint32_t socketConexion);
void enviar_confirmar_pedido(confirmar_pedido* msg, uint32_t socketConexion);
void enviar_guardar_pedido(guardar_pedido* msg, uint32_t socketConexion);
void enviar_aniadir_plato(aniadir_plato* msg, uint32_t socketConexion);
void enviar_guardar_plato(guardar_plato* msg, uint32_t socketConexion);
respuesta_pedido* enviar_obtener_pedido(obtener_pedido* msg, uint32_t socketConexion);
void enviar_restaurantes_conectados(uint32_t socketConexion);
void enviar_finalizar_pedido(t_pcb* pcb);
void enviar_consultar_platos(uint32_t socketConexion);
void enviar_respuesta_consultar_platos(respuesta_consultar_platos* msg, uint32_t socketConexion);
bool respuesta_obtener_pedido(obtener_pedido* msg, uint32_t socketConexion);
uint32_t obtener_respuesta_crear_pedido(uint32_t socketConexion);
void enviar_respuesta_consultar_pedido (respuesta_pedido* respuesta, char* nombreRestaurante, uint32_t socketConexion);

#endif /* SRC_DATOS_APP_H_ */
