#include "Sindicato.h"

//#define ERROR_ARGUMENTOS 2

int main() {
	iniciar_sindicato();

	pthread_create(&hiloReceive, NULL, (void*) iniciar_servidor, NULL);
	pthread_detach(hiloReceive);	// Join -> Un hilo X espera a otro hilo Y a que termina para poder segur ejecutando.

	leer_consola();

	terminar_programa();

	return EXIT_SUCCESS;
}



void iniciar_sindicato(){

	leer_config();
	FS_RESET();
	logger = iniciar_logger();

	crear_punto_de_montaje();
	inicializar_paths_aux();

	obtener_metada();
	inicializar_bitmap();
	inicializar_bloques();
	inicializar_semaforos();

}

void FS_RESET(){
	uint32_t borrarFS;
	printf("Desea reiniciar el FileSystem? (SI=1 | NO=0) :");
	scanf("%d",&borrarFS);

	if(borrarFS){
		char* aux = string_new();
		string_append_with_format(&aux, "sh eliminadorFS.sh %s", punto_montaje);
		system(aux);
		free(aux);
	}
}



void crear_punto_de_montaje(void){
	struct stat st = {0};
	if(stat(punto_montaje, &st) == -1){
		mkdir(punto_montaje, 0777);
	}
}

t_log* iniciar_logger(void) {
	return log_create(archivo_log, "Sindicato.c", 1, LOG_LEVEL_INFO);
}

void leer_config(void) {
	config = config_create(ARCHIVO_CONFIG);

	ip_sindicato = config_get_string_value(config, "IP_SINDICATO");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	punto_montaje = config_get_string_value(config, "PUNTO_MONTAJE");
	archivo_log = config_get_string_value(config, "ARCHIVO_LOG");
}

void terminar_programa() {
	log_destroy(logger);

	config_destroy(config);

	pthread_mutex_destroy(mutexBlocks);

	free(path_restaurantes);
	free(path_recetas);
	free(path_metadata);
	free(path_bloques);
	free(bitarrayFS);
	free(ruta_bitmap);
	free(ruta_metada);
	free(ruta_bloques);
	free(mutexBlocks);
	free(magic_number);
	free(path_files);

	//TODO Terminar conexiones... seguramente de cliente y restaurante


	printf("\t\t\t~. SINDICATO FINALIZADO .~\n");
}
