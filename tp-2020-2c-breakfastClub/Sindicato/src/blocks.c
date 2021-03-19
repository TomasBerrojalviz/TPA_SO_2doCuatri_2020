/*
 * blocks.c
 *
 *  Created on: 28 nov. 2020
 *      Author: utnso
 */
#include "blocks.h"

t_bloque_FS* crear_bloque(char* stream, uint32_t siguiente){
	t_bloque_FS* bloque = malloc(sizeof(t_bloque_FS));
	bloque->diccionario = dictionary_create();
	bloque->stream = string_duplicate(stream);
	bloque->bloqueSiguiente = siguiente;

	char** lines = string_split(stream, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char** keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(bloque->diccionario, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);

	free(lines);

	return bloque;
}

bool tiene_dato(t_bloque_FS* bloque, char* key){
	return dictionary_has_key(bloque->diccionario, key);
}

char* obtener_dato_string(t_bloque_FS* bloque, char *key){
	return dictionary_get(bloque->diccionario, key);
}

uint32_t obtener_dato_int(t_bloque_FS* bloque, char *key){
	char* value = obtener_dato_string(bloque, key);
	uint32_t valorInt = atoi(value);
	free(value);
	return valorInt;
}

char** obtener_dato_lista(t_bloque_FS* bloque, char* key){
	char* value_in_dictionary = obtener_dato_string(bloque, key);
	return string_get_string_as_array(value_in_dictionary);
}

char* obtener_stream_receta(t_receta_respuesta* msg){
//	pasos
//	tiempo pasos

	uint32_t cantPasos = list_size(msg->listaPasos);
	uint32_t tamanio = strlen("PASOS=[]\nTIEMPO_PASOS=[]")+1;
	uint32_t offset = 0;

	for(int i=0;i<cantPasos;i++){
		char* aux = string_itoa( (uint32_t) list_get(msg->listaTiempoPasos, i));
		tamanio += ((uint32_t) list_get(msg->listaTamanioPasos, i)) + 1;
		tamanio += strlen(aux) +2;
		free(aux);
	}
	char* stream = malloc(tamanio);

	sprintf(stream+offset, "PASOS=[");
	offset += strlen("PASOS=[");

	for(int i=0;i<cantPasos;i++){
		sprintf(stream+offset, "%s", (char*) list_get(msg->listaPasos, i));
		offset += strlen((char*) list_get(msg->listaPasos, i));
		if(i!=cantPasos-1){
			sprintf(stream+offset, ",");
			offset++;
		}
	}
	sprintf(stream+offset, "]\n");
	offset += strlen("]\n");
	sprintf(stream+offset, "TIEMPO_PASOS=[");
	offset += strlen("TIEMPO_PASOS=[");
	for(int i=0;i<cantPasos;i++){
		char* aux = string_itoa( (uint32_t) list_get(msg->listaTiempoPasos, i));
		sprintf(stream+offset, "%d", (uint32_t) list_get(msg->listaTiempoPasos, i));
		offset += strlen(aux);
		free(aux);
		if(i!=cantPasos-1){
			sprintf(stream+offset, ",");
			offset++;
		}
	}
	sprintf(stream+offset, "]");

	return stream;
}

char* obtener_stream_restaurante(obtener_restaurante_respuesta* msg){
	//------------ORDEN------------
	//1. Cantidad cocineros
	//2. Posicion
	//3. afinidad_cocineros         }   DENTRO DE UN FOR
	//4. platos               		} COMO SON LISTAS IRAN
	//5. precio_platos         		}   DENTRO DE UN FOR
	//6. cantidad_hornos
	//7. cantidadPedidos
	//-----------------------------

	uint32_t cantidadAfinidades = list_size(msg->afinidad_cocineros);
	uint32_t cantidadPlatos = list_size(msg->platos);
	uint32_t offset = 0;

	uint32_t tamanio = strlen("CANTIDAD_COCINEROS=\nPOSICION=[,]\nAFINIDAD_COCINEROS=[]\nPLATOS=[]\nPRECIO_PLATOS=[]\nCANTIDAD_HORNOS=\nCANTIDAD_PEDIDOS=")+1;

	for(int i=0;i<cantidadAfinidades;i++){
		tamanio += ((uint32_t) list_get(msg->listaTamanioAfinidades, i)) + 1;
	}
	for(int i=0;i<cantidadPlatos;i++){
		tamanio += ((uint32_t) list_get(msg->listaTamanioPlatos, i)) + 1;
		char* aux = string_itoa( (uint32_t) list_get(msg->precio_platos, i));
		tamanio += strlen(aux) +2;
		free(aux);
	}

	char* stream = malloc(tamanio+1); //TODO como mierda le asigno memoria a este

	sprintf(stream+offset, "CANTIDAD_COCINEROS=%d\nPOSICION=[%d,%d]\nAFINIDAD_COCINEROS=[", (msg->cantidad_cocineros), (msg->posicion.posX), (msg->posicion.posY));
	offset += strlen(stream)-offset;
	for(int i=0;i<cantidadAfinidades;i++){
		sprintf(stream+offset, "%s", (char*) list_get(msg->afinidad_cocineros, i));
		offset += strlen((char*) list_get(msg->afinidad_cocineros, i));
		if(i!=cantidadAfinidades-1){
			sprintf(stream+offset, ",");
			offset++;
		}
	}
	sprintf(stream+offset, "]\nPLATOS=[");
	offset += strlen("]\nPLATOS=[");
	for(int i=0;i<cantidadPlatos;i++){
		sprintf(stream+offset, "%s", (char*) list_get(msg->platos, i));
		offset += strlen((char*) list_get(msg->platos, i));
		if(i!=cantidadPlatos-1){
			sprintf(stream+offset, ",");
			offset++;
		}
	}
	sprintf(stream+offset, "]\nPRECIO_PLATOS=[");
	offset += strlen("]\nPRECIO_PLATOS=[");
	for(int i=0;i<cantidadPlatos;i++){
		char* aux = string_itoa( (uint32_t) list_get(msg->precio_platos, i));
		sprintf(stream+offset, "%d", (uint32_t) list_get(msg->precio_platos, i));
		offset += strlen(aux);
		free(aux);
		if(i!=cantidadPlatos-1){
			sprintf(stream+offset, ",");
			offset++;
		}
	}
	sprintf(stream+offset, "]\nCANTIDAD_HORNOS=%d\nCANTIDAD_PEDIDOS=%d", (msg->cantidad_hornos), (msg->cantidadPedidos));

	return stream;
}
char* obtener_stream_pedido(obtener_pedido_respuesta* msg){
	//------------ORDEN------------
		//1. estado
		//2. platos         			}COMO SON LISTAS IRAN
		//3. listaCantidadDePedido    	}  DENTRO DE UN FOR
		//4. listaCantidadLista			}
		//5. precioTotal
	//-----------------------------
	uint32_t cantPlatos = (uint32_t) list_size(msg->platos);
	uint32_t offset = 0;

	uint32_t tamanio = strlen("ESTADO_PEDIDO=\nLISTA_PLATOS=[]\nCANTIDAD_PLATOS=[]\nCANTIDAD_LISTA=[]\nPRECIO_TOTAL=")+1;
	tamanio += strlen(dar_estado_pedido(msg->estado));
	char* aux0 = string_itoa(msg->precioTotal);
	tamanio += strlen(aux0);
	free(aux0);
	for(int i=0;i<cantPlatos;i++){
		char* aux = string_itoa( (uint32_t) list_get(msg->listaCantidadLista, i));
		char* aux2 = string_itoa( (uint32_t) list_get(msg->listaCantidadDePedido, i));
		tamanio += ((uint32_t) list_get(msg->listaTamanioPlato, i)) + 2;
		tamanio += strlen(aux) +2;
		tamanio += strlen(aux2) +2;
		free(aux);
		free(aux2);
	}

	char* stream = malloc(tamanio); //TODO como mierda le asigno memoria a este


	sprintf(stream+offset, "ESTADO_PEDIDO=%s\nLISTA_PLATOS=[", ((char*) dar_estado_pedido(msg->estado)) );
	offset += strlen(stream) - offset;

	for(int i=0;i<cantPlatos;i++){
		sprintf(stream+offset, "%s", (char*) list_get(msg->platos, i));
		offset += strlen((char*) list_get(msg->platos, i));
		if(i!=cantPlatos-1){
			sprintf(stream+offset, ",");
			offset++;
		}
	}
	sprintf(stream+offset, "]\nCANTIDAD_PLATOS=[");
	offset += strlen("]\nCANTIDAD_PLATOS=[");
	for(int i=0;i<cantPlatos;i++){
		char* aux = string_itoa( (uint32_t) list_get(msg->listaCantidadDePedido, i));
		sprintf(stream+offset, "%d", (uint32_t) list_get(msg->listaCantidadDePedido, i));
		offset += strlen(aux);
		free(aux);
		if(i!=cantPlatos-1){
			sprintf(stream+offset, ",");
			offset++;
		}
	}
	sprintf(stream+offset, "]\nCANTIDAD_LISTA=[");
	offset += strlen("]\nCANTIDAD_LISTA=[");
	for(int i=0;i<cantPlatos;i++){
		char* aux = string_itoa( (uint32_t) list_get(msg->listaCantidadLista, i));
		sprintf(stream+offset, "%d", (uint32_t) list_get(msg->listaCantidadLista, i));
		offset += strlen(aux);
		free(aux);
		if(i!=cantPlatos-1){
			sprintf(stream+offset, ",");
			offset++;
		}
	}
	sprintf(stream+offset, "]\nPRECIO_TOTAL=%d", (msg->precioTotal));

	return stream;
}

t_archivo_FS* crear_data_archivo_FS(uint32_t tamanioTotal, uint32_t bloqueInicial){
	t_archivo_FS* archivoFS = malloc(sizeof(t_archivo_FS));
	archivoFS->diccionario = dictionary_create();

	archivoFS->tamanioTotal = tamanioTotal;
	archivoFS->bloqueInicial = bloqueInicial;
	archivoFS->stream = obtener_stream_archivo(tamanioTotal,bloqueInicial);

	char** lines = string_split(archivoFS->stream, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char** keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(archivoFS->diccionario, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);

	free(lines);

	archivoFS->stream = realloc(archivoFS->stream,archivoFS->tamanioTotal);

	return archivoFS;
}

char* obtener_stream_archivo(uint32_t tamanioTotal, uint32_t bloqueInicial){
	//------------ORDEN------------
	//1. Tamanio total
	//2. Bloque inicial
	//-----------------------------
	uint32_t tamanio = strlen("SIZE=\nINITIAL_BLOCK=\n")+1;
	char* aux = string_itoa(tamanioTotal);
	tamanio += strlen(aux);
	free(aux);
	aux = string_itoa(bloqueInicial);
	tamanio += strlen(aux);
	free(aux);
	uint32_t offset = 0;

	char* stream = malloc(tamanio);

	sprintf(stream+offset, "SIZE=%d\n", tamanioTotal);
	offset += strlen(stream);
	sprintf(stream+offset, "INITIAL_BLOCK=%d\n", bloqueInicial);

	return stream;
}

