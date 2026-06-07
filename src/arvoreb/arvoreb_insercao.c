#include "../include/arvoreb/arvoreb_interna.h"

static void inserir_entrada_em_no_shiftada(ENTRADA_INDICE novaEntrada, byteNoB* no){
    int nroChaves = no_obter_num_chaves(no);
    if(nroChaves == ORDEM_BTREE - 1){
        #ifdef PRINT_ERROS
        DEBUG("O nó está lotado! Não há como inserir.\n");
        #endif
        exit(1);
    }

    ENTRADA_INDICE entradas[nroChaves+1];
    get_entradas(entradas, nroChaves, no);
    bool inseriu = false;
    int i = nroChaves-1;
    
    for(; i>=0; i--){
        if(entradas[i].chave > novaEntrada.chave){
            entradas[i+1] = entradas[i];
        } else {
            entradas[i+1] = novaEntrada;
            inseriu = true;
            break;
        }
    }

    if(!inseriu){
        entradas[0] = novaEntrada;
    }

    set_entradas(entradas, nroChaves+1, no);
}

static ENTRADA_INDICE inserir_entrada_em_no(FILE* arvoreB, byteNoB* cabecalho, byteNoB* noAtual, int RRNatual, ENTRADA_INDICE entradaInserir, int* tipoNoCriar){
    if(check_entrada_nula(entradaInserir)){
        return entradaInserir;
    }

    int nroChaves = no_obter_num_chaves(noAtual);
    if(nroChaves < ORDEM_BTREE - 1){
        inserir_entrada_em_no_shiftada(entradaInserir, noAtual);
        armazenar_no(arvoreB, RRNatual, noAtual);
        return get_entrada_nula();
    } else {
        ENTRADA_INDICE vetorEntradas[ORDEM_BTREE];
        get_entradas(vetorEntradas, ORDEM_BTREE-1, noAtual);
        vetorEntradas[ORDEM_BTREE-1] = entradaInserir;
        qsort(vetorEntradas, ORDEM_BTREE, sizeof(ENTRADA_INDICE), comparar_entradas);
        
        ENTRADA_INDICE entradaPromovida = vetorEntradas[ORDEM_BTREE/2];
        
        byteNoB novoNo[TAM_NO_BTREE];
        int RRNnovoNo = criar_no(arvoreB, cabecalho, *tipoNoCriar, novoNo);
        *tipoNoCriar = TIPOINTERMEDIARIO;
        
        no_definir_filho(novoNo, 0, entradaPromovida.RRNdescendente);
        entradaPromovida.RRNdescendente = RRNnovoNo;
        
        set_entradas(vetorEntradas, ORDEM_BTREE/2, noAtual);
        set_entradas(vetorEntradas+ORDEM_BTREE/2+1, ORDEM_BTREE-ORDEM_BTREE/2-1, novoNo);
        
        armazenar_no(arvoreB, RRNatual, noAtual);
        armazenar_no(arvoreB, RRNnovoNo, novoNo);
        
        return entradaPromovida;
    }
}


static ENTRADA_INDICE inserir_entrada_na_arvore_rec(FILE* arvoreB, byteNoB* cabecalho, byteNoB* noAtual, int RRNatual, ENTRADA_INDICE entradaInserir, int* tipoNoCriado) {
    int RRNdescendente;
    percorrer_no(noAtual, entradaInserir.chave, &RRNdescendente);

    if(RRNdescendente == -1){
        return inserir_entrada_em_no(arvoreB, cabecalho, noAtual, RRNatual, entradaInserir, tipoNoCriado);
    } else {
        byteNoB descendente[TAM_NO_BTREE];
        carregar_no(arvoreB, RRNdescendente, descendente);
        entradaInserir = inserir_entrada_na_arvore_rec(arvoreB, cabecalho, descendente, RRNdescendente, entradaInserir, tipoNoCriado);
        return inserir_entrada_em_no(arvoreB, cabecalho, noAtual, RRNatual, entradaInserir, tipoNoCriado);
    }
}

static void criar_raiz_e_inserir(FILE* arvoreB, byteNoB* cabecalho, ENTRADA_INDICE entradaRaiz){
    int tipo = TIPORAIZ;

    int rrnAntigaRaiz = get_RRNraiz(cabecalho);
    if(get_nroNos(cabecalho) == 0){
        tipo = TIPOFOLHA;
    }

    byteNoB novaRaiz[TAM_NO_BTREE];
    int RRNnovaRaiz = criar_no(arvoreB, cabecalho, tipo, novaRaiz);
    *(int*)&cabecalho[1] = RRNnovaRaiz; // Atualiza RRNRaiz diretamente pelo offset base

    if(tipo == TIPORAIZ){
        no_definir_filho(novaRaiz, 0, rrnAntigaRaiz);
    }
    
    inserir_entrada_em_no_shiftada(entradaRaiz, novaRaiz);
    armazenar_no(arvoreB, RRNnovaRaiz, novaRaiz);
}

void inserir_entrada_na_arvore(FILE* arvoreB, int chave, int RRNdados){
    ENTRADA_INDICE inserirNaRaiz = {chave, RRNdados, -1};
    byteNoB cabecalho[TAM_CABECALHO_BTREE];
    carregar_cabecalho(arvoreB, cabecalho, true);

    if(get_RRNraiz(cabecalho) == -1){
        criar_raiz_e_inserir(arvoreB, cabecalho, inserirNaRaiz);
        armazenar_cabecalho(arvoreB, cabecalho);
        return;
    }

    byteNoB raiz[TAM_NO_BTREE];
    int RRNraiz = get_RRNraiz(cabecalho);
    carregar_no(arvoreB, RRNraiz, raiz);
    int tipoNoCriar = TIPOFOLHA;

    ENTRADA_INDICE entradaRaiz = inserir_entrada_na_arvore_rec(arvoreB, cabecalho, raiz, RRNraiz, inserirNaRaiz, &tipoNoCriar);
    
    if(!check_entrada_nula(entradaRaiz)){
        if (*(int*)&raiz[5] == TIPORAIZ) {
            *(int*)&raiz[5] = TIPOINTERMEDIARIO;
            armazenar_no(arvoreB, RRNraiz, raiz);
        }
        
        criar_raiz_e_inserir(arvoreB, cabecalho, entradaRaiz);
    }

    armazenar_cabecalho(arvoreB, cabecalho);
}
