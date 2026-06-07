#ifndef ARVOREB_INTERNA_H
#define ARVOREB_INTERNA_H

#include "../include/core/definicoes.h"
#include "arvoreb.h"
#include <stdlib.h>

// Macros de Configuração
#define TIPORAIZ 0
#define TIPOFOLHA -1
#define TIPOINTERMEDIARIO 1
#define ORDEM_BTREE 4
#define TAM_CABECALHO_BTREE 17
#define TAM_NO_BTREE 53

// Tipos e Structs Internas
typedef struct {
    int chave;
    int RRNdados;
    int RRNdescendente;
} ENTRADA_INDICE;

// Manipulação de Disco (I/O)
void armazenar_no(FILE* arvoreB, int RRN, byteNoB* no);
void carregar_no(FILE* arvoreB, int RRN, byteNoB* buffer);
void armazenar_cabecalho(FILE* arvoreB, byteNoB* buffer);
void carregar_cabecalho(FILE* arvoreB, byteNoB* buffer, bool statusInconsistente);

// Getters e Setters de Cabeçalho
int get_RRNraiz(byteNoB* cabecalho);
int get_topo(byteNoB* cabecalho);
int get_proxRRN(byteNoB* cabecalho);
int get_nroNos(byteNoB* cabecalho);

// Getters e Setters de Nós
void get_entradas(ENTRADA_INDICE* vetorEntradas, int n, byteNoB* no);
void set_entradas(ENTRADA_INDICE* vetorEntradas, int n, byteNoB* no);
bool no_eh_folha(byteNoB* no);
int no_obter_num_chaves(byteNoB* no);
void no_definir_num_chaves(byteNoB* no, int n);
int no_obter_chave(byteNoB* no, int idx);
void no_definir_chave(byteNoB* no, int idx, int chave);
int no_obter_RRNdados(byteNoB* no, int idx);
void no_definir_RRNdados(byteNoB* no, int idx, int rrn);
int no_obter_filho(byteNoB* no, int idx);
void no_definir_filho(byteNoB* no, int idx, int rrnFilho);

// Getters e Setters de Entrada
ENTRADA_INDICE get_entrada_nula();
bool check_entrada_nula(ENTRADA_INDICE entrada);
int comparar_entradas(const void* entrada1, const void* entrada2);
bool no_pode_emprestar(byteNoB* no);
int num_minimo_chaves();

// Criação de Nós
int criar_no(FILE* arvoreB, byteNoB* cabecalho, int tipoNo, byteNoB* novoNo);

void empilhar_pagina_livre(FILE* arvoreB, byteNoB* cabecalho, int rrnPagina);

// Navegação em Nó
bool percorrer_no(byteNoB* no, int chaveBusca, int* retRRN);

// Funções de Debug
void imprimir_no_bytes(byteNoB* no);
void imprimir_no(byteNoB* noB, int RRN);
void imprimir_cabecalho(byteNoB* cabecalho);

#endif // ARVOREB_INTERNA_H