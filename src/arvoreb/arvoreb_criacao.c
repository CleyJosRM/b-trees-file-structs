#include "arvoreb/arvoreb_interna.h"

FILE* abrir_indice(char* nomeIndice){  

    FILE* fs = fopen(nomeIndice, "rb+");
    if(fs == NULL){
        DEBUG("ERRO EM abrir_indice: NÃO ENCONTROU O ARQUIVO. VERIFIQUE NOME E PERMISSÕES.");
        return NULL;
    }
    fseek(fs, 0, SEEK_SET);
    byteBTree status = INCONSISTENTE;
    fwrite(&status, 1, 1, fs);

    return fs;    
}

bool fechar_indice(FILE* indice){
    if(indice == NULL){
        return true;
    }

    fseek(indice, 0, SEEK_SET);
    byteBTree status = CONSISTENTE;
    fwrite(&status, 1, 1, indice);
    if(fclose(indice) != 0){
        return false;
    }

    return true;
}

void criar_indice(FILE* arquivo){

    byteBTree cabecalhoInicial[TAM_CABECALHO_BTREE];
    cabecalhoInicial[BO_status] = CONSISTENTE;
    set_inteiro(cabecalhoInicial, BO_RRNraiz, -1);
    set_inteiro(cabecalhoInicial, BO_topo, -1);
    set_inteiro(cabecalhoInicial, BO_proxRRN, 0);
    set_inteiro(cabecalhoInicial, BO_nroNos, 0);

    fseek(arquivo, 0, SEEK_SET);
    fwrite(cabecalhoInicial, TAM_CABECALHO_BTREE, 1, arquivo);
}

/**
 * Escreve os valores iniciais de um nó com tipo especificado em um buffer na memória principal.
 * É necessário fornecer o cabeçalho da árvore-B para obter o RRN do novo nó (seja topo da pilha de removidos ou proxRRN)
 * É necessário fornecer a filestream da árvore-B para carregar o topo da pilha de removidos se necessário
 * Retorna o RRN do novo nó
 */
int criar_no(byteBTree* novoNo, FILE* arvoreB, byteBTree* cabecalho, int tipoNo){
    
    // Inicializando valores do nó:

    novoNo[BO_removido] = '0'; // o novo nó não está removido
    set_inteiro(novoNo, BO_proximo, -1); // como não está removido, o RRN do próximo removido deve ser -1
    set_inteiro(novoNo, BO_tipoNo, tipoNo); // o tipo do nó é especificado pelo chamador
    set_entradas(novoNo, NULL, 0); // inicializando o nó com entradas nulas
    set_inteiro(novoNo, BO_P1, -1); // inicializando P1 como -1
    set_inteiro(novoNo, BO_nroChaves, 0); // o nroChaves do nó inicialmente é 0

    int RRNnovoNo;
    int topo = get_inteiro(cabecalho, BO_topo);
    if(topo == -1){ // se não há nenhum nó removido
        
        // Obtendo RRN do novo nó e atualizando cabeçalho:
        
        RRNnovoNo = get_inteiro(cabecalho, BO_proxRRN); // lendo o cabeçalho para obter o RRN do novo nó 
        inc_inteiro(cabecalho, BO_proxRRN, 1); // atualizando o proxRRN no cabeçalho
    
    }else{ // se há algum nó removido

        RRNnovoNo = topo; // o RRN do novo nó é o RRN do nó no topo da pilha de removidos

        byteBTree noTopo[TAM_NO_BTREE]; // buffer na memória para segurar o nó do topo
        carregar_no(noTopo, arvoreB, topo); // trazendo o nó removido do disco para a memória
        int RRNproximoRemovido = get_inteiro(noTopo, BO_proximo); // obtendo o RRN do próximo na pilha

        set_inteiro(cabecalho, BO_topo, RRNproximoRemovido); // atualizando o topo da pilha com o RRN do próximo na pilha

    }

    inc_inteiro(cabecalho, BO_nroNos, 1); // atualizando o nroNos no cabeçalho  

    return RRNnovoNo;
    
}

void empilhar_pagina_livre(FILE* arvoreB, byteBTree* cabecalho, int rrnPagina){
    byteBTree pagina[TAM_NO_BTREE];
    // Lê o conteúdo atual do nó do disco para preservá-lo.
    // Apenas os campos 'removido' e 'proximo' são alterados; o restante
    // (tipoNo, nroChaves, chaves, PRs, filhos) permanece com o último estado válido.
    carregar_no(pagina, arvoreB, rrnPagina);

    pagina[BO_removido] = '1';
    set_inteiro(pagina, BO_proximo, get_inteiro(cabecalho, BO_topo));

    armazenar_no(arvoreB, pagina, rrnPagina);
    set_inteiro(cabecalho, BO_topo, rrnPagina);
    inc_inteiro(cabecalho, BO_nroNos, -1);
}