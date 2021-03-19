#ifndef RESTAURANTE_SRC_DATOS_RESTAURANTE_H_
#define RESTAURANTE_SRC_DATOS_RESTAURANTE_H_

#include "utils_Restaurante.h"

//Envio de mensajes
void enviar_sindicato_confirmar_pedido(confirmar_pedido* msg, uint32_t socketConexion);
void enviar_sindicato_consultar_platos(t_restaurante* msg, uint32_t socketConexion);
void enviar_sindicato_obtener_restaurante(t_restaurante* obtenerRestaurante, uint32_t socketConexion);
void enviar_sindicato_obtener_pedido(confirmar_pedido* obtenerPedido, uint32_t socketConexion);
void enviar_sindicato_obtener_receta(char* nombrePlato, uint32_t socketSindicato);
void enviar_app_consultar_platos_respuesta(consultar_platos_respuesta* consultarPlatoRespuesta,  uint32_t socketConexion);
void enviar_sindicato_guardar_pedido(guardar_pedido* msg, uint32_t socketConexion);
void enviar_app_nombre_restaurante(uint32_t socketConexion);
void enviar_respuesta(t_respuesta2 confirmado, uint32_t socketConexion, op_code codigo);
void enviar_respuesta_pedido(t_respuesta2 confirmado, uint32_t id_pedido, uint32_t socketConexion);
void enviar_sindicato_guardar_plato(guardar_plato* msg, uint32_t socketConexion);
void enviar_sindicato_terminar_pedido(terminar_pedido* msg, uint32_t socketSindicato);
void enviar_obtener_pedido_respuesta(obtener_pedido_respuesta* msg, uint32_t socketConexion);
void enviar_sindicato_plato_listo(plato_listo* platoListo, uint32_t socketConexion);
void enviar_finalizar_pedido(uint32_t id_pedido, uint32_t socketSolicitante);

//Serializacion
void serializar_finalizar_pedido(uint32_t id_pedido, t_buffer* buffer);
void serializar_consultar_platos(t_restaurante* msg, t_buffer* buffer);
void serializar_obtener_restaurante(t_restaurante* msg, t_buffer* buffer);
void serializar_sindicato_obtener_pedido(confirmar_pedido* msg, t_buffer* buffer);
void serializar_sindicato_confirmar_pedido(confirmar_pedido* msg, t_buffer* buffer);
void serializar_obtener_receta(char* nombrePlato, t_buffer* buffer);
void serializar_consultar_platos_respuesta(consultar_platos_respuesta* msg, t_buffer* buffer);
void serializar_sindicato_guardar_pedido(guardar_pedido* msg, t_buffer* buffer);
void serializar_app_nombre_restaurante(t_buffer* buffer);
void serializar_sindicato_guardar_pedido(guardar_pedido* msg, t_buffer* buffer);
void serializar_sindicato_guardar_plato(guardar_plato* msg, t_buffer* buffer);
void serializar_sindicato_terminar_pedido(terminar_pedido* msg, t_buffer* buffer);
void serializar_obtener_pedido_respuesta(obtener_pedido_respuesta* msg, t_buffer* buffer);
void serializar_plato_listo(plato_listo* msg, t_buffer* buffer);

//Deserializacion
confirmar_pedido* deserializar_confirmar_pedido(int socket_cliente);
obtener_pedido_respuesta* deserializar_obtener_pedido_respuesta(uint32_t socket_cliente);
consultar_platos_respuesta* deserializar_consultar_platos_respuesta(uint32_t socket_cliente);
t_receta_respuesta* deserializar_obtener_receta_respuesta(uint32_t socket_cliente);
aniadir_plato*  deserializar_aniadir_plato(int socket_cliente);

char* dar_estado_pedido(estado_pedido estado);

#endif /* RESTAURANTE_SRC_DATOS_RESTAURANTE_H_ */
