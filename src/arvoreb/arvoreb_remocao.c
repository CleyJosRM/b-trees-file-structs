#include "../../include/arvoreb/arvoreb_interna.h"

static void remover_entrada_da_folha(byteBTree* no, int idx){
    int num = get_nroChaves(no);
    for(int j = idx; j < num - 1; j++){
        set_chave(no, j, get_chave(no, j+1));
        set_RRNdados(no, j, get_RRNdados(no, j+1)); 
    }
    set_chave(no, num - 1, -1);
    set_RRNdados(no, num - 1, -1);
    set_nroChaves(no, num - 1);
}

static void remover_entrada_do_interno(byteBTree* no, int idx){
    int num = get_nroChaves(no);
    for(int j = idx; j < num - 1; j++){
        set_chave(no, j, get_chave(no, j+1));
        set_RRNdados(no, j, get_RRNdados(no, j+1));
    }
    for(int j = idx + 1; j <= num; j++){
        set_filho(no, j, get_filho(no, j+1));
    }
    set_filho(no, num, -1);
    set_chave(no, num - 1, -1);
    set_RRNdados(no, num - 1, -1);
    set_nroChaves(no, num - 1);
}

static int obter_folha_sucessora(FILE* arvoreB, int rrnInicial, byteBTree* folha, int* saidaIdx){
    int rrnAtual = rrnInicial;
    carregar_no(folha, arvoreB, rrnAtual);
    while(!no_eh_folha(folha)){
        rrnAtual = get_filho(folha, 0);
        carregar_no(folha, arvoreB, rrnAtual);
    }
    *saidaIdx = 0;
    return rrnAtual;
}

static void pegar_emprestado_da_direita(FILE* arvoreB, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* filho, int rrnFilho, byteBTree* irmao, int rrnIrmao){
    int chavesFilho = get_nroChaves(filho);
    int chavePai = get_chave(pai, idxFilho);
    int rrnDadosPai = get_RRNdados(pai, idxFilho);
    int chaveIrmao = get_chave(irmao, 0);
    int rrnDadosIrmao = get_RRNdados(irmao, 0);
    int filho0Irmao = get_filho(irmao, 0);
    int filho1Irmao = get_filho(irmao, 1); // P2 de irmao: será o novo P1 de irmao após a rotação

    if(no_eh_folha(filho)){
        set_chave(filho, chavesFilho, chavePai);
        set_RRNdados(filho, chavesFilho, rrnDadosPai);
        set_nroChaves(filho, chavesFilho + 1);
        set_chave(pai, idxFilho, chaveIrmao);
        set_RRNdados(pai, idxFilho, rrnDadosIrmao);
        remover_entrada_da_folha(irmao, 0);
    } else {
        set_chave(filho, chavesFilho, chavePai);
        set_RRNdados(filho, chavesFilho, rrnDadosPai);
        set_filho(filho, chavesFilho + 1, filho0Irmao);
        set_nroChaves(filho, chavesFilho + 1);
        set_chave(pai, idxFilho, chaveIrmao);
        set_RRNdados(pai, idxFilho, rrnDadosIrmao);
        remover_entrada_do_interno(irmao, 0);
        // O novo P1 de irmao deve ser o antigo P2.
        set_filho(irmao, 0, filho1Irmao);
    }

    armazenar_no(arvoreB, filho, rrnFilho);
    armazenar_no(arvoreB, irmao, rrnIrmao);
    armazenar_no(arvoreB, pai, rrnPai);
}

