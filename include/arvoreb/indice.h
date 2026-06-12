#ifndef ARVOREB_H
	#define ARVOREB_H

	#include <stdio.h>
	#include <stdbool.h>

	// Funções Públicas do índice, implementado como Árvore-B:
	FILE* abrir_indice(char* nomeIndice, bool escrita);
	bool fechar_indice(FILE* indice, bool marcarConsistente);
	void criar_indice(FILE* arquivo);
	int buscar_entrada(FILE* indice, int chaveBusca);
	void inserir_entrada(FILE* indice, int chave, int BOdados);
	void remover_entrada(FILE* indice, int chave);

#endif