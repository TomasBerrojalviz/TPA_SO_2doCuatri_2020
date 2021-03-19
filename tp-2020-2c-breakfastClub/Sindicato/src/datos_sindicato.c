#include "datos_sindicato.h"

// FileSystem

void bloquear_mutex_bloque(uint32_t bloque){
	pthread_mutex_lock(&mutexBlocks[bloque]);
}
void desbloquear_mutex_bloque(uint32_t bloque){
	pthread_mutex_unlock(&mutexBlocks[bloque]);
}

void inicializar_paths_aux(){
	path_restaurantes = string_new();
	path_recetas = string_new();
	path_metadata = string_new();
	ruta_bitmap = string_new();
	ruta_metada = string_new();
	path_files = string_new();
	path_bloques = string_new();

	string_append_with_format(&path_bloques, "%s/Blocks", punto_montaje);
	string_append_with_format(&path_restaurantes, "%s/Files/Restaurantes", punto_montaje);
	string_append_with_format(&path_recetas, "%s/Files/Recetas", punto_montaje);
	string_append_with_format(&path_files, "%s/Files", punto_montaje);
	string_append_with_format(&path_metadata, "%s/Metadata",punto_montaje);
	string_append_with_format(&ruta_bitmap, "%s/Bitmap.bin", path_metadata);
	string_append_with_format(&ruta_metada, "%s/Metadata.AFIP", path_metadata);


	if (!directorio_existe(path_files)) {
		mkdir(path_files, 0700);
	}
	if (!directorio_existe(path_recetas)) {
		mkdir(path_recetas, 0700);
	}
	if (!directorio_existe(path_restaurantes)) {
		mkdir(path_restaurantes, 0700);
	}

}

void obtener_metada(){
	FILE* metadata = fopen(ruta_metada, "r");

	if(metadata){
		log_info(logger, "SINDICATO :: SE LEE LA METADATA");
		leer_metadata(metadata);
		fclose(metadata);
	}
	else{
		log_info(logger, "SINDICATO :: NO SE PUDO LEER LA METADATA");
		exit(0);
	}
}

void leer_metadata(FILE* archivo){
	char* leido = malloc(sizeof(char));
	int i=0;

	while(fread(leido+i,1,1,archivo)){
		i++;
		leido= (char*) realloc(leido,i+1);
	}
	leido= (char*) realloc(leido,i+1);
	t_bloque_FS* bloqueAux = crear_bloque(leido,0);
	free(leido);

	magic_number = string_duplicate(obtener_dato_string(bloqueAux, "MAGIC_NUMBER"));
	blocks = obtener_dato_int(bloqueAux,"BLOCKS");
	block_size = obtener_dato_int(bloqueAux,"BLOCK_SIZE");

	dictionary_destroy(bloqueAux->diccionario);
	free(bloqueAux->stream);
	free(bloqueAux);
}

void inicializar_bitmap(){
	path_metadata = string_new();

	string_append_with_format(&path_metadata, "%s/Metadata", punto_montaje);

	if (!directorio_existe(path_metadata)) {
		log_info(logger, "NO EXISTE LA METADA");
		exit(0);
//		mkdir(path_metadata, 0700);
	}

	crear_bitmap(path_metadata);
}

void inicializar_bloques(){
	path_bloques = string_new();

	string_append_with_format(&path_bloques, "%s/Blocks", punto_montaje);


	if (!directorio_existe(path_bloques)) {
			mkdir(path_bloques, 0700);
	}

	crearBloques(path_bloques,path_metadata);
}

bool directorio_existe(char* path) {
	struct stat st = { 0 }; // stat(2) precisa de un struct stat a modo de buffer para llenar info del archivo que nosotros no necesitamos.
	char* aux = string_duplicate(path);
	bool existe = (stat(aux, &st) != -1);
	free(aux);
	return existe;
}

int min(int n1, int n2){
	if(n1<n2)
		return n1;
	return n2;
}

char* crear_path_restaurante(char* nombre_restaurante) {
	char* path = string_new();

	string_append_with_format(&path, "%s", path_restaurantes);
	string_append_with_format(&path, "/%s", nombre_restaurante);

	if (!directorio_existe(path)) {

		mkdir(path, 0700);
	}

	return path;
}

char* crear_path_receta(char* nombre_receta) {
	char* path = string_new();

	string_append_with_format(&path, "%s", path_recetas);
	string_append_with_format(&path, "/%s.AFIP", nombre_receta);

	return path;
}

void crear_archivo_info(char* path, char** palabras_del_mensaje){
    char* new = string_new();

    string_append_with_format(&new, "%s/Info.AFIP",path);

//	crear_semaforo(palabras_del_mensaje[1],INFO);
//	bloquear_semaforo(palabras_del_mensaje[1],INFO,0);

	obtener_restaurante_respuesta* datosRestaurante = de_consola_a_datos_restaurante(palabras_del_mensaje);
	datosRestaurante->cantidadPedidos = 0;

	subir_restaurante_bloque(datosRestaurante, new);

	list_destroy(datosRestaurante->afinidad_cocineros);
	list_destroy(datosRestaurante->listaTamanioAfinidades);
	list_destroy(datosRestaurante->listaTamanioPlatos);
	list_destroy(datosRestaurante->platos);
	list_destroy(datosRestaurante->precio_platos);
	free(datosRestaurante);


//    desbloquear_semaforo(palabras_del_mensaje[1],INFO,0);
    free(new);
}

void crear_archivo_receta(char* path, char** palabras_del_mensaje){
	char* new = crear_path_receta(palabras_del_mensaje[1]);

//	crear_semaforo(palabras_del_mensaje[1],RECETA);
//	bloquear_semaforo(palabras_del_mensaje[1],RECETA,0);

	log_info(logger,"SINDICATO :: SE CREA LA NUEVA RECETA");
	t_receta_respuesta* datosReceta = de_consola_a_datos_receta(palabras_del_mensaje);
	//subir_datos_receta_consola(datosReceta, nuevo);
	subir_receta_bloque(datosReceta, new);
	list_destroy(datosReceta->listaPasos);
	list_destroy(datosReceta->listaTamanioPasos);
	list_destroy(datosReceta->listaTiempoPasos);
	free(datosReceta);

//    desbloquear_semaforo(palabras_del_mensaje[1],RECETA,0);
	free(new);
}

void subir_receta_bloque(t_receta_respuesta* datosReceta, char* path){

	char* streamTotal = obtener_stream_receta(datosReceta);

	uint32_t tamanioStream = strlen(streamTotal);
	uint32_t bloquesNecesarios = tamanioStream / (block_size-4); // -4 por los 4 bytes del puntero al siguiente
	if(tamanioStream % (block_size-4)) 	// Si hay resto
		bloquesNecesarios++;		// se tendra en cuenta el mismo

	t_archivo_FS* dataArchivo = crear_data_archivo_FS(tamanioStream,subir_a_FS(streamTotal, bloquesNecesarios));

	FILE* archivo = fopen(path, "w+");

	fwrite(dataArchivo->stream,strlen(dataArchivo->stream),1,archivo);

	dictionary_destroy(dataArchivo->diccionario);
	free(dataArchivo->stream);
	free(dataArchivo);
	fclose(archivo);
}

void subir_pedido_bloque(obtener_pedido_respuesta* datosPedido, char* path){

	char* streamTotal = obtener_stream_pedido(datosPedido);

	uint32_t tamanioStream = strlen(streamTotal);
	uint32_t bloquesNecesarios = tamanioStream / (block_size-4); // -4 por los 4 bytes del puntero al siguiente
	if(tamanioStream % (block_size-4)) 	// Si hay resto
		bloquesNecesarios++;		// se tendra en cuenta el mismo

	t_archivo_FS* dataArchivo = crear_data_archivo_FS(tamanioStream,subir_a_FS(streamTotal, bloquesNecesarios));

	FILE* archivo = fopen(path, "w+");

	fwrite(dataArchivo->stream,strlen(dataArchivo->stream),1,archivo);

	dictionary_destroy(dataArchivo->diccionario);
	free(dataArchivo->stream);
	free(dataArchivo);
	fclose(archivo);
}

void subir_restaurante_bloque(obtener_restaurante_respuesta* datosRestaurante, char* path){

	char* streamTotal = obtener_stream_restaurante(datosRestaurante);

	uint32_t tamanioStream = strlen(streamTotal);
	uint32_t bloquesNecesarios = tamanioStream / (block_size-4); // -4 por los 4 bytes del puntero al siguiente
	if(tamanioStream % (block_size-4)) 	// Si hay resto
		bloquesNecesarios++;		// se tendra en cuenta el mismo

	t_archivo_FS* dataArchivo = crear_data_archivo_FS(tamanioStream,subir_a_FS(streamTotal, bloquesNecesarios));

	FILE* archivo = fopen(path, "w+");

	fwrite(dataArchivo->stream,strlen(dataArchivo->stream),1,archivo);

	dictionary_destroy(dataArchivo->diccionario);
	free(dataArchivo->stream);
	free(dataArchivo);
	fclose(archivo);
}

