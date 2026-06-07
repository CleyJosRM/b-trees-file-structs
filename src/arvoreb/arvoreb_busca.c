#include "../include/arvoreb/arvoreb_interna.h"

bool percorrer_no(byteNoB* no, int chaveBusca, int* retRRN){
    int chavesRestantes = no_obter_num_chaves(no);
    int idx = 0;
    
    while(idx < chavesRestantes && no_obter_chave(no, idx) <= chaveBusca){
        if(no_obter_chave(no, idx) == chaveBusca){
            *retRRN = no_obter_RRNdados(no, idx);
            return true;
        }
        idx++;
    }
    *retRRN = no_obter_filho(no, idx);
    return false;
}

static int buscar_chave_rec(FILE* arvoreB, int chaveBusca, byteNoB* no){
    int BO_RRN;
    if (percorrer_no(no, chaveBusca, &BO_RRN) == true || BO_RRN == -1){
        return BO_RRN;
    } else {
        byteNoB proximoNo[TAM_NO_BTREE];
        carregar_no(arvoreB, BO_RRN, proximoNo);
        return buscar_chave_rec(arvoreB, chaveBusca, proximoNo);
    }
}

int buscar_chave(FILE* arvoreB, int chaveBusca){
    byteNoB cabecalho[TAM_CABECALHO_BTREE];
    carregar_cabecalho(arvoreB, cabecalho, false);

    if(cabecalho[0] != '1'){
        return -1;
    }

    int RRNraiz = get_RRNraiz(cabecalho);
    if(RRNraiz == -1){
        return -1;
    }

    byteNoB raiz[TAM_NO_BTREE];
    carregar_no(arvoreB, RRNraiz, raiz);
    return buscar_chave_rec(arvoreB, chaveBusca, raiz);
}
