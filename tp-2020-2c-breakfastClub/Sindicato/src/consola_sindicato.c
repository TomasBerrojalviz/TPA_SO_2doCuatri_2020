#include "consola_sindicato.h"

// Consola

void leer_consola() {
	char* leido = readline(">");

	while (strcmp(leido, "\0")) {
		char** palabras_del_mensaje = string_split(leido, " ");

		if (!strcmp(palabras_del_mensaje[0], "crearRestaurante")) {
			if (contar_elementos_array(palabras_del_mensaje) != 8) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			}
			else {
				log_info(logger, "SINDICATO :: Creamos el restaurante %s", palabras_del_mensaje[1]);

				char* path = crear_path_restaurante(palabras_del_mensaje[1]);
				crear_archivo_info(path, palabras_del_mensaje);

				free(path);
			}
		}

		if (!strcmp(palabras_del_mensaje[0], "crearReceta")) {
			if (contar_elementos_array(palabras_del_mensaje) != 4) {
				log_info(logger, "Cantidad incorrecta de argumentos");
			}
			else {
				log_info(logger, "SINDICATO :: Creamos la receta %s", palabras_del_mensaje[1]);

				char* path = crear_path_receta(palabras_del_mensaje[1]);
				crear_archivo_receta(path, palabras_del_mensaje);

				free(path);
			}
		}

		free(leido);
		string_iterate_lines(palabras_del_mensaje,(void*) free);
		free(palabras_del_mensaje);
		leido = readline(">");
	}

	free(leido);
}
