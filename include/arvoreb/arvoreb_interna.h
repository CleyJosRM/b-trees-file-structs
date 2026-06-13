// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#ifndef ARVOREB_INTERNA_H
    #define ARVOREB_INTERNA_H

    #include "core/definicoes.h"
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



    // MANIPULAÇÃO DE DISCO (I/O)


    /**
     * Transfere um B-nó da memória principal para o arquivo. 
     * A posição onde se deseja inserir esse B-nó deve ser especificada como RRN.
     * A filestream deve permitir escrita.
     */
    void armazenar_no(FILE* arvoreB, const byteBTree* no, int RRN);

    /**
     * Transfere um B-nó do arquivo para a memória principal. 
     * Para achar esse B-nó, deve ser fornecido o RRN dele.
     * A filestream deve permitir leitura.
     */
    void carregar_no(byteBTree* buffer, FILE* arvoreB, int RRN);
    
    /**
     * Transfere o cabeçalho da árvore-B da memória principal para o arquivo.
     * A filestream deve permitir escrita.
     */
    void armazenar_cabecalho(FILE* arvoreB, const byteBTree* buffer);
    
    /**
     * Transfere o cabeçalho da árvore-B do arquivo para a memória principal. 
     * Caso statusInconsistente seja true, o status do arquivo é marcado como '0' antes de retornar.
     * A filestream deve permitir leitura e escrita, mas a escrita só acontece quando statusInconsistente é true.
     */
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



    // GETTERS DE NÓS E CABEÇALHO
    

    // Getters de cabeçalho
    int get_RRNraiz(byteBTree* cabecalho);
    int get_topo(byteBTree* cabecalho);
    int get_proxRRN(byteBTree* cabecalho);
    int get_nroNos(byteBTree* cabecalho);

    // Getters e setters de valores individuais dos nós
    bool no_eh_folha(byteBTree* no); // verifica se no é do tipo folha
    int get_nroChaves(byteBTree* no);
    void set_nroChaves(byteBTree* no, int n);
    int get_chave(byteBTree* no, int idx);
    void set_chave(byteBTree* no, int idx, int chave);
    int get_RRNdados(byteBTree* no, int idx);
    void set_RRNdados(byteBTree* no, int idx, int rrn);
    int get_filho(byteBTree* no, int idx);
    void set_filho(byteBTree* no, int idx, int rrnFilho);



    // GETTERS E SETTERS DE ENTRADAS

    

    /**
     * Lê n entradas de um nó em memória principal e coloca em um vetor de entradas fornecido.
     * Se o tamanho do vetor for maior que n, o resto do vetor é mantido do jeito que estava antes.
     */
    void get_entradas(ENTRADA_INDICE vetorEntradas[], const byteBTree* no, int n);
    
    // Lê n entradas de um vetor e coloca em um nó em memória principal, preenchendo o resto do nó com entradas nulas.
    void set_entradas(byteBTree* no, const ENTRADA_INDICE vetorEntradas[], int n);
    
    // Retorna uma entrada nula, definida como tendo chave, BOdados e RRNdescendente iguais a -1.
    ENTRADA_INDICE get_entrada_nula();
    
    // Verifica se uma entrada qualquer é igual à entrada nula definida acima. Retorna true caso seja, false caso não seja.
    bool check_entrada_nula(ENTRADA_INDICE entrada);
    
    // Função de comparação para o qsort. Retorna >0 se a chave da primeira entrada é maior que da segunda, <0 se for menor, 0 se for igual.
    int comparar_entradas(const void* entrada1, const void* entrada2);



    // CRIAÇÃO DE NÓS

    /**
     * Escreve os valores iniciais de um nó com tipo especificado em um buffer na memória principal.
     * É necessário fornecer o cabeçalho da árvore-B para obter o RRN do novo nó (seja topo da pilha de removidos ou proxRRN)
     * É necessário fornecer a filestream da árvore-B para carregar o topo da pilha de removidos se necessário
     * Retorna o RRN do novo nó
     */
    int criar_no(byteBTree* novoNo, FILE* arvoreB, byteBTree* cabecalho, int tipoNo);



    // NAVEGAÇÃO EM NÓ



    /**
     * Busca uma chave em um nó da árvore B em memória principal.
     * Retorna se a chave foi encontrada ou não, e escreve um inteiro no endereço fornecido.
     * Se a chave foi encontrada, esse inteiro é o byte offset de dados.
     * Se a chave não foi encontrada, esse inteiro é o RRN do nó descendente onde a chaveBusca pode estar.
     */
    bool percorrer_no(int* ret, const byteBTree* no, int chaveBusca);

    // Funções de Debug
    void imprimir_no_bytes(byteBTree* no);
    void imprimir_no(byteBTree* noB, int RRN);
    void imprimir_cabecalho(byteBTree* cabecalho);

#endif // ARVOREB_INTERNA_H