uint32_t subir_a_FS(char* stream, uint32_t bloquesNecesarios){
	uint32_t bloqueAnterior, bloqueInicial, offset = 0;
	uint32_t tamanioTotalAux = strlen(stream);

	for(int i=0;i<bloquesNecesarios;i++){

		t_bitarray* bitarray = leerBitmap();
		uint32_t bloqueLibre = obtener_bloque_libre(bitarray);

		if(i==0)
			bloqueInicial = bloqueLibre;

		if(bloqueLibre == -1){
			log_info(logger,"SINDICATO :: NO HAY BLOQUES LIBRES");
			return -1;
		}

		if(i!=0){
			char* pathArchivoBloque = path_bloque(bloqueAnterior);

			bloquear_mutex_bloque(bloqueAnterior);
			FILE* bloque = fopen(pathArchivoBloque, "a");

			fseek(bloque,-sizeof(uint32_t),SEEK_END);
			fwrite(&bloqueLibre, sizeof(uint32_t),1,bloque);

			fclose(bloque);
			desbloquear_mutex_bloque(bloqueAnterior);
			free(pathArchivoBloque);
		}

		char* pathArchivoBloque = path_bloque(bloqueLibre);

		bloquear_mutex_bloque(bloqueLibre);
		FILE* bloque = fopen(pathArchivoBloque, "w");
		uint32_t tamanioSubir = min(block_size-4,tamanioTotalAux);

		fwrite(stream+offset,tamanioSubir,1,bloque);

		tamanioTotalAux = tamanioTotalAux - tamanioSubir;
		offset += tamanioSubir;
		bloqueAnterior = bloqueLibre;

		fclose(bloque);
		desbloquear_mutex_bloque(bloqueLibre);
		free(pathArchivoBloque);
	}

	free(stream);
	return bloqueInicial;
}

char* bajar_datos_bloques(t_archivo_FS* archivoFS){
	int32_t bloquesNecesarios = archivoFS->tamanioTotal / (block_size-4);
	if(archivoFS->tamanioTotal % (block_size-4))
		bloquesNecesarios++;

	char* stream = malloc(archivoFS->tamanioTotal + 1);
	uint32_t bloqueActual, bloqueSiguiente,offset = 0;

	for(int i=0;i<bloquesNecesarios;i++){

		if(i == 0)
			bloqueActual = archivoFS->bloqueInicial;

		char* pathBloque = path_bloque(bloqueActual);

		bloquear_mutex_bloque(bloqueActual);
		FILE* bloque = fopen(pathBloque, "r");

		uint32_t tamanioBajar = min(block_size-4,archivoFS->tamanioTotal);


		fread(stream + offset, tamanioBajar, 1, bloque);
		if(bloquesNecesarios != i+1){
			fread(&bloqueSiguiente, sizeof(uint32_t), 1, bloque);
			archivoFS->tamanioTotal = archivoFS->tamanioTotal - tamanioBajar;
			offset = offset + tamanioBajar;
		}
		fclose(bloque);
		free(pathBloque);
		desbloquear_mutex_bloque(bloqueActual);
		bloqueActual = bloqueSiguiente;
	}
	dictionary_destroy(archivoFS->diccionario);
	free(archivoFS->stream);
	free(archivoFS);
	return stream;
}

uint32_t obtener_bloque_libre(t_bitarray* bitarray){
	for(int i=0;i<blocks;i++){
		if(bitarray_test_bit(bitarray, i) == 0){
			bitarray_set_bit(bitarray,i);
			subir_bitarray_FS(bitarray);
			return i;
		}
	}
	return -1;
}

t_bitarray* leerBitmap(){

	FILE* bitmap = fopen(ruta_bitmap,"r");
	fseek(bitmap,0,SEEK_SET);

	if(bitmap){
		uint32_t posicion=0;

		fread(bitarrayFS,blocks/8,1,bitmap);

		t_bitarray* bitarray = bitarray_create(bitarrayFS,blocks/8);

		fclose(bitmap);

		return bitarray;
	}

	return NULL;

}

obtener_restaurante_respuesta* de_consola_a_datos_restaurante(char** palabras_del_mensaje){

	obtener_restaurante_respuesta* datosRestaurante = malloc(sizeof(obtener_restaurante_respuesta));

	datosRestaurante->listaTamanioAfinidades = list_create();
	datosRestaurante->afinidad_cocineros = list_create();
	datosRestaurante->listaTamanioPlatos = list_create();
	datosRestaurante->platos = list_create();
	datosRestaurante->precio_platos = list_create();
	uint32_t aux;

	datosRestaurante->cantidad_cocineros = (uint32_t) atoi(palabras_del_mensaje[2]);
	char** posicion = string_get_string_as_array(palabras_del_mensaje[3]);
	datosRestaurante->posicion.posX = (uint32_t) atoi(posicion[0]);
	datosRestaurante->posicion.posY = (uint32_t) atoi(posicion[1]);
	string_iterate_lines(posicion, (void*) free);
	free(posicion);
	char** afinidades = string_get_string_as_array(palabras_del_mensaje[4]);
	for(int i = 0; afinidades[i]; i++){
		aux = strlen(afinidades[i]) + 1;
		list_add(datosRestaurante->listaTamanioAfinidades, aux);
		list_add(datosRestaurante->afinidad_cocineros, afinidades[i]);
	}
	char** platos = string_get_string_as_array(palabras_del_mensaje[5]);
	char** preciosPlatos = string_get_string_as_array(palabras_del_mensaje[6]);
	for(int i = 0; platos[i]; i++){
		aux = strlen(platos[i]) + 1;
		list_add(datosRestaurante->listaTamanioPlatos, aux);
		list_add(datosRestaurante->platos, platos[i]);
		aux = atoi(preciosPlatos[i]);
		list_add(datosRestaurante->precio_platos, aux);
	}
	datosRestaurante->cantidad_hornos = (uint32_t) atoi(palabras_del_mensaje[7]);

	return datosRestaurante;
}

t_receta_respuesta* de_consola_a_datos_receta(char** palabras_del_mensaje){
	t_receta_respuesta* datosReceta = malloc(sizeof(t_receta_respuesta));
	datosReceta->listaPasos = list_create();
	datosReceta->listaTamanioPasos = list_create();
	datosReceta->listaTiempoPasos=list_create();
	uint32_t aux;

	char** pasos = string_get_string_as_array(palabras_del_mensaje[2]);
	char** tiempoPasos = string_get_string_as_array(palabras_del_mensaje[3]);
	for(int i = 0; pasos[i]; i++){
		aux = strlen(pasos[i]) + 1;
		list_add(datosReceta->listaTamanioPasos, aux);
		list_add(datosReceta->listaPasos, pasos[i]);
		aux = (uint32_t) atoi(tiempoPasos[i]);
		list_add(datosReceta->listaTiempoPasos,aux);
		free(tiempoPasos[i]);
	}
	free(pasos);
	free(tiempoPasos);

	return datosReceta;
}

void crear_bitmap(char* pathMetada) {
	char* pathArchivoBitarray = string_new();
	bitarrayFS = malloc(blocks / 8);
	string_append_with_format(&pathArchivoBitarray, "%s/Bitmap.bin",pathMetada);
	ruta_bitmap = string_duplicate(pathArchivoBitarray);
	if (!archivo_existe(ruta_bitmap)) {
		FILE* bitmap = fopen(pathArchivoBitarray, "w+");

		for (int i = 0; i < blocks / 8; i++) {
			bitarrayFS[i] = '0';
		}
		t_bitarray* bitarray = bitarray_create(bitarrayFS, blocks / 8);
		for (int i = 0; i < blocks / 8; i++) {
			bitarray_clean_bit(bitarray, i);
		}

		fwrite(bitarray->bitarray, bitarray->size, 1, bitmap);

		free(bitarray);

		fclose(bitmap);
	}

	free(pathArchivoBitarray);

}

void subir_bitarray_FS(t_bitarray* bitarray){
	FILE* bitmap = fopen(ruta_bitmap,"w+");
	fwrite(bitarray->bitarray, bitarray->size, 1, bitmap);
	fclose(bitmap);
	bitarray_destroy(bitarray);
}

void crearBloques(char* path_bloques,char* path_metadata){
	
	ruta_bloques = string_duplicate(path_bloques);

	log_info(logger, "SINDICATO :: SE INICIALIZARAN LOS %d BLOQUES DE %d TAMANIO", blocks, block_size);

	for(int i = 1; i <= blocks; i++){
		char* pathArchivoBloque = string_new();
		string_append_with_format(&pathArchivoBloque, "%s", path_bloques);
		string_append_with_format(&pathArchivoBloque, "/%d.", i);
		string_append_with_format(&pathArchivoBloque, "%s", magic_number);

		if(!archivo_existe(pathArchivoBloque)){
			FILE* bloque = fopen(pathArchivoBloque, "w");
//			log_info(logger, "SE ROMPE ACA");
			fclose(bloque); // CON EL HELGRIND SE ROMPE ACA
//			log_info(logger, "YA SE ROMPIO");
		}

		free(pathArchivoBloque);
	}
}
// Deserializacion


t_restaurante* deserializar_consultar_platos(int socket_cliente){

	//------------ORDEN------------
			//1. Tamaño nombre
			//2. Nombre
	//-----------------------------

	t_restaurante* msg_recibido = malloc(sizeof(t_restaurante));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);

	return msg_recibido;
}

t_restaurante* deserializar_obtener_restaurante(int socket_cliente){

	//------------ORDEN------------
			//1. Tamaño Nombre
			//2. Nombre
	//-----------------------------

	t_restaurante* msg_recibido = malloc(sizeof(t_restaurante));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);

	log_info(logger, "Nombre resto: %s",msg_recibido->nombreRestaurante);

	return msg_recibido;
}

