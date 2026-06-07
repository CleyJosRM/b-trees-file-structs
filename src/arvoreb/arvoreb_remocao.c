#include "../include/arvoreb/arvoreb_interna.h"

static void remover_entrada_da_folha(byteNoB* no, int idx){
    int num = no_obter_num_chaves(no);
    for(int j = idx; j < num - 1; j++){
        no_definir_chave(no, j, no_obter_chave(no, j+1));
        no_definir_RRNdados(no, j, no_obter_RRNdados(no, j+1));
    }
    no_definir_chave(no, num - 1, -1);
    no_definir_RRNdados(no, num - 1, -1);
    no_definir_num_chaves(no, num - 1);
}

static void remover_entrada_do_interno(byteNoB* no, int idx){
    int num = no_obter_num_chaves(no);
    for(int j = idx; j < num - 1; j++){
        no_definir_chave(no, j, no_obter_chave(no, j+1));
        no_definir_RRNdados(no, j, no_obter_RRNdados(no, j+1));
    }
    for(int j = idx + 1; j <= num; j++){
        no_definir_filho(no, j, no_obter_filho(no, j+1));
    }
    no_definir_filho(no, num, -1);
    no_definir_chave(no, num - 1, -1);
    no_definir_RRNdados(no, num - 1, -1);
    no_definir_num_chaves(no, num - 1);
}

static int obter_folha_sucessora(FILE* arvoreB, int rrnInicial, byteNoB* folha, int* saidaIdx){
    int rrnAtual = rrnInicial;
    carregar_no(arvoreB, rrnAtual, folha);
    while(!no_eh_folha(folha)){
        rrnAtual = no_obter_filho(folha, 0);
        carregar_no(arvoreB, rrnAtual, folha);
    }
    *saidaIdx = 0;
    return rrnAtual;
}

static void pegar_emprestado_da_direita(FILE* arvoreB, byteNoB* pai, int rrnPai, int idxFilho, byteNoB* filho, int rrnFilho, byteNoB* irmao, int rrnIrmao){
    int chavesFilho = no_obter_num_chaves(filho);
    int chavePai = no_obter_chave(pai, idxFilho);
    int rrnDadosPai = no_obter_RRNdados(pai, idxFilho);
    int chaveIrmao = no_obter_chave(irmao, 0);
    int rrnDadosIrmao = no_obter_RRNdados(irmao, 0);
    int filho0Irmao = no_obter_filho(irmao, 0);

    if(no_eh_folha(filho)){
        no_definir_chave(filho, chavesFilho, chavePai);
        no_definir_RRNdados(filho, chavesFilho, rrnDadosPai);
        no_definir_num_chaves(filho, chavesFilho + 1);
        no_definir_chave(pai, idxFilho, chaveIrmao);
        no_definir_RRNdados(pai, idxFilho, rrnDadosIrmao);
        remover_entrada_da_folha(irmao, 0);
    } else {
        no_definir_chave(filho, chavesFilho, chavePai);
        no_definir_RRNdados(filho, chavesFilho, rrnDadosPai);
        no_definir_filho(filho, chavesFilho + 1, filho0Irmao);
        no_definir_num_chaves(filho, chavesFilho + 1);
        no_definir_chave(pai, idxFilho, chaveIrmao);
        no_definir_RRNdados(pai, idxFilho, rrnDadosIrmao);
        remover_entrada_do_interno(irmao, 0);
    }

    armazenar_no(arvoreB, rrnFilho, filho);
    armazenar_no(arvoreB, rrnIrmao, irmao);
    armazenar_no(arvoreB, rrnPai, pai);
}

