// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#ifndef TABELA_FUNCOES_H
	#define TABELA_FUNCOES_H

	#include<stdbool.h>

	typedef struct {
		// Deve apagar o item e modificar a referência a ele para NULL
		void (*item_apagar)(void** item);
		// Deve retornar <0 se o item1 deve aparecer antes do item2, 0 se tanto faz a ordem, >0 se o item2 deve aparecer antes do item1.
		int (*item_ordenar)(void* item1, void* item2);
		// Deve retornar true se os dois itens têm a mesma chave
		bool (*item_identificar)(void* item1, void* item2);
	} TABELA_FUNCOES;

#endif