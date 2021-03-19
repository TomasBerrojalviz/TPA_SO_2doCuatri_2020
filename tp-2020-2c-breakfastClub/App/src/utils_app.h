
#ifndef SRC_UTILS_APP_H_
#define SRC_UTILS_APP_H_

// Includes
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<netdb.h>
#include<string.h>
// Incluir datos de los demas modulos
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<readline/readline.h>
#include "/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/utils_en_comun.h"
#include <semaphore.h>

//---------ENUMS--------//

typedef enum {
	NEW,
	READY,
	BLOCKED,
	EXEC,
	EXIT
} t_estado;

typedef enum {
	FIFO,
	HRRN,
	SJF_SD
}t_algoritmo;

typedef enum{
	PENDIENTE,
	CONFIRMADO,
	TERMINADO
} estado_pedido;

typedef enum {
	OK2,
	FAIL2
} t_respuesta2;


//-------------------------//

//---------Typedefs--------//


typedef struct{
	uint32_t posX;
	uint32_t posY;
} posicion;

typedef struct{
	uint32_t tamanioId;
	char* idCliente;
	posicion posCliente;
	uint32_t socket;
} datos_cliente;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} seleccionar_restaurante;

typedef struct{
	char* nombre_restaurante;
	uint32_t cantidad_cocineros;
	posicion posicion;
	t_list* afinidad_cocineros;
	t_list* platos;
	t_list* precio_platos;
	uint32_t cantidad_hornos;
} crear_restaurante;

typedef struct{
	char* nombre_receta;
	t_list* pasos;
	t_list* tiempo_pasos;
} crear_receta;

typedef struct {
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} consultar_platos;

typedef struct{
	uint32_t cantPlatos;
	int* tamaniosPlato;
	char** nombresPlato;
	int* preciosPlato;
} respuesta_consultar_platos;

typedef struct{ // TIRAR REFACTOR PARA T_RESTAURANTE
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} obtener_restaurante;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t idPedido;
} guardar_pedido;

typedef struct{
	uint32_t tamanioPlato;
	char* nombrePlato;
	uint32_t idPedido;
} aniadir_plato;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t tamanioNombrePlato;
	char* nombrePlato;
	uint32_t cantidad_platos;
	uint32_t id_pedido;
} guardar_plato;

typedef struct{
	uint32_t id_pedido;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
} confirmar_pedido;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t tamanioNombrePlato;
	char* nombrePlato;
	uint32_t id_pedido;
} plato_listo;

typedef struct{
	uint32_t idPedido;
}consultar_pedido;

typedef struct{
	estado_pedido estadoPedido;
	uint32_t cantidadPlatos;
	t_list* platos;
} respuesta_pedido;

typedef struct{
	uint32_t tamanioNombre;
	char* nombrePlato;
	uint32_t cantidadPlato;
	uint32_t cantidadLista;
} plato;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t id_pedido;
} obtener_pedido;

/*typedef struct{
	uint32_t cantidadElementos;
	char** listaPlatos;
	char** cantidadPlatos;
	char** cantidadLista;

};*/

typedef struct{
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	uint32_t idPedido;
} terminar_pedido;

typedef struct{
	uint32_t tamanioNombre;
	char* nombreReceta;
} obtener_receta;

typedef struct{
	posicion posicion;
	uint32_t frecuencia_descanso;
	uint32_t pasosDados;
	uint32_t tiempo_descanso;
	uint32_t id_pedido;
	bool ocupado;
} t_repartidor;

typedef struct {
	estado_pedido estadoPedido;
	char** listaPlatos;
	char** cantidadPlatos;
	char** cantidadLista;
	uint32_t precioTotal;
	uint32_t id_restaurante;
} t_pedido;

typedef struct{
	uint32_t socketRestaurante;
	uint32_t tamanioNombre;
	char* nombreRestaurante;
	posicion posicion;
	uint32_t codigoRestaurante;
} restaurante_conectado;

typedef struct{
	uint32_t tamanio;
	char* nombreRestaurante;
	posicion posicion;
}t_restaurante2;

typedef struct{
	uint32_t id_pcb;
	uint32_t id_pedido;
	t_estado estado;
	posicion posicion_restaurante;
	char* nombreRestaurante;
	posicion posicion_cliente;
	uint32_t socketCliente;
	t_repartidor* repartidor;
	bool recogidoDelRestaurante;
	uint32_t tiempo_espera;
	float estimacion;
	bool estaEsperando;
} t_pcb;

typedef struct{
	char* idCliente;
	uint32_t socketRestaurante;
} vinResCli;

//-------------------------//

//---------CONFIG--------//

char* ip_comanda;
char* puerto_comanda;
char* puerto_escucha;
uint32_t retardo_ciclo_cpu;
uint32_t grado_multiprocesamiento;
char* algoritmo_planificacion;
double alpha;
double estimacion_inicial;
char** repartidores;
char** frecuencia_de_descanso;
char** tiempo_de_descanso;
char* archivo_log;
char** platos_default;
char* ip_app;
uint32_t posicion_rest_default_x;
uint32_t posicion_rest_default_y;

//-------------------------//

//---------MAIN--------//

t_config* config;
t_log* logger;
uint32_t socketConexionComanda;
uint32_t socketConexionRestaurante;
uint32_t socketConexionCliente;
uint32_t id_pcb;
uint32_t grado_multiprogramacion;
t_list* lista_repartidores;
t_list* restaurantes_conectados;
t_list* lista_pcbs;
posicion posicionDefault;
t_list* restaurantesPorCliente;
uint32_t idPedidoDefault;
uint32_t idRestaurantes;
uint32_t offsetCodRestaurante;
datos_cliente* clienteActual;
guardar_plato* mensaje;
confirmar_pedido* confirmar_pedido_recibido;

t_queue* cola_new;
t_list* lista_ready;
t_list* lista_bloqueados;
t_queue* cola_exit;

//-------------------------//

//---------THREADS--------//
pthread_t hiloReceive;
pthread_t planificar_largo;
pthread_t planificar_corto;
//-------------------------//

//---------SEMAFOROS--------//
sem_t semaforo_planificacion;
sem_t semaforo_nuevo_pcb;
sem_t semaforo_ready_pcb;
sem_t semaforo_repartidor_libre;
pthread_mutex_t mutex_idPcb;
pthread_mutex_t mutex_block;
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_cola_ready;
pthread_mutex_t idPedidoG;
pthread_mutex_t repartidorCercano;
pthread_mutex_t restConectados;
pthread_mutex_t vinculos;
pthread_mutex_t mutexAniadirPlato;
pthread_mutex_t mutexConfirmarPedido;
pthread_mutex_t conexionRestaurantes;
pthread_mutex_t mutexClienteActual;
//-------------------------//

//---------FUNCIONES--------//
void* planificar_corto_plazo;
void iniciar_servidor(void);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
void procesar_mensaje_recibido(int cod_op, int cliente_fd);
void retornar_restaurantes_conectados(void);
t_respuesta2 recibir_respuesta(int socket);
posicion obtener_posicion_restaurante(char* nombre_restaurante);
void planificacion(void);
t_list* repartidores_libres();
bool repartidor_esta_libre(t_repartidor* repartidor);
bool restaurante_esta_conectado(void);
restaurante_conectado* obtener_restaurante_conectado(int socketRestaurante);
char* obtener_restaurante_del_cliente(void);
void asociar_restaurante(char* nombreRestaurante);
void romper_asociacion(datos_cliente* cliente);
char enteroACaracter(int numero);
//-------------------------//

#endif /* SRC_UTILS_APP_H_ */