guardar_pedido* deserializar_guardar_pedido(int socket_cliente){

	//------------ORDEN------------
			//1. Tamaño Nombre
			//2. Nombre
			//3. ID Pedido
	//-----------------------------

	guardar_pedido* msg_recibido = malloc(sizeof(guardar_pedido));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);

	return msg_recibido;
}

guardar_plato* deserializar_guardar_plato(int socket_cliente){

	//------------ORDEN------------
			//1. Tamaño Nombre Restaurante
			//2. Nombre Restaurante
			//3. Tamaño Nombre Plato
			//4. Nombre Plato
			//5. Cantidad de Platos
			//6. ID Pedido
	//-----------------------------


	guardar_plato* msg_recibido = malloc(sizeof(guardar_plato));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);

	recv(socket_cliente, &(msg_recibido->tamanioNombrePlato), sizeof(uint32_t), 0);
	msg_recibido->nombrePlato = malloc(msg_recibido->tamanioNombrePlato);

	recv(socket_cliente, msg_recibido->nombrePlato, msg_recibido->tamanioNombrePlato, 0);

	recv(socket_cliente, &(msg_recibido->cantidad_platos), sizeof(uint32_t), 0);

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);

	return msg_recibido;
}

confirmar_pedido* deserializar_confirmar_pedido(int socket_cliente){

	//------------ORDEN------------
	//1. Id Pedido
	//2. Tamanio Nombre Restaurante
	//3. Nombre Restaurante;
	//-----------------------------

	confirmar_pedido* msg_recibido = malloc(sizeof(confirmar_pedido));

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);


	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);


	recv(socket_cliente, (msg_recibido->nombreRestaurante), msg_recibido->tamanioNombre, 0);

	return msg_recibido;
}

plato_listo* deserializar_plato_listo(int socket_cliente) {

	//------------ORDEN------------
			//1. Tamaño Nombre Restaurante
			//2. Nombre Restaurante
			//3. Tamaño Nombre Plato
			//4. Nombre Plato
			//5. ID Pedido
	//-----------------------------

	plato_listo* msg_recibido = malloc(sizeof(plato_listo));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);

	recv(socket_cliente, &(msg_recibido->tamanioNombrePlato), sizeof(uint32_t), 0);
	msg_recibido->nombrePlato = malloc(msg_recibido->tamanioNombrePlato);

	recv(socket_cliente, msg_recibido->nombrePlato, msg_recibido->tamanioNombrePlato, 0);

	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);

	return msg_recibido;
}

obtener_pedido* deserializar_obtener_pedido(int socket_cliente){

	//------------ORDEN------------
			//1. Tamaño Nombre
			//2. Nombre
			//3. ID Pedido
	//-----------------------------

	obtener_pedido* msg_recibido = malloc(sizeof(obtener_pedido));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);
	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);

	return msg_recibido;
}

terminar_pedido* deserializar_terminar_pedido(int socket_cliente){

	//------------ORDEN------------
			//1. Tamaño Nombre
			//2. Nombre
			//3. ID Pedido
	//-----------------------------

	terminar_pedido* msg_recibido = malloc(sizeof(terminar_pedido));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreRestaurante = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreRestaurante, msg_recibido->tamanioNombre, 0);
	recv(socket_cliente, &(msg_recibido->id_pedido), sizeof(uint32_t), 0);

	return msg_recibido;
}

obtener_receta* deserializar_obtener_receta(int socket_cliente){

	//------------ORDEN--------------
			//1. Tamaño Nombre Receta
			//2. Nombre Receta
	//-------------------------------

	obtener_receta* msg_recibido = malloc(sizeof(obtener_receta));

	recv(socket_cliente, &(msg_recibido->tamanioNombre), sizeof(uint32_t), 0);
	msg_recibido->nombreReceta = malloc(msg_recibido->tamanioNombre);

	recv(socket_cliente, msg_recibido->nombreReceta, msg_recibido->tamanioNombre, 0);

	return msg_recibido;
}

