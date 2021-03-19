#ifndef SRC_MEMORIA_H_
#define SRC_MEMORIA_H_

#include "utils_comanda.h"

void iniciar_memoria(void);
void* convertir(char* algoritmo_nombre);
bool pagina_esta_en_memoria(t_pagina* pagina);
void escribir_en_memoria(t_pagina* pagina, bool esta_en_memoria, int idPedido, bool modificado);
bool pagina_esta_en_memoria(t_pagina* pagina);
void seleccionar_victima_LRU(void);
void seleccionar_victima_CLOCK_ME(void);
bool hay_marcos_libres(void);
entradaTablaMarcos* buscar_entrada(void* marco);
entradaTablaMarcos* asignar_entrada_marco_libre(void);
bool ambos_bits_apagados(entradaTablaMarcos* entrada);
bool bit_uso(entradaTablaMarcos* entrada);
bool bit_modificado(entradaTablaMarcos* entrada);

int cantidad_de_marcos;
int cantidad_de_marcos_swap;

#endif /* SRC_MEMORIA_H_ */
