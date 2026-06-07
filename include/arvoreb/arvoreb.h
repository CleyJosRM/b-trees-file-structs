#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>
#include <stdbool.h>

typedef unsigned char byteNoB;

// Funções Públicas da Árvore-B
void criar_BTree(FILE* arquivo);
int buscar_chave(FILE* arvoreB, int chaveBusca);
void inserir_entrada_na_arvore(FILE* arvoreB, int chave, int RRNdados);
void remover_chave_arvoreB(FILE* arvoreB, int chave);

#endif