void serializar_restaurante_obtener_restaurante_respuesta(obtener_restaurante_respuesta* msg, t_buffer* buffer) {
	//------------ORDEN------------
	//1. Tamaño Nombre
	//2. Nombre
	//3. Cantidad cocineros
	//4. Posicion en X
	//5. Posicion en Y
	//6. Cantidad Afinidades
	//7. listaTamanioAfinidades } COMO SON LISTAS IRAN
	//8. afinidad_cocineros        }   DENTRO DE UN FOR
	//9. Cantidad platos
	//10. listaTamanioPlatos    } COMO SON LISTAS IRAN
	//11. platos                }   DENTRO DE UN FOR
	//12. precio_platos            }
	//13. cantidad_hornos
	//14. canntidadPedidods
	//-----------------------------

	uint32_t cantidadAfinidades = list_size(msg->afinidad_cocineros);
	uint32_t cantidadPlatos = list_size(msg->platos);

	buffer->size = 8 * sizeof(uint32_t) + msg->tamanioNombre;

	for (int i = 0; i < cantidadAfinidades; i++) {
		buffer->size += ((uint32_t) list_get(msg->listaTamanioAfinidades, i)) + sizeof(uint32_t);
	}
	for (int i = 0; i < cantidadPlatos; i++) {
		buffer->size += ((uint32_t) list_get(msg->listaTamanioPlatos, i)) + 2 * sizeof(uint32_t);
	}
	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, msg->nombre_restaurante, msg->tamanioNombre);
	offset += msg->tamanioNombre;

	memcpy(buffer->stream + offset, &(msg->cantidad_cocineros), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->posicion.posX), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->posicion.posY), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(cantidadAfinidades), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	uint32_t aux;
	for (int i = 0; i < cantidadAfinidades; i++) {

		aux = (uint32_t) list_get(msg->listaTamanioAfinidades, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, (char*) list_get(msg->afinidad_cocineros, i), aux);
		offset += aux;
	}

	memcpy(buffer->stream + offset, &(cantidadPlatos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < cantidadPlatos; i++) {

		aux = (uint32_t) list_get(msg->listaTamanioPlatos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, (char*) list_get(msg->platos, i), aux);
		offset += aux;

		aux = (uint32_t) list_get(msg->precio_platos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	memcpy(buffer->stream + offset, &(msg->cantidad_hornos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->cantidadPedidos), sizeof(uint32_t));
}


void serializar_datos_restaurante_FS(obtener_restaurante_respuesta* msg, t_buffer* buffer) {
	//------------ORDEN------------
	//3. Cantidad cocineros
	//4. Posicion en X
	//5. Posicion en Y
	//6. Cantidad Afinidades
	//7. listaTamanioAfinidades } COMO SON LISTAS IRAN
	//8. afinidad_cocineros        }   DENTRO DE UN FOR
	//9. Cantidad platos
	//10. listaTamanioPlatos    } COMO SON LISTAS IRAN
	//11. platos                }   DENTRO DE UN FOR
	//12. precio_platos            }
	//13. cantidad_hornos
	//14. canntidadPedidods
	//-----------------------------

	uint32_t cantidadAfinidades = list_size(msg->afinidad_cocineros);
	uint32_t cantidadPlatos = list_size(msg->platos);

	buffer->size = 7 * sizeof(uint32_t) ;

	for (int i = 0; i < cantidadAfinidades; i++) {
		buffer->size += ((uint32_t) list_get(msg->listaTamanioAfinidades, i)) + sizeof(uint32_t);
	}
	for (int i = 0; i < cantidadPlatos; i++) {
		buffer->size += ((uint32_t) list_get(msg->listaTamanioPlatos, i)) + 2 * sizeof(uint32_t);
	}
	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->cantidad_cocineros), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->posicion.posX), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->posicion.posY), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(cantidadAfinidades), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	uint32_t aux;
	for (int i = 0; i < cantidadAfinidades; i++) {

		aux = (uint32_t) list_get(msg->listaTamanioAfinidades, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, (char*) list_get(msg->afinidad_cocineros, i), aux);
		offset += aux;
	}

	memcpy(buffer->stream + offset, &(cantidadPlatos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < cantidadPlatos; i++) {

		aux = (uint32_t) list_get(msg->listaTamanioPlatos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, (char*) list_get(msg->platos, i), aux);
		offset += aux;

		aux = (uint32_t) list_get(msg->precio_platos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	memcpy(buffer->stream + offset, &(msg->cantidad_hornos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->cantidadPedidos), sizeof(uint32_t));
}

void enviar_obtener_restaurante_FAIL(uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(RESPUESTA);

	paquete_a_enviar->buffer->size = sizeof(uint32_t);

	paquete_a_enviar->buffer->stream = malloc(paquete_a_enviar->buffer->size);

	uint32_t aux = -1;

    memcpy(paquete_a_enviar->buffer->stream, &(aux), sizeof(uint32_t));

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_restaurante_obtener_restaurante_respuesta(obtener_restaurante_respuesta* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_RESTAURANTE_RESPUESTA);

	serializar_restaurante_obtener_restaurante_respuesta(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_restaurante_obtener_pedido_respuesta(obtener_pedido_respuesta* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_PEDIDO_RESPUESTA);

	serializar_restaurante_obtener_pedido_respuesta(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_restaurante_obtener_pedido_respuesta(obtener_pedido_respuesta* msg, t_buffer* buffer) {
	//------------ORDEN------------
			//1. Estado
			//2. Cantidad de platos
			//3. Tamanio Plato  }
			//3. Platos         } COMO SON LISTAS IRAN
			//4. Cant x Platos  }   DENTRO DE UN FOR
			//5. Cant Lista     }
			//6. Precio Total
	//-----------------------------

	uint32_t cant;
	if(msg->platos)
		cant = list_size(msg->platos);
	else
		cant = 0;

	buffer->size = sizeof(estado_pedido) + sizeof(uint32_t)*cant*3 + sizeof(uint32_t);

	for (int i = 0; i < cant; i++) {
		buffer->size += (uint32_t) list_get(msg->listaTamanioPlato, i);
	}

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(msg->estado), sizeof(estado_pedido));
	offset += sizeof(estado_pedido);

	memcpy(buffer->stream + offset, &(cant), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < cant; i++) {
		uint32_t aux;

		aux = (uint32_t) list_get(msg->listaTamanioPlato, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		char* auxPlato = (char*) list_get(msg->platos, i);
		memcpy(buffer->stream + offset, auxPlato, aux);
		offset += aux;

		aux = (uint32_t) list_get(msg->listaCantidadDePedido, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		aux = (uint32_t) list_get(msg->listaCantidadLista, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

//	memcpy(buffer->stream + offset, &(msg->precioTotal), sizeof(uint32_t));
}

void enviar_respuesta(t_respuesta2 confirmado, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(RESPUESTA);

	paquete_a_enviar->buffer->size = sizeof(t_respuesta2);

	paquete_a_enviar->buffer->stream = malloc(paquete_a_enviar->buffer->size);

    memcpy(paquete_a_enviar->buffer->stream, &(confirmado), sizeof(t_respuesta2));

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_restaurante_consultar_platos_respuesta(consultar_platos_respuesta* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(CONSULTAR_PLATOS);

	serializar_restaurante_consultar_platos_respuesta(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void enviar_obtener_receta_respuesta(t_receta_respuesta* msg, uint32_t socketConexion){
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_RECETA);

	serializar_restaurante_obtener_receta_respuesta(msg, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_restaurante_consultar_platos_respuesta(consultar_platos_respuesta* msg, t_buffer* buffer){

	//------------ORDEN------------
	//1. Cantidad platos
	//2. listaTamanioPlatos    } COMO SON LISTAS IRAN
	//3. platos                }   DENTRO DE UN FOR
	//4. precio_platos         }
	//-----------------------------

	uint32_t cantidadPlatos = list_size(msg->platos);
	uint32_t offset = 0;
	uint32_t aux;

	buffer->size = sizeof(uint32_t);

	for (int i = 0; i < cantidadPlatos; i++) {
		buffer->size += ((uint32_t) list_get(msg->listaTamanioPlatos, i)) + 2 * sizeof(uint32_t);
	}

	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(cantidadPlatos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < cantidadPlatos; i++) {
		aux = (uint32_t) list_get(msg->listaTamanioPlatos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, list_get(msg->platos, i), aux);
		offset += aux;

		aux = (uint32_t) list_get(msg->precio_platos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

}


void enviar_restaurante_obtener_receta_respuesta(t_receta_respuesta* obtenerRecetaRespuesta, uint32_t socketConexion) {
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_RECETA);

	serializar_restaurante_obtener_receta_respuesta(obtenerRecetaRespuesta, paquete_a_enviar->buffer);

	int tamanio_a_enviar = 0;

	void* a_enviar = serializar_paquete(paquete_a_enviar, &tamanio_a_enviar);

	send(socketConexion, a_enviar, tamanio_a_enviar, 0);

	free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
	free(a_enviar);
}

void serializar_restaurante_obtener_receta_respuesta(t_receta_respuesta* msg, t_buffer* buffer){

	//------------ORDEN------------
	//1. Cantidad pasos
	//2. listaTamanioPasos	   		 } COMO SON LISTAS IRAN
	//3. listaPasos            	   	 }   DENTRO DE UN FOR
	//4. listaTiempoPasos     		 }
	//-----------------------------

	uint32_t cantidadPasos= list_size(msg->listaPasos);
	uint32_t offset = 0;
	uint32_t aux;

	buffer->size = sizeof(uint32_t);

	for (int i = 0; i < cantidadPasos; i++) {
		buffer->size += ((uint32_t) list_get(msg->listaTamanioPasos, i)) + 2 * sizeof(uint32_t);
	}

	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream, &(cantidadPasos), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < cantidadPasos; i++) {
		aux = (uint32_t) list_get(msg->listaTamanioPasos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, list_get(msg->listaPasos, i), aux);
		offset += aux;

		aux = (uint32_t) list_get(msg->listaTiempoPasos, i);
		memcpy(buffer->stream + offset, &(aux), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

}

bool archivo_existe(char* nombreArchivo){

	if (access(nombreArchivo, F_OK) != 0){
		//printf("'%s' no existe\n", nombreArchivo);
		return false;
	}
	else {
		//printf("'%s' existe\n", nombreArchivo);
		return true;
	}

}

char* path_bloque(uint32_t bloque){
	char* pathArchivoBloque = string_new();
	string_append_with_format(&pathArchivoBloque, "%s", path_bloques);
	string_append_with_format(&pathArchivoBloque, "/%d.",bloque);
	string_append_with_format(&pathArchivoBloque, "%s", magic_number);

	return pathArchivoBloque;
}


char* path_info_restaurante(char* nombreRestaurante){
	char* pathCompleto = string_new();
	char* path = crear_path_restaurante(nombreRestaurante);
	string_append_with_format(&pathCompleto, "%s/Info.AFIP",path);
	free(path);

	return pathCompleto;
}


char* path_pedido_restaurante(char* restaurante, uint32_t pedido){
	char* pathCompleto = string_new();
	char* path = crear_path_restaurante(restaurante);
	string_append_with_format(&pathCompleto, "%s/pedido",path);
	string_append_with_format(&pathCompleto, "%d.AFIP",pedido);
	free(path);

	return pathCompleto;
}

obtener_restaurante_respuesta* obtener_restaurante_FS(t_restaurante* obtenerRestauranteRecibido){

//	1. Verificar si el Restaurante existe dentro del File System.
//		Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante.
//		En caso de no existir se deberá informar dicha situación.
//	2. Obtener todos los datos del archivo info.AFIP.
//	3. Responder el mensaje indicando los datos del Restaurante.

	char* pathCompleto = path_info_restaurante(obtenerRestauranteRecibido->nombreRestaurante);

	FILE* restauranteAfip = fopen(pathCompleto, "r");

	if(restauranteAfip){
		log_info(logger, "SINDICATO :: EL RESTAURANTE SE ENCUENTRA EN AFIP");

		t_archivo_FS* archivoFS = leer_archivo(restauranteAfip);
		char* stream = bajar_datos_bloques(archivoFS);
		obtener_restaurante_respuesta* msg = deserializar_restaurante_FS(stream);

		msg->tamanioNombre = obtenerRestauranteRecibido->tamanioNombre;
		msg->nombre_restaurante = string_duplicate(obtenerRestauranteRecibido->nombreRestaurante);
		fclose(restauranteAfip);

		log_info(logger, "SINDICATO :: SE OBTIENEN LOS DATOS DEL RESTAURANTE");
		free(pathCompleto);
		return msg;
	}

	log_info(logger, "SINDICATO :: EL RESTAURANTE NO SE ENCUENTRA EN AFIP");
	free(pathCompleto);
	free(restauranteAfip);
	return NULL;
}

t_archivo_FS* leer_archivo(FILE* archivo){
	char* leido = malloc(1);
	int i=0;

	while(fread(leido+i,1,1,archivo)){
		i++;
		leido= (char*) realloc(leido,i+1);
	}
	leido= (char*) realloc(leido,i+1);
	t_bloque_FS* bloqueAux = crear_bloque(leido,0);
	free(leido);

	t_archivo_FS* archivoFS = crear_data_archivo_FS(obtener_dato_int(bloqueAux,"SIZE"),obtener_dato_int(bloqueAux,"INITIAL_BLOCK"));

	dictionary_destroy(bloqueAux->diccionario);
	free(bloqueAux->stream);
	free(bloqueAux);
	return archivoFS;
}

obtener_restaurante_respuesta* de_archivo_a_datos_restaurante(char** palabras_del_mensaje){

	obtener_restaurante_respuesta* datosRestaurante = malloc(sizeof(obtener_restaurante_respuesta));

	datosRestaurante->listaTamanioAfinidades = list_create();
	datosRestaurante->afinidad_cocineros = list_create();
	datosRestaurante->listaTamanioPlatos = list_create();
	datosRestaurante->platos = list_create();
	datosRestaurante->precio_platos = list_create();
	uint32_t aux;

	datosRestaurante->cantidad_cocineros = (uint32_t) atoi(palabras_del_mensaje[0]);
	char** posicion = string_get_string_as_array(palabras_del_mensaje[1]);
	datosRestaurante->posicion.posX = (uint32_t) atoi(posicion[0]);
	datosRestaurante->posicion.posY = (uint32_t) atoi(posicion[1]);
	char** afinidades = string_get_string_as_array(palabras_del_mensaje[2]);
	for(int i = 0; afinidades[i]; i++){
		aux = strlen(afinidades[i]) + 1;
		list_add(datosRestaurante->listaTamanioAfinidades, aux);
		list_add(datosRestaurante->afinidad_cocineros, afinidades[i]);
	}
	char** platos = string_get_string_as_array(palabras_del_mensaje[3]);
	char** preciosPlatos = string_get_string_as_array(palabras_del_mensaje[4]);
	for(int i = 0; platos[i]; i++){
		aux = strlen(platos[i]) + 1;
		list_add(datosRestaurante->listaTamanioPlatos, aux);
		list_add(datosRestaurante->platos, platos[i]);
		aux = atoi(preciosPlatos[i]);
		list_add(datosRestaurante->precio_platos, aux);
	}
	datosRestaurante->cantidad_hornos = (uint32_t) atoi(palabras_del_mensaje[5]);
	datosRestaurante->cantidadPedidos = (uint32_t) atoi(palabras_del_mensaje[6]);

	return datosRestaurante;
}

obtener_restaurante_respuesta* deserializar_obtener_restaurante_FS(t_buffer* buffer){
	//------------ORDEN------------
	//1. Cantidad cocineros
	//2. Posicion en X
	//3. Posicion en Y
	//4. Cantidad Afinidades
	//5. listaTamanioAfinidades 	} COMO SON LISTAS IRAN
	//6. afinidad_cocineros         }   DENTRO DE UN FOR
	//7. Cantidad platos
	//8. listaTamanioPlatos    		} COMO SON LISTAS IRAN
	//9. platos               		}   DENTRO DE UN FOR
	//10. precio_platos         	}
	//11. cantidad_hornos
	//12.  cantidadPedidos
	//-----------------------------

	uint32_t cantidadAfinidades;
	uint32_t cantidadPlatos;
	uint32_t aux;
	int offset = 0;

	obtener_restaurante_respuesta* msg = malloc(sizeof(obtener_restaurante_respuesta));
	msg->afinidad_cocineros = list_create();
	msg->listaTamanioAfinidades = list_create();
	msg->platos = list_create();
	msg->listaTamanioPlatos = list_create();
	msg->precio_platos = list_create();


	memcpy(&(msg->cantidad_cocineros), buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&(msg->posicion.posX), buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&(msg->posicion.posY), buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&(cantidadAfinidades), buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < cantidadAfinidades; ++i) {

		memcpy(&(aux), buffer->stream + offset, sizeof(uint32_t));
		list_add(msg->listaTamanioAfinidades, aux);
		offset += sizeof(uint32_t);

		char* auxAfinidades = malloc(aux);
		memcpy(auxAfinidades, buffer->stream + offset, aux);
		list_add(msg->afinidad_cocineros, auxAfinidades);
		offset += aux;
	}

	memcpy(&(cantidadPlatos), buffer->stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < cantidadPlatos; ++i) {

		memcpy(&(aux), buffer->stream + offset, sizeof(uint32_t));
		list_add(msg->listaTamanioPlatos, aux);
		offset += sizeof(uint32_t);

		char* auxPlatos = malloc(aux);
		memcpy(auxPlatos, buffer->stream + offset, aux);
		list_add(msg->platos, auxPlatos);
		offset += aux;

		memcpy(&(aux), buffer->stream + offset, sizeof(uint32_t));
		list_add(msg->precio_platos, aux);
		offset += sizeof(uint32_t);
	}

	memcpy(&(msg->cantidad_hornos), buffer->stream + offset, sizeof(uint32_t));

	memcpy(&(msg->cantidadPedidos), buffer->stream + offset, sizeof(uint32_t));

	return msg;
}

consultar_platos_respuesta* consultar_platos_FS(t_restaurante* consultarPlatosRecibido){

//	1. Verificar si el Restaurante existe dentro del File System.
//		Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante.
//		En caso de no existir se deberá informar dicha situación.
//	2. Obtener los platos que puede preparar dicho Restaurante del archivo info.AFIP.
//	3. Responder el mensaje indicando los platos que puede preparar el Restaurante.

	obtener_restaurante_respuesta* datosRestaurante = obtener_restaurante_FS(consultarPlatosRecibido);

	if(datosRestaurante){
		consultar_platos_respuesta* msg = obtener_platos_de_datos(datosRestaurante);

		log_info(logger, "SINDICATO :: SE OBTUVIERON LOS PLATOS DEL RESTAURANTE");

		list_destroy(datosRestaurante->afinidad_cocineros);
		list_destroy(datosRestaurante->listaTamanioAfinidades);
		list_destroy(datosRestaurante->listaTamanioPlatos);
		list_destroy(datosRestaurante->platos);
		list_destroy(datosRestaurante->precio_platos);
		free(datosRestaurante->nombre_restaurante);
		free(datosRestaurante);

		return msg;
	}

	log_info(logger, "SINDICATO :: NO SE OBTUVIERON LOS PLATOS DEL RESTAURANTE");
	consultar_platos_respuesta* sinPlatos = malloc(sizeof(consultar_platos_respuesta));
	sinPlatos->listaTamanioPlatos = list_create();
	sinPlatos->platos = list_create();
	sinPlatos->precio_platos = list_create();

	list_destroy(datosRestaurante->afinidad_cocineros);
	list_destroy(datosRestaurante->listaTamanioAfinidades);
	list_destroy(datosRestaurante->listaTamanioPlatos);
	list_destroy(datosRestaurante->platos);
	list_destroy(datosRestaurante->precio_platos);
	free(datosRestaurante->nombre_restaurante);
	free(datosRestaurante);

	return sinPlatos;
}

consultar_platos_respuesta* obtener_platos_de_datos(obtener_restaurante_respuesta* datosRestaurante){

	consultar_platos_respuesta* msg = (consultar_platos_respuesta*) malloc(sizeof(obtener_restaurante_respuesta));
	msg->listaTamanioPlatos = list_duplicate(datosRestaurante->listaTamanioPlatos);
	msg->precio_platos = list_duplicate(datosRestaurante->precio_platos);
	msg->platos = list_duplicate(datosRestaurante->platos);

	return msg;
}

void agregar_pedido_a_restaurante(char* nombreRestaurante){
	char* pathCompleto = path_info_restaurante(nombreRestaurante);

	FILE* restauranteAfip = fopen(pathCompleto, "r");

	t_archivo_FS* archivoFS = leer_archivo(restauranteAfip);
	char* stream = bajar_datos_bloques_y_liberarlos(archivoFS);
	obtener_restaurante_respuesta* datosRestaurante = deserializar_restaurante_FS(stream);

	datosRestaurante->tamanioNombre = strlen(nombreRestaurante)+1;
	datosRestaurante->nombre_restaurante = string_duplicate(nombreRestaurante);
	fclose(restauranteAfip);

	datosRestaurante->cantidadPedidos++;

	subir_restaurante_bloque(datosRestaurante,pathCompleto);

	list_destroy(datosRestaurante->afinidad_cocineros);
	list_destroy(datosRestaurante->listaTamanioAfinidades);
	list_destroy(datosRestaurante->listaTamanioPlatos);
	list_destroy(datosRestaurante->platos);
	list_destroy(datosRestaurante->precio_platos);
	free(datosRestaurante->nombre_restaurante);
	free(datosRestaurante);
	free(pathCompleto);
}

t_respuesta2 crear_archivo_pedido(guardar_pedido* nuevoPedido){
	char* pathRestaurante = crear_path_restaurante(nuevoPedido->nombreRestaurante);
	obtener_pedido_respuesta* pedidoNuevo = dar_nuevo_pedido();
	if(directorio_existe(pathRestaurante)){
		free(pathRestaurante);

		log_info(logger, "SINDICATO :: EL RESTAURANTE %s SE ENCUENTRA EN AFIP",nuevoPedido->nombreRestaurante);

		char* pathPedidoNuevo = path_pedido_restaurante(nuevoPedido->nombreRestaurante, nuevoPedido->id_pedido);

		if(!archivo_existe(pathPedidoNuevo)){ 

			subir_pedido_bloque(pedidoNuevo, pathPedidoNuevo);

			agregar_pedido_a_restaurante(nuevoPedido->nombreRestaurante);

			log_info(logger, "SINDICATO :: SE CREO EL NUEVO PEDIDO %d",nuevoPedido->id_pedido);
			free(pathPedidoNuevo);

			return OK2;

		}
		else{
			log_info(logger, "SINDICATO :: EL PEDIDO %d YA ESTA CREADO",nuevoPedido->id_pedido);
			free(pathPedidoNuevo);
			return OK2;
		}
	}

	log_info(logger, "SINDICATO :: EL RESTAURANTE NO SE ENCUENTRA EN AFIP");
	log_info(logger, "SINDICATO :: NO SE CREO EL NUEVO PEDIDO %d",nuevoPedido->id_pedido);

	return FAIL2;
}

t_respuesta2 confirmar_pedido_FS(confirmar_pedido* confirmarPedidoRecibido){

//	Este mensaje cumplirá la función de cambiar el estado de un determinado pedido. Para esto recibirá:
//	El nombre del Restaurante
//	El ID del pedido
//	Al recibir este mensaje se deberán realizar las siguientes operaciones:
//	1. Verificar si el Restaurante existe dentro del File System. Para esto se deberá buscar
//		dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante.
//		En caso de no existir se deberá informar dicha situación.
//	2. Verificar si el Pedido existe dentro del File System. Para esto se deberá buscar dentro
//		del directorio del Restaurante si existe dicho pedido. En caso de no existir se deberá
//		informar dicha situación.
//	3. Verificar que el pedido esté en estado “Pendiente”. En caso contrario se deberá informar dicha
//		situación.
//	4. Cambiar el estado del pedido de “Pendiente” a “Confirmado” (se debe truncar el archivo en caso
//		de ser necesario).
//	5. Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).

	char* pathRestaurante = path_info_restaurante(confirmarPedidoRecibido->nombreRestaurante);
	if(directorio_existe(pathRestaurante)){
		free(pathRestaurante);

		log_info(logger, "SINDICATO :: EL RESTAURANTE SE ENCUENTRA EN AFIP");

		char* pathCompleto = path_pedido_restaurante(confirmarPedidoRecibido->nombreRestaurante, confirmarPedidoRecibido->id_pedido);

		if(archivo_existe(pathCompleto)){
			FILE* pedido = fopen(pathCompleto, "r");

			t_archivo_FS* archivoFS = leer_archivo(pedido);
			fclose(pedido);

			char* stream = bajar_datos_bloques_y_liberarlos(archivoFS);

			obtener_pedido_respuesta* pedidoObtenido = deserializar_pedido_FS(stream);

			if(pedidoObtenido->estado!=PENDIENTE){
				log_info(logger, "SINDICATO :: El pedido %d no se encuentra en estado pendiente",confirmarPedidoRecibido->id_pedido);

				list_destroy(pedidoObtenido->listaCantidadDePedido);
				list_destroy(pedidoObtenido->listaCantidadLista);
				list_destroy(pedidoObtenido->listaTamanioPlato);
				list_destroy(pedidoObtenido->platos);
				free(pedidoObtenido);

				return FAIL2;
			}
			else{
				pedidoObtenido->estado=CONFIRMADO;

				subir_pedido_bloque(pedidoObtenido, pathCompleto);

				log_info(logger, "SINDICATO :: SE CONFIRMO EL PEDIDO %d",confirmarPedidoRecibido->id_pedido);

				list_destroy(pedidoObtenido->listaCantidadDePedido);
				list_destroy(pedidoObtenido->listaCantidadLista);
				list_destroy(pedidoObtenido->listaTamanioPlato);
				list_destroy(pedidoObtenido->platos);
				free(pedidoObtenido);

				return OK2;
			}
		}
		else{
			log_info(logger, "SINDICATO :: NO EXISTE EL PEDIDO");
			return FAIL2;
		}
	}

	log_info(logger, "SINDICATO :: EL RESTAURANTE NO SE ENCUENTRA EN AFIP");
	log_info(logger, "SINDICATO :: NO SE CONFIRMO EL PEDIDO %d",confirmarPedidoRecibido->id_pedido);

	return FAIL2;
}

t_respuesta2 plato_listo_FS(plato_listo* platoListoRecibido){

//	1. Verificar si el Restaurante existe dentro del File System.
//		Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante.
//		En caso de no existir se deberá informar dicha situación.
//	2. Verificar si el Pedido existe dentro del File System.
//		Para esto se deberá buscar dentro del directorio del Restaurante si existe dicho pedido.
//		En caso de no existir se deberá informar dicha situación.
//	3. Verificar que el pedido esté en estado “Confirmado”. En caso contrario se deberá informar dicha situación.
//	4. Verificar si ese plato ya existe dentro del archivo.
//		En caso de existir, se deberá aumentar en uno la cantidad lista de ese plato.
//		En caso contrario se deberá informar dicha situación.
//	5. Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).

	char* pathRestaurante = path_info_restaurante(platoListoRecibido->nombreRestaurante);
	t_respuesta2 platoListo;
		if(directorio_existe(pathRestaurante)){
			free(pathRestaurante);

			log_info(logger, "SINDICATO :: EL RESTAURANTE SE ENCUENTRA EN AFIP");

			char* pathCompleto = path_pedido_restaurante(platoListoRecibido->nombreRestaurante, platoListoRecibido->id_pedido);

			if(archivo_existe(pathCompleto)){

				FILE* pedido = fopen(pathCompleto, "r");
				t_archivo_FS* archivoFS = leer_archivo(pedido);
				fclose(pedido);

				char* stream = bajar_datos_bloques_y_liberarlos(archivoFS);
				obtener_pedido_respuesta* pedidoObtenido = deserializar_pedido_FS(stream);
				if(pedidoObtenido->estado!=CONFIRMADO){
					log_info(logger, "SINDICATO :: El pedido %d no se encuentra en estado Confirmado",platoListoRecibido->id_pedido);
					platoListo = FAIL2;
				}
				else
					platoListo = verificar_plato_listo(pedidoObtenido,platoListoRecibido);

				subir_pedido_bloque(pedidoObtenido, pathCompleto);
				list_destroy(pedidoObtenido->listaCantidadDePedido);
				list_destroy(pedidoObtenido->listaCantidadLista);
				list_destroy(pedidoObtenido->listaTamanioPlato);
				list_destroy(pedidoObtenido->platos);
				free(pedidoObtenido);
				free(pathCompleto);

				if(platoListo == OK2)
					log_info(logger, "SINDICATO :: SE ANIADIO UN PLATO LISTO: %s PARA EL PEDIDO %d",platoListoRecibido->nombrePlato,platoListoRecibido->id_pedido);
				else
					log_info(logger, "SINDICATO :: NO SE PUEDO PONER EN LISTO EL PLATO %s EN EL PEDIDO %d",platoListoRecibido->nombrePlato,platoListoRecibido->id_pedido);

				return platoListo;
			}
			log_info(logger, "SINDICATO :: NO EXISTE EL PEDIDO");
			return FAIL2;
		}

		log_info(logger, "SINDICATO :: EL RESTAURANTE NO SE ENCUENTRA EN AFIP");
		log_info(logger, "SINDICATO :: NO SE ANIADIO EL PLATO LISTO %s",platoListoRecibido->nombrePlato);

		return FAIL2;
}

t_receta_respuesta* obtener_receta_FS(obtener_receta* obtenerRecetaRespuesta){
//	1. Verificar si existe el plato dado dentro del directorio de recetas.
//		En caso de no existir, se deberá informar dicha situación.
//	2. Responder el mensaje con la receta solicitada.

	char* pathReceta = crear_path_receta(obtenerRecetaRespuesta->nombreReceta);

	if(archivo_existe(pathReceta)){
		log_info(logger, "SINDICATO :: LA RECETA SE ENCUENTRA EN AFIP");

		FILE* receta = fopen(pathReceta, "r");
		t_archivo_FS* archivoFS = leer_archivo(receta);
		char* stream = bajar_datos_bloques(archivoFS);
		t_receta_respuesta* msg = deserializar_receta_FS(stream);
		fclose(receta);
		free(pathReceta);
		log_info(logger, "SINDICATO :: SE OBTUVO LA RECETA Y EL PASO 1 TIENE TAMANIO DE %d", list_get(msg->listaTamanioPasos,0));
		return msg;
	}
	else{
		free(pathReceta);
		log_info(logger,"SINDICATO :: NO EXISTE LA RECETA");
	}

	return NULL;
}

void subir_pedido_restaurante(obtener_pedido_respuesta* msg, FILE* archivo) {
	//------------ORDEN------------
		//1. estado
		//2. platos         			}COMO SON LISTAS IRAN
		//3. listaCantidadDePedido    	}  DENTRO DE UN FOR
		//4. listaCantidadLista			}
		//5. precioTotal
	//-----------------------------
	uint32_t cantPlatos = (uint32_t) list_size(msg->platos);

	fprintf(archivo, "%s", dar_estado_pedido(msg->estado));

	fprintf(archivo, " [");
	for(int i=0;i<cantPlatos;i++){
		fprintf(archivo, "%s", (char*) list_get(msg->platos, i));
		if(i!=cantPlatos-1)
			fprintf(archivo, ",");
	}
	fprintf(archivo, "]");
	fprintf(archivo, " [");
	for(int i=0;i<cantPlatos;i++){
		fprintf(archivo, "%d", (uint32_t) list_get(msg->listaCantidadDePedido, i));
		if(i!=cantPlatos-1)
			fprintf(archivo, ",");
	}
	fprintf(archivo, "]");
	fprintf(archivo, " [");
	for(int i=0;i<cantPlatos;i++){
		fprintf(archivo, "%d", (uint32_t) list_get(msg->listaCantidadLista, i));
		if(i!=cantPlatos-1)
			fprintf(archivo, ",");
	}
	fprintf(archivo, "]");

	fprintf(archivo, " %d", (msg->precioTotal));

}


t_respuesta2 agregar_plato_a_pedido(guardar_plato* guardarPlatoRecibido){
	//guardar plato
//			1-Verificar si el Restaurante existe dentro del File System.
//				Para esto se deberá buscar dentro del directorio Restaurantes
//				si existe un subdirectorio con el nombre del Restaurante.
//				En caso de no existir se deberá informar dicha situación.
//			2-Verificar si el Pedido existe dentro del File System.
//				Para esto se deberá buscar dentro del directorio del Restaurante
//				si existe dicho pedido.
//				En caso de no existir se deberá informar dicha situación.
//			3-Verificar que el pedido esté en estado “Pendiente”.
//				En caso contrario se deberá informar dicha situación.
//			4-Verificar si ese plato ya existe dentro del archivo.
//				En caso de existir, se deberán agregar la cantidad pasada
//				por parámetro a la actual.
//				En caso de no existir se deberá agregar el plato a la lista de Platos
//				y anexar la cantidad que se tiene que cocinar de dicho plato y
//				aumentar el precio total del pedido.
//			5-Responder el mensaje indicando si se pudo realizar la operación
//				correctamente (Ok/Fail).

	char* pathRestaurante = path_info_restaurante(guardarPlatoRecibido->nombreRestaurante);
	if(directorio_existe(pathRestaurante)){
		free(pathRestaurante);
		log_info(logger, "SINDICATO :: EL RESTAURANTE SE ENCUENTRA EN AFIP");

		char* pathCompleto = path_pedido_restaurante(guardarPlatoRecibido->nombreRestaurante, guardarPlatoRecibido->id_pedido);

		if(archivo_existe(pathCompleto)){
			FILE* pedido = fopen(pathCompleto, "r");
			t_archivo_FS* archivoFS = leer_archivo(pedido);
			char* streamAux = bajar_datos_bloques_y_liberarlos(archivoFS);
			obtener_pedido_respuesta* msg = deserializar_pedido_FS(streamAux);

			fclose(pedido);
			if(msg->estado==PENDIENTE){

				agregar_al_pedido_plato(msg,guardarPlatoRecibido);

				subir_pedido_bloque(msg,pathCompleto);

				log_info(logger, "SINDICATO :: SE AGREGO EL PLATO AL PEDIDO");

				list_destroy(msg->listaCantidadDePedido);
				list_destroy(msg->listaCantidadLista);
				list_destroy(msg->listaTamanioPlato);
				list_destroy(msg->platos);
				free(msg);
				free(pathCompleto);

				return OK2;
			}
			else{
				list_destroy(msg->listaCantidadDePedido);
				list_destroy(msg->listaCantidadLista);
				list_destroy(msg->listaTamanioPlato);
				list_destroy(msg->platos);
				free(msg);
				free(pathCompleto);
				log_info(logger, "SINDICATO :: EL PEDIDO %d NO ESTA EN ESTADO PENDIENTE",guardarPlatoRecibido->id_pedido);
				return FAIL2;
			}
		}
		else{
			free(pathCompleto);
			log_info(logger, "SINDICATO :: EL PEDIDO %d NO EXISTE",guardarPlatoRecibido->id_pedido);
			return FAIL2;
		}
	}

	log_info(logger, "SINDICATO :: EL RESTAURANTE NO SE ENCUENTRA EN AFIP");

	return FAIL2;
}

char* bajar_datos_bloques_y_liberarlos(t_archivo_FS* archivoFS){

	int32_t bloquesNecesarios = archivoFS->tamanioTotal / (block_size-4);
	if(archivoFS->tamanioTotal % (block_size-4))
		bloquesNecesarios++;

	char* stream = malloc(archivoFS->tamanioTotal+1);
	uint32_t bloqueActual,offset = 0;

	for(int i=0;i<bloquesNecesarios;i++){

		if(i == 0)
			bloqueActual = archivoFS->bloqueInicial;

		t_bitarray* bitarray = leerBitmap();
		bitarray_clean_bit(bitarray,bloqueActual);
		subir_bitarray_FS(bitarray);

		char* pathBloque = path_bloque(bloqueActual);

		FILE* bloque = fopen(pathBloque, "r");

		uint32_t tamanioBajar = min(block_size-4,archivoFS->tamanioTotal);


		fread(stream + offset, tamanioBajar, 1, bloque);
		if(bloquesNecesarios != i+1){
			fread(&bloqueActual, sizeof(uint32_t), 1, bloque);
			archivoFS->tamanioTotal = archivoFS->tamanioTotal - tamanioBajar;
			offset = offset + tamanioBajar;
		}
		fclose(bloque);
		free(pathBloque);
	}
	dictionary_destroy(archivoFS->diccionario);
	free(archivoFS->stream);
	free(archivoFS);
	return stream;
}

t_respuesta2 terminar_pedido_FS(terminar_pedido* terminarPedidoRecibido){
	char* pathRestaurante = path_info_restaurante(terminarPedidoRecibido->nombreRestaurante);
		if(directorio_existe(pathRestaurante)){
			free(pathRestaurante);

			log_info(logger, "SINDICATO :: EL RESTAURANTE SE ENCUENTRA EN AFIP");

			char* pathCompleto = path_pedido_restaurante(terminarPedidoRecibido->nombreRestaurante, terminarPedidoRecibido->id_pedido);

			if(archivo_existe(pathCompleto)){

				FILE* pedido = fopen(pathCompleto, "r");
				t_archivo_FS* archivoFS = leer_archivo(pedido);
				fclose(pedido);

				char* stream = bajar_datos_bloques_y_liberarlos(archivoFS);
				obtener_pedido_respuesta* pedidoObtenido = deserializar_pedido_FS(stream);

				if(pedidoObtenido->estado!=CONFIRMADO){
					log_info(logger, "SINDICATO :: El pedido %d no se encuentra en estado confirmado",terminarPedidoRecibido->id_pedido);

					subir_pedido_bloque(pedidoObtenido, pathCompleto);

					return FAIL2;
				}

				pedidoObtenido->estado=TERMINADO;

				subir_pedido_bloque(pedidoObtenido, pathCompleto);

				log_info(logger, "SINDICATO :: SE TERMINO EL PEDIDO %d",terminarPedidoRecibido->id_pedido);

				list_destroy(pedidoObtenido->listaCantidadDePedido);
				list_destroy(pedidoObtenido->listaCantidadLista);
				list_destroy(pedidoObtenido->listaTamanioPlato);
				list_destroy(pedidoObtenido->platos);
				free(pedidoObtenido);

				return OK2;
			}
			log_info(logger, "SINDICATO :: NO EXISTE EL PEDIDO %d",terminarPedidoRecibido->id_pedido);
			return FAIL2;
		}
		log_info(logger, "SINDICATO :: EL RESTAURANTE NO SE ENCUENTRA EN AFIP");

		return FAIL2;
}


obtener_pedido_respuesta* obtener_pedido_FS(char* restaurante, uint32_t pedido){

//	1. Verificar si el Restaurante existe dentro del File System.
//		Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante.
//		En caso de no existir se deberá informar dicha situación.
//	2. Verificar si el Pedido existe dentro del File System.
//		Para esto se deberá buscar dentro del directorio del Restaurante si existe dicho pedido.
//		En caso de no existir se deberá informar dicha situación.
//	3. Responder el mensaje indicando si se pudo realizar en conjunto con la información del pedido si correspondiera.

	char* pathRestaurante = crear_path_restaurante(restaurante);
	obtener_pedido_respuesta* msg = dar_nuevo_pedido();

	if(directorio_existe(pathRestaurante)){
		free(pathRestaurante);
		log_info(logger, "SINDICATO :: EL RESTAURANTE NO %s SE ENCUENTRA EN AFIP", restaurante);
		char* pathCompleto = path_pedido_restaurante(restaurante, pedido);

		FILE* pedidoArchivo = fopen(pathCompleto, "r");

		if(pedidoArchivo){

			log_info(logger, "SINDICATO :: EL PEDIDO %d SE ENCUENTRA EN AFIP", pedido);

			t_archivo_FS* archivoFS = leer_archivo(pedidoArchivo);
			char* stream = bajar_datos_bloques(archivoFS);
			msg = deserializar_pedido_FS(stream);

			log_info(logger, "SINDICATO :: SE OBTIENEN LOS DATOS DEL PEDIDO");

			fclose(pedidoArchivo);
		}
		else
			log_info(logger, "SINDICATO :: EL PEDIDO NO SE ENCUENTRA EN AFIP");
	}
	log_info(logger, "SINDICATO :: EL RESTAURANTE NO %s SE ENCUENTRA EN AFIP", restaurante);

	return msg;
}

void agregar_al_pedido_plato(obtener_pedido_respuesta* msg, guardar_plato* guardarPlatoRecibido){

	bool mismoPlato(char* elem){
		return (strcmp(elem, guardarPlatoRecibido->nombrePlato) == 0);
	}
	t_restaurante* obtenerRestaurante = malloc(sizeof(t_restaurante));
	obtenerRestaurante->tamanioNombre = guardarPlatoRecibido->tamanioNombre;
	obtenerRestaurante->nombreRestaurante = string_duplicate(guardarPlatoRecibido->nombreRestaurante);
	obtener_restaurante_respuesta* restaurante = obtener_restaurante_FS(obtenerRestaurante);

	uint32_t precioPlato=0;
	if(list_any_satisfy(restaurante->platos, mismoPlato)){
		for(int i=0;i < list_size(restaurante->platos);i++){
			if( strcmp(list_get(restaurante->platos, i),guardarPlatoRecibido->nombrePlato) == 0 )
				precioPlato = (uint32_t) list_get(restaurante->precio_platos, i);
		}
	}
	free(obtenerRestaurante->nombreRestaurante);
	free(obtenerRestaurante);
	list_destroy(restaurante->afinidad_cocineros);
	list_destroy(restaurante->listaTamanioAfinidades);
	list_destroy(restaurante->listaTamanioPlatos);
	list_destroy(restaurante->platos);
	list_destroy(restaurante->precio_platos);
	free(restaurante->nombre_restaurante);
	free(restaurante);
	if(!list_any_satisfy(msg->platos, mismoPlato)){
		list_add(msg->listaTamanioPlato, (uint32_t) strlen(guardarPlatoRecibido->nombrePlato)+1);
		list_add(msg->platos, guardarPlatoRecibido->nombrePlato);
		list_add(msg->listaCantidadDePedido, guardarPlatoRecibido->cantidad_platos);
		list_add(msg->listaCantidadLista, 0);
		msg->precioTotal += precioPlato * guardarPlatoRecibido->cantidad_platos;
		return;
	}
	else{
		for(int i=0;i < list_size(msg->platos);i++){
			if( strcmp(list_get(msg->platos, i),guardarPlatoRecibido->nombrePlato) == 0 ){
				list_replace(msg->listaCantidadDePedido, i, list_get(msg->listaCantidadDePedido, i) + guardarPlatoRecibido->cantidad_platos);
				msg->precioTotal += precioPlato * guardarPlatoRecibido->cantidad_platos;
				return;
			}
		}
	}
}


t_receta_respuesta* deserializar_receta_FS(char* streamAux){
	//------------ORDEN------------
	//1. Cantidad pasos
	//2. listaTamanioPasos	   		 } COMO SON LISTAS IRAN
	//3. listaPasos            	   	 }   DENTRO DE UN FOR
	//4. listaTiempoPasos     		 }
	//-----------------------------

	t_receta_respuesta* msg = malloc(sizeof(t_receta_respuesta));

	msg->listaPasos = list_create();
	msg->listaTamanioPasos = list_create();
	msg->listaTiempoPasos = list_create();
	uint32_t offset = 0;

	t_bloque_FS* bloqueAux = crear_bloque(streamAux,0);
	free(streamAux);
	uint32_t aux;

	char** pasosAux = obtener_dato_lista(bloqueAux,"PASOS");

	char** listaTiempoAux= obtener_dato_lista(bloqueAux,"TIEMPO_PASOS");
	for(int i = 0; pasosAux[i]; i++){
		aux = strlen(pasosAux[i]) + 1;
		list_add(msg->listaTamanioPasos, aux);
		list_add(msg->listaPasos, pasosAux[i]);
		list_add(msg->listaTiempoPasos, atoi(listaTiempoAux[i]));
	}

	dictionary_destroy(bloqueAux->diccionario);
	free(bloqueAux->stream);
	free(bloqueAux);
	free(pasosAux);
	free(listaTiempoAux);
	return msg;
}

obtener_restaurante_respuesta* deserializar_restaurante_FS(char* stream){

	//------------RECIBE------------
	//1. Cantidad cocineros
	//2. Posicion
	//3. afinidad_cocineros         }   DENTRO DE UN FOR
	//4. platos               		} COMO SON LISTAS IRAN
	//5. precio_platos         		}   DENTRO DE UN FOR
	//6. cantidad_hornos
	//7. cantidadPedidos
	//-----------------------------

	//------------CREA------------
		//3. Cantidad cocineros
		//4. Posicion en X
		//5. Posicion en Y
		//6. Cantidad Afinidades
		//7. listaTamanioAfinidades		 } COMO SON LISTAS IRAN
		//8. afinidad_cocineros       	 }   DENTRO DE UN FOR
		//9. Cantidad platos
		//10. listaTamanioPlatos	   	 } COMO SON LISTAS IRAN
		//11. platos               		 }   DENTRO DE UN FOR
		//12. precio_platos          	 }
		//13. cantidad_hornos
		//14. cantidadPedidods
	//-----------------------------

	t_bloque_FS* bloqueAux = crear_bloque(stream,0);
	free(stream);
	uint32_t aux;

	obtener_restaurante_respuesta* msg= malloc(sizeof(obtener_restaurante_respuesta));

	msg->listaTamanioAfinidades = list_create();
	msg->afinidad_cocineros = list_create();
	msg->listaTamanioPlatos = list_create();
	msg->platos = list_create();
	msg->precio_platos = list_create();

	msg->cantidad_cocineros = obtener_dato_int(bloqueAux,"CANTIDAD_COCINEROS");
	char** posicionAux = obtener_dato_lista(bloqueAux,"POSICION");
	msg->posicion.posX = atoi(posicionAux[0]);
	msg->posicion.posY = atoi(posicionAux[1]);
	string_iterate_lines(posicionAux, (void*) free);
	free(posicionAux);

	char** afinidadesAux = obtener_dato_lista(bloqueAux,"AFINIDAD_COCINEROS");
	for(int i = 0; afinidadesAux[i]; i++){
		aux = strlen(afinidadesAux[i]) + 1;
		list_add(msg->listaTamanioAfinidades, aux);
		list_add(msg->afinidad_cocineros, afinidadesAux[i]);
	}
	free(afinidadesAux);

	char** platosAux = obtener_dato_lista(bloqueAux,"PLATOS");
	char** preciosPlatosAux = obtener_dato_lista(bloqueAux,"PRECIO_PLATOS");
	for(int i = 0; platosAux[i]; i++){
		aux = strlen(platosAux[i]) + 1;
		list_add(msg->listaTamanioPlatos, aux);
		list_add(msg->platos, platosAux[i]);
		aux = (uint32_t) atoi(preciosPlatosAux[i]);
		list_add(msg->precio_platos, aux);
		free(preciosPlatosAux[i]);
	}
	msg->cantidad_hornos = obtener_dato_int(bloqueAux,"CANTIDAD_HORNOS");
	msg->cantidadPedidos = obtener_dato_int(bloqueAux,"CANTIDAD_PEDIDOS");

	dictionary_destroy(bloqueAux->diccionario);
	free(bloqueAux->stream);
	free(bloqueAux);
	free(platosAux);
	free(preciosPlatosAux);
	return msg;
}

obtener_pedido_respuesta* deserializar_pedido_FS(char* streamAux){
	//------------ORDEN------------
			//1. Cantidad de platos
			//2. Estado
			//3. Tamanio Plato  }
			//3. Platos         } COMO SON LISTAS IRAN
			//4. Cant x Platos  }   DENTRO DE UN FOR
			//5. Cant Lista     }
			//6. Precio Total
	//-----------------------------

	t_bloque_FS* bloqueAux = crear_bloque(streamAux,0);
	free(streamAux);
	uint32_t aux;

	obtener_pedido_respuesta* msg= malloc(sizeof(obtener_pedido_respuesta));

	msg->listaCantidadDePedido = list_create();
	msg->listaCantidadLista = list_create();
	msg->platos = list_create();
	msg->listaTamanioPlato = list_create();

	msg->estado = dar_estado_pedido_INT( (char*) obtener_dato_string(bloqueAux,"ESTADO_PEDIDO") );
	char** platosAux = obtener_dato_lista(bloqueAux,"LISTA_PLATOS");
	char** cantPlatosXPedidoAux = obtener_dato_lista(bloqueAux,"CANTIDAD_PLATOS");
	char** cantPlatosListosAux = obtener_dato_lista(bloqueAux,"CANTIDAD_LISTA");
	for(int i = 0; platosAux[i]; i++){
		aux = strlen(platosAux[i]) + 1;
		list_add(msg->listaTamanioPlato, aux);
		list_add(msg->platos, platosAux[i]);
		list_add(msg->listaCantidadDePedido, atoi(cantPlatosXPedidoAux[i]));
		free(cantPlatosXPedidoAux[i]);
		list_add(msg->listaCantidadLista, atoi(cantPlatosListosAux[i]));
		free(cantPlatosListosAux[i]);
	}
	free(cantPlatosXPedidoAux);
	free(cantPlatosListosAux);
	msg->precioTotal = obtener_dato_int(bloqueAux,"PRECIO_TOTAL");

	dictionary_destroy(bloqueAux->diccionario);
	free(bloqueAux->stream);
	free(bloqueAux);
	free(platosAux);

	return msg;
}

char* dar_estado_pedido(estado_pedido estado){
	switch(estado){
		case PENDIENTE:
			return "Pendiente";
		case CONFIRMADO:
			return "Confirmado";
		case TERMINADO:
			return "Terminado";
	}
	return NULL;
}

estado_pedido dar_estado_pedido_INT(char* estado){
	if(strcmp(estado,"Pendiente")==0)
		return PENDIENTE;
	if(strcmp(estado,"Confirmado")==0)
		return CONFIRMADO;
	if(strcmp(estado,"Terminado")==0)
		return TERMINADO;
	return -1;
}

t_respuesta2 verificar_plato_listo(obtener_pedido_respuesta* pedidoObtenido,plato_listo* platoListoRecibido){
	bool mismoPlato(char* elem){
		return (strcmp(elem, platoListoRecibido->nombrePlato) == 0);
	}
	if(!list_any_satisfy(pedidoObtenido->platos, mismoPlato)){
		log_info(logger, "SINDICATO :: El plato %s no se encuentra en la lista de platos del pedido %d",platoListoRecibido->nombrePlato, platoListoRecibido->id_pedido);
		return FAIL2;
	}
	else{
		for(int i=0;i < list_size(pedidoObtenido->platos);i++){
			if( strcmp(list_get(pedidoObtenido->platos, i),platoListoRecibido->nombrePlato) == 0){
				if(list_get(pedidoObtenido->listaCantidadLista,i) < list_get(pedidoObtenido->listaCantidadDePedido,i)) {
					list_replace(pedidoObtenido->listaCantidadLista, i,list_get(pedidoObtenido->listaCantidadLista, i)+1);
					return OK2;
				}
				else
					return FAIL2; // TODO ESTA BIEN QUE DIGA QUE NO???
			}
		}
	}
}