static void pegar_emprestado_da_esquerda(FILE* arvoreB, byteNoB* pai, int rrnPai, int idxFilho, byteNoB* filho, int rrnFilho, byteNoB* irmao, int rrnIrmao){
    int chavesFilho = no_obter_num_chaves(filho);
    int chavePai = no_obter_chave(pai, idxFilho - 1);
    int rrnDadosPai = no_obter_RRNdados(pai, idxFilho - 1);
    int ultimoIrmao = no_obter_num_chaves(irmao) - 1;
    int chaveIrmao = no_obter_chave(irmao, ultimoIrmao);
    int rrnDadosIrmao = no_obter_RRNdados(irmao, ultimoIrmao);
    int ultimoFilhoIrmao = no_obter_filho(irmao, ultimoIrmao + 1);

    if(no_eh_folha(filho)){
        for(int j = chavesFilho; j > 0; j--){
            no_definir_chave(filho, j, no_obter_chave(filho, j-1));
            no_definir_RRNdados(filho, j, no_obter_RRNdados(filho, j-1));
        }
        no_definir_chave(filho, 0, chavePai);
        no_definir_RRNdados(filho, 0, rrnDadosPai);
        no_definir_num_chaves(filho, chavesFilho + 1);
        no_definir_chave(pai, idxFilho - 1, chaveIrmao);
        no_definir_RRNdados(pai, idxFilho - 1, rrnDadosIrmao);
        remover_entrada_da_folha(irmao, ultimoIrmao);
    } else {
        for(int j = chavesFilho; j > 0; j--){
            no_definir_chave(filho, j, no_obter_chave(filho, j-1));
            no_definir_RRNdados(filho, j, no_obter_RRNdados(filho, j-1));
        }
        for(int j = chavesFilho + 1; j > 0; j--){
            no_definir_filho(filho, j, no_obter_filho(filho, j-1));
        }
        no_definir_chave(filho, 0, chavePai);
        no_definir_RRNdados(filho, 0, rrnDadosPai);
        no_definir_filho(filho, 0, ultimoFilhoIrmao);
        no_definir_num_chaves(filho, chavesFilho + 1);
        no_definir_chave(pai, idxFilho - 1, chaveIrmao);
        no_definir_RRNdados(pai, idxFilho - 1, rrnDadosIrmao);
        remover_entrada_do_interno(irmao, ultimoIrmao);
    }

    armazenar_no(arvoreB, rrnFilho, filho);
    armazenar_no(arvoreB, rrnIrmao, irmao);
    armazenar_no(arvoreB, rrnPai, pai);
}

static void fundir_com_esquerda(FILE* arvoreB, byteNoB* cabecalho, byteNoB* pai, int rrnPai, int idxFilho, byteNoB* esquerda, int rrnEsquerda, byteNoB* direita, int rrnDireita){
    int chavesEsquerda = no_obter_num_chaves(esquerda);
    int chavePai = no_obter_chave(pai, idxFilho - 1);
    int rrnDadosPai = no_obter_RRNdados(pai, idxFilho - 1);

    no_definir_chave(esquerda, chavesEsquerda, chavePai);
    no_definir_RRNdados(esquerda, chavesEsquerda, rrnDadosPai);
    chavesEsquerda++;

    int chavesDireita = no_obter_num_chaves(direita);
    for(int j = 0; j < chavesDireita; j++){
        no_definir_chave(esquerda, chavesEsquerda + j, no_obter_chave(direita, j));
        no_definir_RRNdados(esquerda, chavesEsquerda + j, no_obter_RRNdados(direita, j));
    }

    if(!no_eh_folha(esquerda)){
        no_definir_filho(esquerda, chavesEsquerda, no_obter_filho(direita, 0));
        for(int j = 1; j <= chavesDireita; j++){
            no_definir_filho(esquerda, chavesEsquerda + j, no_obter_filho(direita, j));
        }
    }

    no_definir_num_chaves(esquerda, chavesEsquerda + chavesDireita);
    remover_entrada_do_interno(pai, idxFilho - 1);
    empilhar_pagina_livre(arvoreB, cabecalho, rrnDireita);
    armazenar_no(arvoreB, rrnEsquerda, esquerda);
    armazenar_no(arvoreB, rrnPai, pai);
}