static void pegar_emprestado_da_esquerda(FILE* arvoreB, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* filho, int rrnFilho, byteBTree* irmao, int rrnIrmao){
    int chavesFilho = get_nroChaves(filho);
    int chavePai = get_chave(pai, idxFilho - 1);
    int rrnDadosPai = get_RRNdados(pai, idxFilho - 1);
    int ultimoIrmao = get_nroChaves(irmao) - 1;
    int chaveIrmao = get_chave(irmao, ultimoIrmao);
    int rrnDadosIrmao = get_RRNdados(irmao, ultimoIrmao);
    int ultimoFilhoIrmao = get_filho(irmao, ultimoIrmao + 1);

    if(no_eh_folha(filho)){
        for(int j = chavesFilho; j > 0; j--){
            set_chave(filho, j, get_chave(filho, j-1));
            set_RRNdados(filho, j, get_RRNdados(filho, j-1));
        }
        set_chave(filho, 0, chavePai);
        set_RRNdados(filho, 0, rrnDadosPai);
        set_nroChaves(filho, chavesFilho + 1);
        set_chave(pai, idxFilho - 1, chaveIrmao);
        set_RRNdados(pai, idxFilho - 1, rrnDadosIrmao);
        remover_entrada_da_folha(irmao, ultimoIrmao);
    } else {
        for(int j = chavesFilho; j > 0; j--){
            set_chave(filho, j, get_chave(filho, j-1));
            set_RRNdados(filho, j, get_RRNdados(filho, j-1));
        }
        for(int j = chavesFilho + 1; j > 0; j--){
            set_filho(filho, j, get_filho(filho, j-1));
        }
        set_chave(filho, 0, chavePai);
        set_RRNdados(filho, 0, rrnDadosPai);
        set_filho(filho, 0, ultimoFilhoIrmao);
        set_nroChaves(filho, chavesFilho + 1);
        set_chave(pai, idxFilho - 1, chaveIrmao);
        set_RRNdados(pai, idxFilho - 1, rrnDadosIrmao);
        remover_entrada_do_interno(irmao, ultimoIrmao);
    }

    armazenar_no(arvoreB, filho, rrnFilho);
    armazenar_no(arvoreB, irmao, rrnIrmao);
    armazenar_no(arvoreB, pai, rrnPai);
}

static void fundir_com_esquerda(FILE* arvoreB, byteBTree* cabecalho, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* esquerda, int rrnEsquerda, byteBTree* direita, int rrnDireita){
    int chavesEsquerda = get_nroChaves(esquerda);
    int chavePai = get_chave(pai, idxFilho - 1);
    int rrnDadosPai = get_RRNdados(pai, idxFilho - 1);

    set_chave(esquerda, chavesEsquerda, chavePai);
    set_RRNdados(esquerda, chavesEsquerda, rrnDadosPai);
    chavesEsquerda++;

    int chavesDireita = get_nroChaves(direita);
    for(int j = 0; j < chavesDireita; j++){
        set_chave(esquerda, chavesEsquerda + j, get_chave(direita, j));
        set_RRNdados(esquerda, chavesEsquerda + j, get_RRNdados(direita, j));
    }

    if(!no_eh_folha(esquerda)){
        set_filho(esquerda, chavesEsquerda, get_filho(direita, 0));
        for(int j = 1; j <= chavesDireita; j++){
            set_filho(esquerda, chavesEsquerda + j, get_filho(direita, j));
        }
    }

    set_nroChaves(esquerda, chavesEsquerda + chavesDireita);
    remover_entrada_do_interno(pai, idxFilho - 1);
    empilhar_pagina_livre(arvoreB, cabecalho, rrnDireita);
    armazenar_no(arvoreB, esquerda, rrnEsquerda);
    armazenar_no(arvoreB, pai, rrnPai);
}

