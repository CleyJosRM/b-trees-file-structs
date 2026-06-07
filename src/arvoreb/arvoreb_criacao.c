#include "../include/arvoreb/arvoreb_interna.h"

void criar_BTree(FILE* arquivo){
    byteNoB cabecalhoInicial[TAM_CABECALHO_BTREE];
    cabecalhoInicial[0] = '0';
    *(int*)&cabecalhoInicial[1] = -1; // RRNraiz
    *(int*)&cabecalhoInicial[5] = -1; // Topo
    *(int*)&cabecalhoInicial[9] = 0;  // ProxRRN
    *(int*)&cabecalhoInicial[13] = 0; // NroNos

    fseek(arquivo, 0, SEEK_SET);
    fwrite(cabecalhoInicial, TAM_CABECALHO_BTREE, 1, arquivo);
}

int criar_no(FILE* arvoreB, byteNoB* cabecalho, int tipoNo, byteNoB* novoNo){
    memset(novoNo, LIXO, TAM_NO_BTREE);    
    novoNo[0] = '0'; // Removido
    *(int*)&novoNo[1] = -1;  // ProxPilha
    *(int*)&novoNo[5] = tipoNo;
    *(int*)&novoNo[9] = 0;   // NroChaves
    
    ENTRADA_INDICE aux[1];
    set_entradas(aux, 0, novoNo);
    no_definir_filho(novoNo, 0, -1);

    int RRNnovoNo;
    int topo = get_topo(cabecalho);

    if (topo != -1) {
        RRNnovoNo = topo;
        byteNoB noRemovido[TAM_NO_BTREE];
        carregar_no(arvoreB, RRNnovoNo, noRemovido);
        // Atualiza o topo do cabeçalho para o próximo RRN da pilha
        *(int*)&cabecalho[5] = *(int*)&noRemovido[1]; 
    } else {
        RRNnovoNo = get_proxRRN(cabecalho);
        *(int*)&cabecalho[9] += 1;  // Atualiza ProxRRN apenas se for nó novo
    }

    *(int*)&cabecalho[13] += 1; // NroNos
    return RRNnovoNo;
}

void empilhar_pagina_livre(FILE* arvoreB, byteNoB* cabecalho, int rrnPagina){
    byteNoB pagina[TAM_NO_BTREE];
    pagina[0] = '1'; // Removido
    *(int*)&pagina[1] = get_topo(cabecalho);
    *(int*)&pagina[5] = TIPOFOLHA;
    *(int*)&pagina[9] = 0;
    
    armazenar_no(arvoreB, rrnPagina, pagina);
    *(int*)&cabecalho[5] = rrnPagina; // Atualiza o topo
    *(int*)&cabecalho[13] -= 1;       // Atualiza NroNos
}
