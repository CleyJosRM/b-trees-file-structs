// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#ifndef ABB_H
	#define ABB_H
	
	#include "tabelafuncoes.h"

	// Como alguns itens da Árvore Binária de Busca vão ter a mesma chave 
	// (ex: estações com mesmo nome), ela seguirá a seguinte regra: para cada nó,
	// os itens com ordem menor ou igual estarão à esquerda, e os estritamente maiores
	// estarão à direita.

	typedef struct _abb ABB;
	
	/* Objetivo: criar uma árvore binária capaz de ordenar, identificar e apagar um item customizado.
	* Ass:
	* o parâmetro funcoes_item deve apontar para um array de ponteiros para funções, cujos tipos
	* estão especificados em tabelafuncoes.h
	* Retorno: ponteiro para a árvore binária, com o qual se pode realizar as outras operações.
	*/
	ABB* abb_criar(const TABELA_FUNCOES* funcoes_item);

	// Apaga a ABB e apaga a referência
	void abb_apagar(ABB**);

	// Dado um item e uma ABB, insere o item nela
	void abb_inserir(ABB*, void* item);

	// Dado uma item e uma ABB, remove um item que tenha a mesma chave de busca
	bool abb_remover(ABB*, void* busca);

	// Conta quantos elementos distintos estão na ABB
	int abb_contar_distintos(ABB*);

#endif