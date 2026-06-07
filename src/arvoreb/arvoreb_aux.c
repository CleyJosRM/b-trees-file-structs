#include "../include/arvoreb/arvoreb_interna.h"

ENTRADA_INDICE get_entrada_nula(){
    ENTRADA_INDICE entradaNula;
    entradaNula.chave = -1;
    entradaNula.RRNdados = -1;
    entradaNula.RRNdescendente = -1;
    return entradaNula;
}

bool check_entrada_nula(ENTRADA_INDICE entrada){
    ENTRADA_INDICE nula = get_entrada_nula();
    if(entrada.chave == nula.chave && entrada.RRNdados == nula.RRNdados && entrada.RRNdescendente == nula.RRNdescendente){
        return true;
    }
    return false;
}

int comparar_entradas(const void* entrada1, const void* entrada2){
    ENTRADA_INDICE e1 = *(ENTRADA_INDICE*)entrada1;
    ENTRADA_INDICE e2 = *(ENTRADA_INDICE*)entrada2;
    return e1.chave - e2.chave;
}

int num_minimo_chaves(){
    return (ORDEM_BTREE + 1) / 2 - 1;
}

bool no_pode_emprestar(byteNoB* no){
    return no_obter_num_chaves(no) > num_minimo_chaves();
}