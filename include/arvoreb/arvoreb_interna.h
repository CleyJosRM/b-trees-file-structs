#ifndef ARVOREB_INTERNA_H
    #define ARVOREB_INTERNA_H

    #include "../../include/core/definicoes.h"
    #include "indice.h"
    #include<stdlib.h>
    #include<stdio.h>
    #include<stdlib.h>
    #include<stdbool.h>
    #include<string.h>

    // Tipos e Structs Internas

    // Macros de Configuração

    #define ORDEM_BTREE 4
    #define m ORDEM_BTREE
    #define NUM_MIN_CHAVES (m+1)/2 - 1
    #define INCONSISTENTE '0'
    #define CONSISTENTE '1'
    #define TIPORAIZ 0
    #define TIPOINTERMEDIARIO 1
    #define TIPOFOLHA -1

    // BO é abreviação de byteoffset
    #define TAM_CABECALHO_BTREE 17
    #define BO_status 0
    #define BO_RRNraiz 1
    #define BO_topo 5
    #define BO_proxRRN 9
    #define BO_nroNos 13


    #define TAM_NO_BTREE 53
    #define NO_INEXISTENTE -1
    #define BO_removido 0
    #define BO_proximo 1
    #define BO_tipoNo 5
    #define BO_nroChaves 9
    #define BO_C1 13
    #define BO_PR1 BO_C1 + 4
    #define BO_P1 BO_C1 + 8*(ORDEM_BTREE-1)
    #define BO_Pm BO_P1 + 4*(ORDEM_BTREE-1) // byte offset do último Pi

    typedef unsigned char byteBTree;

    typedef struct _entrada_indice_ {
        int chave;
        int BOdados;
        int RRNdescendente;
    } ENTRADA_INDICE;



    // Manipulação de Disco (I/O)
    void armazenar_no(FILE* arvoreB, const byteBTree* no, int RRN);
    void carregar_no(byteBTree* buffer, FILE* arvoreB, int RRN);
    void armazenar_cabecalho(FILE* arvoreB, const byteBTree* buffer);
    void carregar_cabecalho(byteBTree* buffer, FILE* arvoreB, bool statusInconsistente);

    /*
    MEXENDO COM VETORES DE BYTES
    essas funções não verificam o tamanho do buffer,
    portanto o chamador deve garantir que os 4 bytes
    a partir de byteoffset devem estar dentro do vetor
    */






    /**
     * Acessa a memória no endereço vetor + byteoffset e lê os próximos 
     * quatro bytes como inteiro em formato little-endian e o retorna.
     * O byteoffset deve ser menor ou igual ao tamanho do vetor - 4.
     */
    static inline int get_inteiro(const byteBTree* vetor, int byteoffset){
        return *(int*)&vetor[byteoffset];
    }

    /**
     * Acessa a memória no endereço vetor + byteoffset
     * e escreve um inteiro nos próximos quatro bytes
     * em formato little-endian.
     * O byteoffset deve ser menor ou igual ao tamanho do vetor - 4.
     */
    static inline void set_inteiro(byteBTree* vetor, int byteoffset, int val){
        *(int*)&vetor[byteoffset] = val;
    }

    /**
     * Acessa a memória no endereço vetor + byteoffset
     * e interpreta os próximos quatro bytes como inteiro
     * e adiciona um incremento.
     * O byteoffset deve ser menor ou igual ao tamanho do vetor - 4.
     */
    static inline void inc_inteiro(byteBTree* vetor, int byteoffset, int incremento){
        *(int*)&vetor[byteoffset] += incremento;
    }

    // Getters e Setters de Cabeçalho
    int get_RRNraiz(byteBTree* cabecalho);
    int get_topo(byteBTree* cabecalho);
    int get_proxRRN(byteBTree* cabecalho);
    int get_nroNos(byteBTree* cabecalho);

    // Getters e Setters de Nós
    void get_entradas(ENTRADA_INDICE vetorEntradas[], const byteBTree* no, int n);
    void set_entradas(byteBTree* no, const ENTRADA_INDICE vetorEntradas[], int n);
    bool no_eh_folha(byteBTree* no);
    int get_nroChaves(byteBTree* no);
    void set_nroChaves(byteBTree* no, int n);
    int get_chave(byteBTree* no, int idx);
    void set_chave(byteBTree* no, int idx, int chave);
    int get_RRNdados(byteBTree* no, int idx);
    void set_RRNdados(byteBTree* no, int idx, int rrn);
    int get_filho(byteBTree* no, int idx);
    void set_filho(byteBTree* no, int idx, int rrnFilho);

    // Getters e Setters de Entrada
    ENTRADA_INDICE get_entrada_nula();
    bool check_entrada_nula(ENTRADA_INDICE entrada);
    int comparar_entradas(const void* entrada1, const void* entrada2);
    bool no_pode_emprestar(byteBTree* no);

    // Criação de Nós
    int criar_no(byteBTree* novoNo, FILE* arvoreB, byteBTree* cabecalho, int tipoNo);

    void empilhar_pagina_livre(FILE* arvoreB, byteBTree* cabecalho, int rrnPagina);

    // Navegação em Nó
    bool percorrer_no(int* ret, const byteBTree* no, int chaveBusca);

    // Funções de Debug
    void imprimir_no_bytes(byteBTree* no);
    void imprimir_no(byteBTree* noB, int RRN);
    void imprimir_cabecalho(byteBTree* cabecalho);

#endif // ARVOREB_INTERNA_H