static void fundir_com_direita(FILE* arvoreB, byteBTree* cabecalho, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* filho, int rrnFilho, byteBTree* direita, int rrnDireita){
    int chavesFilho = get_nroChaves(filho);
    int chavePai = get_chave(pai, idxFilho);
    int rrnDadosPai = get_RRNdados(pai, idxFilho);

    set_chave(filho, chavesFilho, chavePai);
    set_RRNdados(filho, chavesFilho, rrnDadosPai);
    chavesFilho++;

    int chavesDireita = get_nroChaves(direita);
    for(int j = 0; j < chavesDireita; j++){
        set_chave(filho, chavesFilho + j, get_chave(direita, j));
        set_RRNdados(filho, chavesFilho + j, get_RRNdados(direita, j));
    }

    if(!no_eh_folha(filho)){
        set_filho(filho, chavesFilho, get_filho(direita, 0));
        for(int j = 1; j <= chavesDireita; j++){
            set_filho(filho, chavesFilho + j, get_filho(direita, j));
        }
    }

    set_nroChaves(filho, chavesFilho + chavesDireita);
    remover_entrada_do_interno(pai, idxFilho);
    empilhar_pagina_livre(arvoreB, cabecalho, rrnDireita);
    armazenar_no(arvoreB, filho, rrnFilho);
    armazenar_no(arvoreB, pai, rrnPai);
}

static void corrigir_underflow(FILE* arvoreB, byteBTree* cabecalho, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* filho, int rrnFilho){
    int numPais = get_nroChaves(pai);
    int chavesMinimas = NUM_MIN_CHAVES;

    if(get_nroChaves(filho) >= chavesMinimas) return;

    if(idxFilho < numPais){
        int rrnDireita = get_filho(pai, idxFilho + 1);
        byteBTree direita[TAM_NO_BTREE];
        carregar_no(direita, arvoreB, rrnDireita);
        if(no_pode_emprestar(direita)){
            pegar_emprestado_da_direita(arvoreB, pai, rrnPai, idxFilho, filho, rrnFilho, direita, rrnDireita);
            return;
        }
    }

    if(idxFilho > 0){
        int rrnEsquerda = get_filho(pai, idxFilho - 1);
        byteBTree esquerda[TAM_NO_BTREE];
        carregar_no(esquerda, arvoreB, rrnEsquerda);
        if(no_pode_emprestar(esquerda)){
            pegar_emprestado_da_esquerda(arvoreB, pai, rrnPai, idxFilho, filho, rrnFilho, esquerda, rrnEsquerda);
            return;
        }
    }

    if(idxFilho > 0){
        int rrnEsquerda = get_filho(pai, idxFilho - 1);
        byteBTree esquerda[TAM_NO_BTREE];
        carregar_no(esquerda, arvoreB, rrnEsquerda);
        fundir_com_esquerda(arvoreB, cabecalho, pai, rrnPai, idxFilho, esquerda, rrnEsquerda, filho, rrnFilho);
    } else {
        int rrnDireita = get_filho(pai, idxFilho + 1);
        byteBTree direita[TAM_NO_BTREE];
        carregar_no(direita, arvoreB, rrnDireita);
        fundir_com_direita(arvoreB, cabecalho, pai, rrnPai, idxFilho, filho, rrnFilho, direita, rrnDireita);
    }
}