static void fundir_com_direita(FILE* arvoreB, byteNoB* cabecalho, byteNoB* pai, int rrnPai, int idxFilho, byteNoB* filho, int rrnFilho, byteNoB* direita, int rrnDireita){
    int chavesFilho = no_obter_num_chaves(filho);
    int chavePai = no_obter_chave(pai, idxFilho);
    int rrnDadosPai = no_obter_RRNdados(pai, idxFilho);

    no_definir_chave(filho, chavesFilho, chavePai);
    no_definir_RRNdados(filho, chavesFilho, rrnDadosPai);
    chavesFilho++;

    int chavesDireita = no_obter_num_chaves(direita);
    for(int j = 0; j < chavesDireita; j++){
        no_definir_chave(filho, chavesFilho + j, no_obter_chave(direita, j));
        no_definir_RRNdados(filho, chavesFilho + j, no_obter_RRNdados(direita, j));
    }

    if(!no_eh_folha(filho)){
        no_definir_filho(filho, chavesFilho, no_obter_filho(direita, 0));
        for(int j = 1; j <= chavesDireita; j++){
            no_definir_filho(filho, chavesFilho + j, no_obter_filho(direita, j));
        }
    }

    no_definir_num_chaves(filho, chavesFilho + chavesDireita);
    remover_entrada_do_interno(pai, idxFilho);
    empilhar_pagina_livre(arvoreB, cabecalho, rrnDireita);
    armazenar_no(arvoreB, rrnFilho, filho);
    armazenar_no(arvoreB, rrnPai, pai);
}

static void corrigir_underflow(FILE* arvoreB, byteNoB* cabecalho, byteNoB* pai, int rrnPai, int idxFilho, byteNoB* filho, int rrnFilho){
    int numPais = no_obter_num_chaves(pai);
    int chavesMinimas = num_minimo_chaves();

    if(no_obter_num_chaves(filho) >= chavesMinimas) return;

    if(idxFilho < numPais){
        int rrnDireita = no_obter_filho(pai, idxFilho + 1);
        byteNoB direita[TAM_NO_BTREE];
        carregar_no(arvoreB, rrnDireita, direita);
        if(no_pode_emprestar(direita)){
            pegar_emprestado_da_direita(arvoreB, pai, rrnPai, idxFilho, filho, rrnFilho, direita, rrnDireita);
            return;
        }
    }

    if(idxFilho > 0){
        int rrnEsquerda = no_obter_filho(pai, idxFilho - 1);
        byteNoB esquerda[TAM_NO_BTREE];
        carregar_no(arvoreB, rrnEsquerda, esquerda);
        if(no_pode_emprestar(esquerda)){
            pegar_emprestado_da_esquerda(arvoreB, pai, rrnPai, idxFilho, filho, rrnFilho, esquerda, rrnEsquerda);
            return;
        }
    }

    if(idxFilho > 0){
        int rrnEsquerda = no_obter_filho(pai, idxFilho - 1);
        byteNoB esquerda[TAM_NO_BTREE];
        carregar_no(arvoreB, rrnEsquerda, esquerda);
        fundir_com_esquerda(arvoreB, cabecalho, pai, rrnPai, idxFilho, esquerda, rrnEsquerda, filho, rrnFilho);
    } else {
        int rrnDireita = no_obter_filho(pai, idxFilho + 1);
        byteNoB direita[TAM_NO_BTREE];
        carregar_no(arvoreB, rrnDireita, direita);
        fundir_com_direita(arvoreB, cabecalho, pai, rrnPai, idxFilho, filho, rrnFilho, direita, rrnDireita);
    }
}

