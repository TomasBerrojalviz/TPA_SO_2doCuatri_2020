#include "utils_en_comun.h"

void* serializar_paquete(t_paquete* paquete, int* bytes){
	(*bytes) = sizeof(op_code) + sizeof(uint32_t) + (paquete->buffer->size);
	int offset = 0;
	void* stream_a_enviar = malloc(*bytes);

	memcpy(stream_a_enviar + offset, &(paquete->codigo_operacion), sizeof(op_code));
	offset += sizeof(op_code);

	memcpy(stream_a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream_a_enviar + offset, (paquete->buffer->stream), paquete->buffer->size);

	return stream_a_enviar;
}

void crear_buffer(t_paquete* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(uint32_t unOp_code) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = unOp_code;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente) {
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

uint32_t crear_conexion(char* ip, char* puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	uint32_t socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) return -1;

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente){
	close(socket_cliente);
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

int contar_elementos_array(char** array) {
	int contador = 0;

	while (array[contador] != NULL) {
		contador++;
	}

	return contador;
}

void* serializar_buffer_respuesta(t_buffer* buffer, int* bytes){
    (*bytes) = sizeof(uint32_t) + (buffer->size);
    int offset = 0;
    void* stream_a_enviar = malloc(*bytes);

    memcpy(stream_a_enviar + offset, &(buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream_a_enviar + offset, (buffer->stream), buffer->size);

    return stream_a_enviar;
}

t_buffer* crear_buffer_respuesta(){
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = 0;
    buffer->stream = NULL;

    return buffer;
}
