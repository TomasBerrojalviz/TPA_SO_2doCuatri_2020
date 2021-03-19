

#ifndef SRC_DATOS_CLIENTE_H_
#define SRC_DATOS_CLIENTE_H_

#include "utils_cliente.h"

typedef enum{
	OK2,
	FAIL2
}t_respuesta2;

//---------------Deserializacion--------------------//

finalizar_pedido* deserializar_finalizar_pedido(int socket_cliente);

//---------------Envio de mensajes--------------------//

void enviar_datos_cliente(uint32_t socketConexion);
void enviar_cerrar_conexion(uint32_t socketConexion);
void enviar_consultar_restaurantes(uint32_t socketConexion);
void enviar_seleccionar_restaurante(seleccionar_restaurante* msg, uint32_t socketConexion);
void enviar_obtener_restaurante(obtener_restaurante* msg, uint32_t socketConexion);
void enviar_consultar_platos(consultar_platos* msg, uint32_t socketConexion);
void enviar_crear_pedido(uint32_t socketConexion);
void enviar_guardar_pedido(guardar_pedido* msg, uint32_t socketConexion);
void enviar_aniadir_plato(aniadir_plato* msg, uint32_t socketConexion);
void enviar_guardar_plato(guardar_plato* msg, uint32_t socketConexion);
void enviar_confirmar_pedido(confirmar_pedido* msg, uint32_t socketConexion);
void enviar_confirmar_pedido_restaurante(confirmar_pedido* msg, uint32_t socketConexion);
void enviar_plato_listo(plato_listo* msg, uint32_t socketConexion);
void enviar_consultar_pedido(consultar_pedido* msg, uint32_t socketConexion);
void enviar_obtener_pedido(obtener_pedido* msg, uint32_t socketConexion);
void enviar_finalizar_pedido(finalizar_pedido* msg, uint32_t socketConexion);
void enviar_terminar_pedido(terminar_pedido* msg, uint32_t socketConexion);
void enviar_obtener_receta(obtener_receta* msg, uint32_t socketConexion);

//-----------------------------------------------------//

t_respuesta2 recibir_respuesta(uint32_t socketConexion);

//------------------Serializacion----------------------//

void serializar_datos_cliente(t_buffer* buffer);
void serializar_seleccionar_restaurante(seleccionar_restaurante* msg, t_buffer* buffer);
void serializar_obtener_restaurante(obtener_restaurante* msg, t_buffer* buffer);
void serializar_consultar_platos(consultar_platos* msg, t_buffer* buffer);
void serializar_guardar_pedido(guardar_pedido* msg, t_buffer* buffer);
void serializar_aniadir_plato(aniadir_plato* msg, t_buffer* buffer);
void serializar_guardar_plato(guardar_plato* msg, t_buffer* buffer);
void serializar_confirmar_pedido(confirmar_pedido* msg, t_buffer* buffer);
void serializar_confirmar_pedido_restaurante(confirmar_pedido* msg, t_buffer* buffer);
void serializar_plato_listo(plato_listo* msg, t_buffer* buffer);
void serializar_consultar_pedido(consultar_pedido* msg, t_buffer* buffer);
void serializar_obtener_pedido(obtener_pedido* msg, t_buffer* buffer);
void serializar_finalizar_pedido(finalizar_pedido* msg, t_buffer* buffer);
void serializar_terminar_pedido(terminar_pedido* msg, t_buffer* buffer);
void serializar_obtener_receta(obtener_receta* msg, t_buffer* buffer);

//-----------------------------------------------------//

//----------------Escucha pedido---------------------//

void confirmar_pedido_app(void* msg);
void confirmar_pedido_restaurante(void* msg);

//-----------------------------------------------------//
#endif /* SRC_DATOS_CLIENTE_H_ */
