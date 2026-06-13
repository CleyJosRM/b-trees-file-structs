/**
 * ============================================================================
 * @file        arvoreb_remocao.c
 * @brief       Implementação da funcionalidade de remoção em árvore-B, incluinddo todos seus algorítmos.
 *
 * @author      Cleyton Jose Rodrigues Macedo
 * @author      Guilherme Cavalcanti de Santana
 *
 * @details     Este módulo implementa a remoção de chaves em uma árvore-B armazenada em disco.
 * 
 * Fluxo geral:
 *  1. remover_chave_arvoreB          — ponto de entrada público
 *  2. remover_chave_rec_b            — desce recursivamente até o nó que contém
 *                                      a chave; decide entre remoção direta em
 *                                      folha ou substituição pelo sucessor em nó
 *                                      interno; na subida chama corrigir_underflow
 *  3. corrigir_underflow             — tenta rotação (empréstimo de irmão) e,
 *                                      se não for possível, faz fusão
 *  4. corrigir_raiz                  — após a recursão, verifica se a raiz ficou
 *                                      vazia e, se sim, promove o único filho
 * ============================================================================
 */

#include "../../include/arvoreb/arvoreb_interna.h"

/*
 * Remove a entrada de índice 'idx' de um nó FOLHA.
 * Desloca as entradas à direita de idx uma posição para a esquerda e
 * apaga (com -1) a última posição que ficou duplicada.
 * Ponteiros de filho não existem em folhas, portanto não são tocados.
 */
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

/*
 * Remove a entrada de índice 'idx' de um nó INTERNO.
 * Além de deslocar chaves/dados como na versão para folha, também desloca os
 * ponteiros de filho: os filhos à direita de idx+1 sobem uma posição, e o
 * último ponteiro é zerado com -1.
 */
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

/*
 * Desce pela sub-árvore enraizada em 'rrnInicial' pelo filho mais à
 * esquerda (P1) até alcançar uma folha. Essa folha contém a menor chave da
 * sub-árvore, que é o sucessor in-order da chave a ser removida.
 *
 * Saídas:
 *   folha     — buffer preenchido com o nó folha encontrado
 *   saidaIdx  — sempre 0 (o sucessor é sempre a primeira chave da folha mais
 *               à esquerda)
 *   return    — RRN da folha encontrada
 */
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


/* ---------------------------------------------------------------------------
 * ROTAÇÕES (EMPRÉSTIMO ENTRE IRMÃOS)
 *
 * Quando um nó fica com menos chaves do que o mínimo (underflow), tentamos
 * pegar emprestado uma chave de um irmão adjacente que tenha chaves suficientes.
 * A rotação não leva a chave diretamente do irmão para o filho deficiente,
 * ela passa pelo pai para manter a propriedade de ordenação da árvore-B:
 *
 *   Rotação à direita (empréstimo do irmão direito):
 *     chave do pai desce para o filho; menor chave do irmão sobe para o pai.
 *
 *   Rotação à esquerda (empréstimo do irmão esquerdo):
 *     chave do pai desce para o filho; maior chave do irmão sobe para o pai.
 * ------------------------------------------------------------------------- */

/* 
 * Rotação à direita: move uma chave do PAI para o FILHO deficiente e sobe
 * a menor chave do IRMÃO direito para o PAI.
 *
 * Em nós internos, o primeiro filho do irmão (P1_irmao) é adotado como último
 * filho do filho deficiente, pois ele é a sub-árvore que fica "entre" a chave
 * que desceu do pai e a chave que subiu do irmão.
 */
static void pegar_emprestado_da_direita(FILE* arvoreB, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* filho, int rrnFilho, byteBTree* irmao, int rrnIrmao){
    int chavesFilho = get_nroChaves(filho);
    int chavePai = get_chave(pai, idxFilho);
    int rrnDadosPai = get_RRNdados(pai, idxFilho);
    int chaveIrmao = get_chave(irmao, 0);
    int rrnDadosIrmao = get_RRNdados(irmao, 0);
    int filho0Irmao = get_filho(irmao, 0);
    int filho1Irmao = get_filho(irmao, 1); // será o novo P1 do irmão após a rotação

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
        set_filho(filho, chavesFilho + 1, filho0Irmao); // adota P1 do irmão
        set_nroChaves(filho, chavesFilho + 1);
        set_chave(pai, idxFilho, chaveIrmao);
        set_RRNdados(pai, idxFilho, rrnDadosIrmao);
        remover_entrada_do_interno(irmao, 0);
        set_filho(irmao, 0, filho1Irmao); // o antigo P2 torna-se o novo P1 do irmão
    }

    armazenar_no(arvoreB, filho, rrnFilho);
    armazenar_no(arvoreB, irmao, rrnIrmao);
    armazenar_no(arvoreB, pai, rrnPai);
}

