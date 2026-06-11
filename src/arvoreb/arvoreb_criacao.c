#include "arvoreb/arvoreb_interna.h"

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
 * É necessário fornecer o cabeçalho da árvore-B para obter o RRN do novo nó e atualizar o cabeçalho.
 * Retorna o RRN do novo nó
 */
int criar_no(byteBTree* novoNo, byteBTree* cabecalho, int tipoNo){
    novoNo[BO_removido] = '0'; // o novo nó não está removido
    set_inteiro(novoNo, BO_proximo, -1); // não há próximo nó na pilha de removidos
    set_inteiro(novoNo, BO_tipoNo, tipoNo); // o chamador deve especificar o tipo do nó criado
    set_inteiro(novoNo, BO_nroChaves, 0); // o nroChaves do nó inicialmente é 0
    
    set_entradas(novoNo, NULL, 0); // inicializando o nó com entradas nulas
    set_inteiro(novoNo, BO_P1, -1); // inicializando P1 como -1

    // Obtendo RRN do novo nó e atualizando cabeçalho:
    int RRNnovoNo = get_inteiro(cabecalho, BO_proxRRN); // lendo o cabeçalho para obter o RRN do novo nó 
    inc_inteiro(cabecalho, BO_proxRRN, 1); // atualizando o proxRRN no cabeçalho
    inc_inteiro(cabecalho, BO_nroNos, 1); // atualizando o nroNos no cabeçalho
    return RRNnovoNo; // retornando RRN do novo nó
}

void empilhar_pagina_livre(FILE* arvoreB, byteBTree* cabecalho, int rrnPagina){
    byteBTree pagina[TAM_NO_BTREE];
    pagina[0] = '1'; // Removido
    //*(int*)&pagina[1] = get_topo(cabecalho);
    set_inteiro(pagina, BO_proximo, get_inteiro(cabecalho, BO_topo));
    //*(int*)&pagina[5] = TIPOFOLHA;
    set_inteiro(pagina, BO_tipoNo, TIPOFOLHA);
    //*(int*)&pagina[9] = 0;
    set_inteiro(pagina, BO_nroChaves, 0);
    
    armazenar_no(arvoreB, pagina, rrnPagina);
    // *(int*)&cabecalho[5] = rrnPagina; // Atualiza o topo
    set_inteiro(cabecalho, BO_topo, rrnPagina);
    //*(int*)&cabecalho[13] -= 1;       // Atualiza NroNos
    inc_inteiro(cabecalho, BO_nroNos, -1);
}
