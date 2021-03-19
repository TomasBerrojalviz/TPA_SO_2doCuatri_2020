#ifndef DATOS_SINDICATO_H_
#define DATOS_SINDICATO_H_
#include "utils_Sindicato.h"
#include "blocks.h"
#include <unistd.h>

// FileSystem

void bloquear_mutex_bloque(uint32_t bloque);
void desbloquear_mutex_bloque(uint32_t bloque);
void inicializar_paths_aux();
void obtener_metada();
void leer_metadata(FILE* archivo);
void inicializar_bitmap();
void inicializar_bloques();
char* crear_path_restaurante(char* nombre_restaurante);
char* crear_path_receta(char* nombre_receta);
void crear_archivo_info(char* path, char** palabras_del_mensaje);
obtener_restaurante_respuesta* de_consola_a_datos_restaurante(char** palabras_del_mensaje);
t_receta_respuesta* de_consola_a_datos_receta(char** palabras_del_mensaje);
void crear_archivo_receta(char* path, char** palabras_del_mensaje);
t_respuesta2 crear_archivo_pedido(guardar_pedido* nuevoPedido);
t_respuesta2 confirmar_pedido_FS(confirmar_pedido* confirmarPedidoRecibido);
bool directorio_existe(char*);
bool archivo_existe(char* nombreArchivo);
t_archivo_FS* leer_archivo(FILE* archivo);
char* path_info_restaurante(char* nombreRestaurante);
char* path_pedido_restaurante(char* restaurante, uint32_t pedido);
obtener_restaurante_respuesta* obtener_restaurante_FS(t_restaurante* obtenerRestauranteRecibido);
obtener_restaurante_respuesta* de_archivo_a_datos_restaurante(char** palabras_del_mensaje);
obtener_restaurante_respuesta* deserializar_obtener_restaurante_FS(t_buffer* buffer);
consultar_platos_respuesta* consultar_platos_FS(t_restaurante* consultarPlatosRecibido);
consultar_platos_respuesta* obtener_platos_de_datos(obtener_restaurante_respuesta* datosRestaurante);
obtener_pedido_respuesta* obtener_pedido_FS(char* restaurante, uint32_t pedido);
void agregar_al_pedido_plato(obtener_pedido_respuesta* msg, guardar_plato* guardarPlatoRecibido);
t_respuesta2 terminar_pedido_FS(terminar_pedido* terminarPedidoRecibido);
t_respuesta2 verificar_plato_listo(obtener_pedido_respuesta* pedidoObtenido,plato_listo* platoListoRecibido);
t_receta_respuesta* deserializar_receta_FS(char* streamAux);
void subir_receta_bloque(t_receta_respuesta* datosReceta, char* path);
void subir_pedido_bloque(obtener_pedido_respuesta* datosPedido, char* path);
void subir_restaurante_bloque(obtener_restaurante_respuesta* datosRestaurante, char* path);
obtener_restaurante_respuesta* buscar_restaurante_FS(info_bloque* bloqueRestaurante);
obtener_pedido_respuesta* buscar_pedido_FS(info_bloque* bloquePedido);
uint32_t subir_a_FS(char* stream, uint32_t bloquesNecesarios);
uint32_t obtener_bloque_libre(t_bitarray* bitarray);
t_bitarray* leerBitmap();
void crearDirectorios();
void crear_bitmap();
void subir_bitarray_FS(t_bitarray* bitarray);
void crearBloques();
char* dar_estado_pedido(estado_pedido estado);
estado_pedido dar_estado_pedido_INT(char* estado);
char* path_bloque(uint32_t bloque);
obtener_pedido_respuesta* deserializar_pedido_FS(char* streamAux);
obtener_restaurante_respuesta* deserializar_restaurante_FS(char* stream);
void serializar_datos_restaurante_FS(obtener_restaurante_respuesta* msg, t_buffer* buffer);
char* bajar_datos_bloques_y_liberarlos(t_archivo_FS* archivoFS);

// Serializacion
void serializar_restaurante_obtener_pedido_respuesta(obtener_pedido_respuesta* msg, t_buffer* buffer);
void serializar_restaurante_consultar_platos_respuesta(consultar_platos_respuesta* msg, t_buffer* buffer);
void serializar_restaurante_obtener_receta_respuesta(t_receta_respuesta* msg, t_buffer* buffer);
void serializar_datos_restaurante_consola(obtener_restaurante_respuesta* msg, t_buffer* buffer);
void subir_datos_receta_consola(t_receta_respuesta* msg, FILE* archivo);
void subir_datos_restaurante_consola(obtener_restaurante_respuesta* msg, FILE* archivo);
void subir_pedido_restaurante(obtener_pedido_respuesta* msg, FILE* archivo);

// Deserializacion
t_restaurante* deserializar_consultar_platos(int socket_cliente);
t_restaurante* deserializar_obtener_restaurante(int socket_cliente);
guardar_pedido* deserializar_guardar_pedido(int socket_cliente);
guardar_plato* deserializar_guardar_plato(int socket_cliente);
confirmar_pedido* deserializar_confirmar_pedido(int socket_cliente);
plato_listo* deserializar_plato_listo(int socket_cliente);
obtener_pedido* deserializar_obtener_pedido(int socket_cliente);
terminar_pedido* deserializar_terminar_pedido(int socket_cliente);
obtener_receta* deserializar_obtener_receta(int socket_cliente);

//Envio de Mensajes
void enviar_restaurante_obtener_restaurante_respuesta(obtener_restaurante_respuesta* msg, uint32_t socketConexion);
void enviar_restaurante_obtener_pedido_respuesta(obtener_pedido_respuesta* msg, uint32_t socketConexion);
void enviar_respuesta(t_respuesta2 confirmado, uint32_t cliente_fd);
void enviar_restaurante_obtener_restaurante_respuesta(obtener_restaurante_respuesta* msg, uint32_t socketConexion);
void enviar_restaurante_consultar_platos_respuesta(consultar_platos_respuesta* msg, uint32_t socketConexion);
void enviar_restaurante_obtener_receta_respuesta(t_receta_respuesta* obtenerRecetaRespuesta, uint32_t socketConexion);

#endif /* DATOS_SINDICATO_H_ */