/*
 * Rotação à esquerda: move uma chave do PAI para o FILHO deficiente e sobe
 * a maior chave do IRMÃO esquerdo para o PAI.
 *
 * As chaves existentes no filho precisam ser deslocadas uma posição à direita
 * antes de inserir a chave descendente do pai na posição 0. Em nós internos,
 * o mesmo deslocamento se aplica aos ponteiros de filho, e o último filho do
 * irmão é adotado como primeiro filho do filho deficiente.
 */
static void pegar_emprestado_da_esquerda(FILE* arvoreB, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* filho, int rrnFilho, byteBTree* irmao, int rrnIrmao){
    int chavesFilho = get_nroChaves(filho);
    int chavePai = get_chave(pai, idxFilho - 1);
    int rrnDadosPai = get_RRNdados(pai, idxFilho - 1);
    int ultimoIrmao = get_nroChaves(irmao) - 1;
    int chaveIrmao = get_chave(irmao, ultimoIrmao);
    int rrnDadosIrmao = get_RRNdados(irmao, ultimoIrmao);
    int ultimoFilhoIrmao = get_filho(irmao, ultimoIrmao + 1); // último filho do irmão

    if(no_eh_folha(filho)){
        for(int j = chavesFilho; j > 0; j--){  // abre espaço na posição 0
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
        for(int j = chavesFilho; j > 0; j--){  // abre espaço na posição 0 (chaves)
            set_chave(filho, j, get_chave(filho, j-1));
            set_RRNdados(filho, j, get_RRNdados(filho, j-1));
        }
        for(int j = chavesFilho + 1; j > 0; j--){  // abre espaço na posição 0 (filhos)
            set_filho(filho, j, get_filho(filho, j-1));
        }
        set_chave(filho, 0, chavePai);
        set_RRNdados(filho, 0, rrnDadosPai);
        set_filho(filho, 0, ultimoFilhoIrmao); // adota o último filho do irmão
        set_nroChaves(filho, chavesFilho + 1);
        set_chave(pai, idxFilho - 1, chaveIrmao);
        set_RRNdados(pai, idxFilho - 1, rrnDadosIrmao);
        remover_entrada_do_interno(irmao, ultimoIrmao);
    }

    armazenar_no(arvoreB, filho, rrnFilho);
    armazenar_no(arvoreB, irmao, rrnIrmao);
    armazenar_no(arvoreB, pai, rrnPai);
}


/* ---------------------------------------------------------------------------
 * FUSÕES (MERGE)
 *
 * Quando nenhum irmão tem chaves para emprestar, fundimos o nó deficiente com
 * um irmão adjacente. A chave do pai que separava os dois nós desce para o nó
 * resultante da fusão, e o nó que ficou vazio é liberado (empilhado como página livre).
 * A fusão pode propagar underflow para o pai, pois este perdeu uma chave.
 * ------------------------------------------------------------------------- */

/*
 * Funde o nó DIREITA (filho deficiente) no nó ESQUERDA (irmão à esquerda).
 * A chave separadora do pai (posição idxFilho-1) desce para esquerda.
 * Todas as chaves e filhos de direita são copiados para esquerda após a chave
 * do pai. O nó direita é então descartado.
 */
static void fundir_com_esquerda(FILE* arvoreB, byteBTree* cabecalho, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* esquerda, int rrnEsquerda, byteBTree* direita, int rrnDireita){
    int chavesEsquerda = get_nroChaves(esquerda);
    int chavePai = get_chave(pai, idxFilho - 1);
    int rrnDadosPai = get_RRNdados(pai, idxFilho - 1);

    // desce a chave separadora do pai para esquerda
    set_chave(esquerda, chavesEsquerda, chavePai);
    set_RRNdados(esquerda, chavesEsquerda, rrnDadosPai);
    chavesEsquerda++;

    // copia chaves (e filhos, se interno) de direita para esquerda
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
    remover_entrada_do_interno(pai, idxFilho - 1); // remove a chave separadora do pai
    empilhar_pagina_livre(arvoreB, cabecalho, rrnDireita); // libera o nó direita
    armazenar_no(arvoreB, esquerda, rrnEsquerda);
    armazenar_no(arvoreB, pai, rrnPai);
}

/*
 * Funde o nó DIREITA (irmão) no nó FILHO (deficiente).
 * Análogo a fundir_com_esquerda, porém o filho deficiente é o nó à esquerda.
 * A chave separadora do pai (posição idxFilho) desce para filho, e as chaves
 * de direita são acrescentadas em seguida. O nó direita é descartado.
 */
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


/* ---------------------------------------------------------------------------
 * CORREÇÃO DE UNDERFLOW
 * ------------------------------------------------------------------------- */

/*
 * Verifica e corrige underflow no nó FILHO após a remoção de uma chave.
 * Prioridade:
 *   1. Tentar rotação com irmão à DIREITA (se existir e puder emprestar).
 *   2. Tentar rotação com irmão à ESQUERDA (se existir e puder emprestar).
 *   3. Fundir com irmão à ESQUERDA (se existir).
 *   4. Fundir com irmão à DIREITA (último recurso).
 *
 * idxFilho é a posição do filho no vetor de ponteiros do pai, e não o índice
 * de chave.
 */
static void corrigir_underflow(FILE* arvoreB, byteBTree* cabecalho, byteBTree* pai, int rrnPai, int idxFilho, byteBTree* filho, int rrnFilho){
    int numPais = get_nroChaves(pai);
    int chavesMinimas = NUM_MIN_CHAVES;

    if(get_nroChaves(filho) >= chavesMinimas) return; // sem underflow

    // Rotação com irmão direito
    if(idxFilho < numPais){
        int rrnDireita = get_filho(pai, idxFilho + 1);
        byteBTree direita[TAM_NO_BTREE];
        carregar_no(direita, arvoreB, rrnDireita);
        if(no_pode_emprestar(direita)){
            pegar_emprestado_da_direita(arvoreB, pai, rrnPai, idxFilho, filho, rrnFilho, direita, rrnDireita);
            return;
        }
    }

    // Rotação com irmão esquerdo
    if(idxFilho > 0){
        int rrnEsquerda = get_filho(pai, idxFilho - 1);
        byteBTree esquerda[TAM_NO_BTREE];
        carregar_no(esquerda, arvoreB, rrnEsquerda);
        if(no_pode_emprestar(esquerda)){
            pegar_emprestado_da_esquerda(arvoreB, pai, rrnPai, idxFilho, filho, rrnFilho, esquerda, rrnEsquerda);
            return;
        }
    }

    // Nenhum irmão pode emprestar, então faz a fusão
    if(idxFilho > 0){
        // Prefere fundir com esquerda (filho deficiente é o nó direita da fusão)
        int rrnEsquerda = get_filho(pai, idxFilho - 1);
        byteBTree esquerda[TAM_NO_BTREE];
        carregar_no(esquerda, arvoreB, rrnEsquerda);
        fundir_com_esquerda(arvoreB, cabecalho, pai, rrnPai, idxFilho, esquerda, rrnEsquerda, filho, rrnFilho);
    } else {
        // Filho deficiente é o nó mais à esquerda (funde com o irmão à direita)
        int rrnDireita = get_filho(pai, idxFilho + 1);
        byteBTree direita[TAM_NO_BTREE];
        carregar_no(direita, arvoreB, rrnDireita);
        fundir_com_direita(arvoreB, cabecalho, pai, rrnPai, idxFilho, filho, rrnFilho, direita, rrnDireita);
    }
}


/* ---------------------------------------------------------------------------
 * PÓS-PROCESSAMENTO DA RAIZ
 * ------------------------------------------------------------------------- */

/*
 * Verifica se a raiz ficou vazia após a remoção e ajusta o cabeçalho:
 *     Raiz vazia e folha: árvore ficou completamente vazia; libera o nó e
 *     seta RRNraiz = -1.
 * 
 *     Raiz vazia e interna: a árvore reduziu um nível; o único filho
 *     torna-se a nova raiz (marcado com TIPORAIZ).
 */
static void corrigir_raiz(FILE* arvoreB, byteBTree* cabecalho){
    int rrnRaiz = get_RRNraiz(cabecalho);
    if(rrnRaiz == -1) return;

    byteBTree raiz[TAM_NO_BTREE];
    carregar_no(raiz, arvoreB, rrnRaiz);

    if(get_nroChaves(raiz) > 0) return; // raiz ainda tem chaves, nada a fazer

    if(no_eh_folha(raiz)){
        // Árvore esvaziou completamente
        empilhar_pagina_livre(arvoreB, cabecalho, rrnRaiz);
        set_inteiro(cabecalho, BO_RRNraiz, -1);
        return;
    }

    // Raiz interna vazia: promove o único filho como nova raiz
    int rrnNovaRaiz = get_filho(raiz, 0);
    if(rrnNovaRaiz == -1) return;

    empilhar_pagina_livre(arvoreB, cabecalho, rrnRaiz);
    set_inteiro(cabecalho, BO_RRNraiz, rrnNovaRaiz); // Atualiza o RRNraiz no cabeçalho

    byteBTree novaRaiz[TAM_NO_BTREE];
    carregar_no(novaRaiz, arvoreB, rrnNovaRaiz);
    set_inteiro(novaRaiz, BO_tipoNo, TIPORAIZ); // Marca a nova raiz
    armazenar_no(arvoreB, novaRaiz, rrnNovaRaiz);
}

/* ---------------------------------------------------------------------------
 * REMOÇÃO RECURSIVA
 * ------------------------------------------------------------------------- */

/*
 * Desce recursivamente a árvore procurando por 'chave'. Ao encontrá-la:
 *
 *   Se nó folha: remove diretamente com remover_entrada_da_folha.
 *
 *   Se nó interno: substitui a chave pelo seu sucessor e desce
 *      recursivamente para remover o sucessor de onde ele estava.
 *      Isso sempre resulta em uma remoção numa folha, simplificando a lógica.
 *
 * Na subida da recursão, verifica se o filho que foi visitado sofreu underflow
 * e chama corrigir_underflow se necessário.
 *
 * Retorna true se a chave foi encontrada e removida, false caso contrário.
 */
static bool remover_chave_rec_b(FILE* arvoreB, byteBTree* cabecalho, byteBTree* noAtual, int rrnAtual, int chave){
    int num = get_nroChaves(noAtual);
    int idx = 0;
    // avança idx até a primeira chave >= chave (busca linear)
    while(idx < num && get_chave(noAtual, idx) < chave){
        idx++;
    }

    bool encontrado = (idx < num && get_chave(noAtual, idx) == chave);

    if(encontrado){
        if(no_eh_folha(noAtual)){
            // Primeiro caso: remoção direta na folha
            remover_entrada_da_folha(noAtual, idx);
            armazenar_no(arvoreB, noAtual, rrnAtual);
            return true;
        }

        // Segundo caso: nó interno -> substituição pelo sucessor
        //
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
        return false; // chave não existe na árvore
    }

    // Chave não está neste nó: desce pelo filho adequado
    int rrnFilho = get_filho(noAtual, idx);
    byteBTree filho[TAM_NO_BTREE];
    carregar_no(filho, arvoreB, rrnFilho);
    bool removido = remover_chave_rec_b(arvoreB, cabecalho, filho, rrnFilho, chave);
    if(removido && get_nroChaves(filho) < NUM_MIN_CHAVES){
        corrigir_underflow(arvoreB, cabecalho, noAtual, rrnAtual, idx, filho, rrnFilho);
    }
    return removido;
}



/* ---------------------------------------------------------------------------
 * PONTO DE ENTRADA
 * ------------------------------------------------------------------------- */

/*
 * Remove a entrada com a chave primária 'chave' (codEstacao) da árvore-B em disco.
 * Marca o arquivo como inconsistente antes de iniciar (carregar_cabecalho com
 * statusInconsistente=true) e o restaura ao final com armazenar_cabecalho,
 * garantindo que interrupções não corrompam silenciosamente o índice.
 */
void remover_chave_arvoreB(FILE* arvoreB, int chave){
    byteBTree cabecalho[TAM_CABECALHO_BTREE];
    carregar_cabecalho(cabecalho, arvoreB, false);

    int rrnRaiz = get_RRNraiz(cabecalho);
    if(rrnRaiz == -1){
        return; // árvore vazia
    }

    byteBTree raiz[TAM_NO_BTREE];
    carregar_no(raiz, arvoreB, rrnRaiz);
    
    if(remover_chave_rec_b(arvoreB, cabecalho, raiz, rrnRaiz, chave)){
        corrigir_raiz(arvoreB, cabecalho);
        armazenar_cabecalho(arvoreB, cabecalho);
    }
}