static bool remover_chave_rec_b(FILE* arvoreB, byteNoB* cabecalho, byteNoB* noAtual, int rrnAtual, int chave){
    int num = no_obter_num_chaves(noAtual);
    int idx = 0;
    while(idx < num && no_obter_chave(noAtual, idx) < chave){
        idx++;
    }

    bool encontrado = (idx < num && no_obter_chave(noAtual, idx) == chave);

    if(encontrado){
        if(no_eh_folha(noAtual)){
            remover_entrada_da_folha(noAtual, idx);
            armazenar_no(arvoreB, rrnAtual, noAtual);
            return true;
        }

        // Obtemos o filho direto à direita da chave removida.
        // obter_folha_sucessora desce por ele até a folha para ler a chave sucessora,
        // mas não usamos o buffer da folha para a recursão, passamos o filho direto.
        // Isso garante que corrigir_underflow sempre recebe um filho direto de noAtual,
        // evitando que ele tente carregar irmãos com RRN inválido.
        int rrnFilhoDir = no_obter_filho(noAtual, idx + 1);
        byteNoB filhoDir[TAM_NO_BTREE];
        carregar_no(arvoreB, rrnFilhoDir, filhoDir);

        // Lê a chave sucessora (menor chave do sub-filho direito) sem alterar nada ainda.
        byteNoB folhaSuc[TAM_NO_BTREE];
        int idxSucessor;
        (void)obter_folha_sucessora(arvoreB, rrnFilhoDir, folhaSuc, &idxSucessor);
        int chaveSucessora   = no_obter_chave(folhaSuc, idxSucessor);
        int rrnDadosSucessor = no_obter_RRNdados(folhaSuc, idxSucessor);

        // Substitui a chave removida pela sua sucessora e salva o nó atual.
        no_definir_chave(noAtual, idx, chaveSucessora);
        no_definir_RRNdados(noAtual, idx, rrnDadosSucessor);
        armazenar_no(arvoreB, rrnAtual, noAtual);

        // Desce recursivamente pelo filho direto para remover a chave sucessora.
        // A recursão chegará à folha correta e propagará underflow de baixo para cima.
        bool removido = remover_chave_rec_b(arvoreB, cabecalho, filhoDir, rrnFilhoDir, chaveSucessora);

        // Recarrega filhoDir do disco, pois a recursão pode tê-lo modificado.
        carregar_no(arvoreB, rrnFilhoDir, filhoDir);
        if(removido && no_obter_num_chaves(filhoDir) < num_minimo_chaves()){
            corrigir_underflow(arvoreB, cabecalho, noAtual, rrnAtual, idx + 1, filhoDir, rrnFilhoDir);
        }
        return removido;
    }

    if(no_eh_folha(noAtual)){
        return false;
    }

    int rrnFilho = no_obter_filho(noAtual, idx);
    byteNoB filho[TAM_NO_BTREE];
    carregar_no(arvoreB, rrnFilho, filho);
    bool removido = remover_chave_rec_b(arvoreB, cabecalho, filho, rrnFilho, chave);
    if(removido && no_obter_num_chaves(filho) < num_minimo_chaves()){
        corrigir_underflow(arvoreB, cabecalho, noAtual, rrnAtual, idx, filho, rrnFilho);
    }
    return removido;
}

static void corrigir_raiz(FILE* arvoreB, byteNoB* cabecalho){
    int rrnRaiz = get_RRNraiz(cabecalho);
    if(rrnRaiz == -1) return;

    byteNoB raiz[TAM_NO_BTREE];
    carregar_no(arvoreB, rrnRaiz, raiz);
    if(no_obter_num_chaves(raiz) > 0 || no_eh_folha(raiz)){
        return;
    }

    int rrnNovaRaiz = no_obter_filho(raiz, 0);
    if(rrnNovaRaiz == -1) return;

    empilhar_pagina_livre(arvoreB, cabecalho, rrnRaiz);
    *(int*)&cabecalho[1] = rrnNovaRaiz; // Atualiza o RRNraiz no cabeçalho

    byteNoB novaRaiz[TAM_NO_BTREE];
    carregar_no(arvoreB, rrnNovaRaiz, novaRaiz);
    *(int*)&novaRaiz[5] = TIPORAIZ;   // byte offset 5 = tipoNo
    armazenar_no(arvoreB, rrnNovaRaiz, novaRaiz);
}

void remover_chave_arvoreB(FILE* arvoreB, int chave){
    byteNoB cabecalho[TAM_CABECALHO_BTREE];
    carregar_cabecalho(arvoreB, cabecalho, true);

    int rrnRaiz = get_RRNraiz(cabecalho);
    if(rrnRaiz == -1){
        return;
    }

    byteNoB raiz[TAM_NO_BTREE];
    carregar_no(arvoreB, rrnRaiz, raiz);
    
    if(remover_chave_rec_b(arvoreB, cabecalho, raiz, rrnRaiz, chave)){
        corrigir_raiz(arvoreB, cabecalho);
        armazenar_cabecalho(arvoreB, cabecalho);
    }
}