static bool remover_chave_rec_b(FILE* arvoreB, byteBTree* cabecalho, byteBTree* noAtual, int rrnAtual, int chave){
    int num = get_nroChaves(noAtual);
    int idx = 0;
    while(idx < num && get_chave(noAtual, idx) < chave){
        idx++;
    }

    bool encontrado = (idx < num && get_chave(noAtual, idx) == chave);

    if(encontrado){
        if(no_eh_folha(noAtual)){
            remover_entrada_da_folha(noAtual, idx);
            armazenar_no(arvoreB, noAtual, rrnAtual);
            return true;
        }

        // Obtemos o filho direto à direita da chave removida.
        // obter_folha_sucessora desce por ele até a folha para ler a chave sucessora,
        // mas não usamos o buffer da folha para a recursão, passamos o filho direto.
        // Isso garante que corrigir_underflow sempre recebe um filho direto de noAtual,
        // evitando que ele tente carregar irmãos com RRN inválido.
        int rrnFilhoDir = get_filho(noAtual, idx + 1);
        byteBTree filhoDir[TAM_NO_BTREE];
        carregar_no(filhoDir, arvoreB, rrnFilhoDir);

        // Lê a chave sucessora (menor chave do sub-filho direito) sem alterar nada ainda.
        byteBTree folhaSuc[TAM_NO_BTREE];
        int idxSucessor;
        (void)obter_folha_sucessora(arvoreB, rrnFilhoDir, folhaSuc, &idxSucessor);
        int chaveSucessora   = get_chave(folhaSuc, idxSucessor);
        int rrnDadosSucessor = get_RRNdados(folhaSuc, idxSucessor);

        // Substitui a chave removida pela sua sucessora e salva o nó atual.
        set_chave(noAtual, idx, chaveSucessora);
        set_RRNdados(noAtual, idx, rrnDadosSucessor);
        armazenar_no(arvoreB, noAtual, rrnAtual);

        // Desce recursivamente pelo filho direto para remover a chave sucessora.
        // A recursão chegará à folha correta e propagará underflow de baixo para cima.
        bool removido = remover_chave_rec_b(arvoreB, cabecalho, filhoDir, rrnFilhoDir, chaveSucessora);

        // Recarrega filhoDir do disco, pois a recursão pode tê-lo modificado.
        carregar_no(filhoDir, arvoreB, rrnFilhoDir);
        if(removido && get_nroChaves(filhoDir) < NUM_MIN_CHAVES){
            corrigir_underflow(arvoreB, cabecalho, noAtual, rrnAtual, idx + 1, filhoDir, rrnFilhoDir);
        }
        return removido;
    }

    if(no_eh_folha(noAtual)){
        return false;
    }

    int rrnFilho = get_filho(noAtual, idx);
    byteBTree filho[TAM_NO_BTREE];
    carregar_no(filho, arvoreB, rrnFilho);
    bool removido = remover_chave_rec_b(arvoreB, cabecalho, filho, rrnFilho, chave);
    if(removido && get_nroChaves(filho) < NUM_MIN_CHAVES){
        corrigir_underflow(arvoreB, cabecalho, noAtual, rrnAtual, idx, filho, rrnFilho);
    }
    return removido;
}

static void corrigir_raiz(FILE* arvoreB, byteBTree* cabecalho){
    int rrnRaiz = get_RRNraiz(cabecalho);
    if(rrnRaiz == -1) return;

    byteBTree raiz[TAM_NO_BTREE];
    carregar_no(raiz, arvoreB, rrnRaiz);

    if(get_nroChaves(raiz) > 0) return;

    if(no_eh_folha(raiz)){
        empilhar_pagina_livre(arvoreB, cabecalho, rrnRaiz);
        set_inteiro(cabecalho, BO_RRNraiz, -1);
        return;
    }

    int rrnNovaRaiz = get_filho(raiz, 0);
    if(rrnNovaRaiz == -1) return;

    empilhar_pagina_livre(arvoreB, cabecalho, rrnRaiz);
    *(int*)&cabecalho[1] = rrnNovaRaiz; // Atualiza o RRNraiz no cabeçalho

    byteBTree novaRaiz[TAM_NO_BTREE];
    carregar_no(novaRaiz, arvoreB, rrnNovaRaiz);
    *(int*)&novaRaiz[5] = TIPORAIZ;   // byte offset 5 = tipoNo
    armazenar_no(arvoreB, novaRaiz, rrnNovaRaiz);
}

void remover_chave_arvoreB(FILE* arvoreB, int chave){
    byteBTree cabecalho[TAM_CABECALHO_BTREE];
    carregar_cabecalho(cabecalho, arvoreB, true);

    int rrnRaiz = get_RRNraiz(cabecalho);
    if(rrnRaiz == -1){
        return;
    }

    byteBTree raiz[TAM_NO_BTREE];
    carregar_no(raiz, arvoreB, rrnRaiz);
    
    if(remover_chave_rec_b(arvoreB, cabecalho, raiz, rrnRaiz, chave)){
        corrigir_raiz(arvoreB, cabecalho);
        armazenar_cabecalho(arvoreB, cabecalho);
    }
}