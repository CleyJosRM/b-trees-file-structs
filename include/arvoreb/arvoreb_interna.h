/**
 * ============================================================================
 * @file        arvoreb_interna.h
 * @brief       Interface interna de implementação da Árvore-B
 *
 * @author      Cleyton Jose Rodrigues Macedo
 * @author      Guilherme Cavalcanti de Santana
 *
 * @details     Define estruturas internas, macros de configuração e funções
 *              privadas necessárias para implementação da Árvore-B. Este
 *              header deve ser incluído apenas por arquivos de implementação
 *              internos (Árvore-B como um TAD com Information Hiding).
 * ============================================================================
 */

#ifndef ARVOREB_INTERNA_H
#define ARVOREB_INTERNA_H

#include "core/definicoes.h"
#include "indice.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* ========================================================================== */
/*                      MACROS DE CONFIGURAÇÃO DA B-TREE                      */
/* ========================================================================== */

/** @brief Ordem da Árvore-B*/
#define ORDEM_BTREE 4

/** @brief Alias m para ORDEM_BTREE */
#define m ORDEM_BTREE

/**
 * @brief Número mínimo de chaves em um nó (exceto raiz)
 *
 * Fórmula: (m+1)/2 - 1
 */
#define NUM_MIN_CHAVES (m + 1) / 2 - 1

/* ========================================================================== */
/*                       CÓDIGOS DE STATUS E TIPOS                            */
/* ========================================================================== */

/** @brief Status de arquivo inconsistente */
#define INCONSISTENTE '0'

/** @brief Status de arquivo consistente */
#define CONSISTENTE '1'

/** @brief Tipo de nó: raiz da árvore */
#define TIPORAIZ 0

/** @brief Tipo de nó: nó intermediário (nem raiz nem folha) */
#define TIPOINTERMEDIARIO 1

/** @brief Tipo de nó: folha (sem filhos) */
#define TIPOFOLHA -1

/* ========================================================================== */
/*                     LAYOUT DO CABEÇALHO DA ÁRVORE-B                        */
/* ========================================================================== */

/** @brief Tamanho total do cabeçalho em bytes */
#define TAM_CABECALHO_BTREE 17

/** @brief [0] Byte offset: status (1 byte: '0' ou '1') */
#define BO_status 0

/** @brief [1-4] Byte offset: RRN da raiz (4 bytes, inteiro) */
#define BO_RRNraiz 1

/** @brief [5-8] Byte offset: topo da pilha de nós removidos (4 bytes, inteiro) */
#define BO_topo 5

/** @brief [9-12] Byte offset: próximo RRN a alocar (4 bytes, inteiro) */
#define BO_proxRRN 9

/** @brief [13-16] Byte offset: número total de nós ativos (4 bytes, inteiro) */
#define BO_nroNos 13

/* ========================================================================== */
/*                       LAYOUT DE NÓ DA ÁRVORE-B                             */
/* ========================================================================== */

/** @brief Tamanho total de um nó em bytes */
#define TAM_NO_BTREE 53

/** @brief Marcador para RRN inexistente/inválido */
#define NO_INEXISTENTE -1

/**
 * @brief [0] Byte offset: flag removido ('0'=ativo, '1'=removido)
 *
 * Usado para manter nós removidos em uma pilha de livres para reutilização
 */
#define BO_removido 0

/**
 * @brief [1-4] Byte offset: RRN do próximo nó na pilha de removidos
 *
 * Forma uma pilha (stack) de nós livres para reutilização
 */
#define BO_proximo 1

/** @brief [5-8] Byte offset: tipo do nó (TIPORAIZ, TIPOINTERMEDIARIO, TIPOFOLHA) */
#define BO_tipoNo 5

/** @brief [9-12] Byte offset: número de chaves armazenadas no nó */
#define BO_nroChaves 9

/**
 * @brief [13...] Byte offset da primeira chave (C1)
 *
 *   - Chave Ci: BO_C1 + 8*(i-1)
 */
#define BO_C1 13

/**
 * @brief Byte offset do primeiro ponteiro de dados (PR1)
 *
 *   - PRi = BO_PR1 + 8 * (i-1) para i = 1, 2, ..., nroChaves
 */
#define BO_PR1 (BO_C1 + 4)

/**
 * @brief Byte offset do primeiro ponteiro de filho (P1)
 *
 *  - Pi = BO_P1 + 4 * (i-1) para i = 1, 2, ..., nroChaves + 1
 */
#define BO_P1 (BO_C1 + 8 * (ORDEM_BTREE - 1))

/**
 * @brief Byte offset do último ponteiro de filho (Pm)
 *
 * Onde m = ORDEM_BTREE (máximo número de filhos possível)
 */
#define BO_Pm (BO_P1 + 4 * (ORDEM_BTREE - 1))

/* ========================================================================== */
/*                       TIPOS E ESTRUTURAS INTERNAS                          */
/* ========================================================================== */

/** @brief Tipo para representar um byte de uma árvore-B */
typedef unsigned char byteBTree;

/**
 * @struct  _entrada_indice_
 * @brief   Entrada (registro) em um nó da árvore-B
 */
typedef struct _entrada_indice_
{
    /** @brief Chave inteira de busca (única na árvore) */
    int chave;

    /** @brief Byte offset do registro correspondente no arquivo de dados */
    int BOdados;

    /** @brief RRN do nó filho descendente (nós não-folha); -1 para folhas */
    int RRNdescendente;
} ENTRADA_INDICE;

/* ========================================================================== */
/*               FUNÇÕES DE MANIPULAÇÃO DE DISCO (I/O PERSISTÊNCIA)           */
/* ========================================================================== */

/**
 * @brief   Escreve um nó da memória principal para o arquivo de índice
 *
 * @param[in]   arvoreB     Ponteiro FILE* do arquivo de índice
 * @param[in]   no          Ponteiro para buffer de nó (TAM_NO_BTREE bytes)
 * @param[in]   RRN         Posição do nó a ser escrito
 *
 * @details Posiciona o cursor em: TAM_CABECALHO_BTREE + TAM_NO_BTREE * RRN
 *          e escreve TAM_NO_BTREE bytes do buffer.
 */
void armazenar_no(FILE *arvoreB, const byteBTree *no, int RRN);

/**
 * @brief   Lê um nó do arquivo de índice para a memória principal
 *
 * @param[out]  buffer      Ponteiro para buffer de nó
 * @param[in]   arvoreB     Ponteiro FILE* do arquivo de índice
 * @param[in]   RRN         Número de referência relativo do nó a ler
 *
 * @details Posiciona o cursor em: TAM_CABECALHO_BTREE + TAM_NO_BTREE * RRN
 *          e lê TAM_NO_BTREE bytes para o buffer.
 */
void carregar_no(byteBTree *buffer, FILE *arvoreB, int RRN);

/**
 * @brief   Escreve o cabeçalho da árvore-B do disco para memória
 *
 * @param[in]   arvoreB     Ponteiro FILE* do arquivo de índice
 * @param[in]   buffer      Ponteiro para buffer de cabeçalho (TAM_CABECALHO_BTREE bytes)
 *
 * @details Posiciona o cursor no início (offset 0) e escreve
 *          TAM_CABECALHO_BTREE bytes.
 */
void armazenar_cabecalho(FILE *arvoreB, const byteBTree *buffer);

/**
 * @brief   Lê o cabeçalho da árvore-B do arquivo para memória
 *
 * @param[out]  buffer              Ponteiro para buffer de cabeçalho
 * @param[in]   arvoreB             Ponteiro FILE* do arquivo de índice
 * @param[in]   statusInconsistente Se true, marca arquivo como inconsistente
 *                                  antes de retornar
 *
 * @details Lê TAM_CABECALHO_BTREE bytes do início do arquivo.
 *          Se statusInconsistente, escreve INCONSISTENTE no byte 0 do arquivo
 */
void carregar_cabecalho(byteBTree *buffer, FILE *arvoreB, bool statusInconsistente);

/* ========================================================================== */
/*               FUNÇÕES DE ACESSO INLINE - MANIPULAÇÃO DE BYTES              */
/* ========================================================================== */

/**
 * @brief   Lê um inteiro (4 bytes) de um vetor
 *
 * @param[in]   vetor       Ponteiro para vetor de bytes
 * @param[in]   byteoffset  Posição inicial na memória
 *
 * @return  Inteiro lido (4 bytes) começando em byteoffset
 *
 * @remarks Sem verificação de limites. Chamador deve garantir que
 *          byteoffset + 4 <= tamanho do vetor.
 */
static inline int get_inteiro(const byteBTree *vetor, int byteoffset)
{
    return *(int *)&vetor[byteoffset];
}

/**
 * @brief   Escreve um inteiro (4 bytes) em um vetor
 *
 * @param[out]  vetor       Ponteiro para vetor de bytes
 * @param[in]   byteoffset  Posição inicial na memória
 * @param[in]   val         Inteiro a escrever
 *
 * @remarks Sem verificação de limites. Chamador deve garantir que
 *          byteoffset + 4 <= tamanho do vetor.
 */
static inline void set_inteiro(byteBTree *vetor, int byteoffset, int val)
{
    *(int *)&vetor[byteoffset] = val;
}

/**
 * @brief   Incrementa um inteiro (4 bytes) armazenado em um vetor
 *
 * @param[in,out]   vetor       Ponteiro para vetor de bytes
 * @param[in]       byteoffset  Posição inicial na memória
 * @param[in]       incremento  Valor a adicionar (pode ser negativo)
 *
 * @remarks Equivalente a: vetor[byteoffset:+4] += incremento
 *          Sem verificação de limites. Chamador deve garantir que
 *          byteoffset + 4 <= tamanho do vetor.
 */
static inline void inc_inteiro(byteBTree *vetor, int byteoffset, int incremento)
{
    *(int *)&vetor[byteoffset] += incremento;
}

/* ========================================================================== */
/*                   FUNÇÕES DE ACESSO AO CABEÇALHO                          */
/* ========================================================================== */

/**
 * @brief   Obtém o RRN da raiz da árvore do cabeçalho
 *
 * @param[in]   cabecalho   Ponteiro para buffer de cabeçalho
 *
 * @return  RRN da raiz (inteiro)
 * @return  -1 se árvore está vazia (sem raiz)
 */
int get_RRNraiz(byteBTree *cabecalho);

/**
 * @brief   Obtém o topo da pilha de nós removidos
 *
 * @param[in]   cabecalho   Ponteiro para buffer de cabeçalho
 *
 * @return  RRN do nó no topo da pilha de livres
 * @return  -1 se não há nós removidos disponíveis para reutilização
 */
int get_topo(byteBTree *cabecalho);

/**
 * @brief   Obtém o próximo RRN a alocar para novo nó
 *
 * @param[in]   cabecalho   Ponteiro para buffer de cabeçalho
 *
 * @return  Próximo RRN sequencial
 */
int get_proxRRN(byteBTree *cabecalho);

/**
 * @brief   Obtém o número total de nós ativos na árvore
 *
 * @param[in]   cabecalho   Ponteiro para buffer de cabeçalho
 *
 * @return  Número de nós (excluindo removidos)
 */
int get_nroNos(byteBTree *cabecalho);

/* ========================================================================== */
/*                      FUNÇÕES DE ACESSO AO NÓ                              */
/* ========================================================================== */

/**
 * @brief   Extrai um vetor de entradas de um nó
 *
 * @param[out]  vetorEntradas   Vetor pré-alocado para n entradas
 * @param[in]   no              Ponteiro para buffer de nó
 * @param[in]   n               Número de entradas a extrair
 *
 * @details Copia n entradas do nó para vetorEntradas. Se o vetor tiver
 *          espaço para mais entradas (preenchido anteriormente), elas
 *          são mantidas intactas.
 */
void get_entradas(ENTRADA_INDICE vetorEntradas[], const byteBTree *no, int n);

/**
 * @brief   Armazena um vetor de entradas em um nó
 *
 * @param[out]  no              Ponteiro para buffer de nó
 * @param[in]   vetorEntradas   Vetor de entradas a armazenar
 * @param[in]   n               Número de entradas a armazenar
 *
 * @details Copia n entradas para o nó. Posições restantes são preenchidas
 *          com entradas nulas. Atualiza automaticamente nroChaves.
 */
void set_entradas(byteBTree *no, const ENTRADA_INDICE vetorEntradas[], int n);

/**
 * @brief   Testa se um nó é uma folha (não tem filhos)
 *
 * @param[in]   no  Ponteiro para buffer de nó
 *
 * @return  true se nó é folha (TIPOFOLHA)
 * @return  false se nó é raiz ou intermediário
 */
bool no_eh_folha(byteBTree *no);

/**
 * @brief   Obtém o número de chaves armazenadas em um nó
 *
 * @param[in]   no  Ponteiro para buffer de nó
 *
 * @return  Número de chaves
 */
int get_nroChaves(byteBTree *no);

/**
 * @brief   Define o número de chaves em um nó
 *
 * @param[in,out]   no      Ponteiro para buffer de nó
 * @param[in]       n       Novo número de chaves
 */
void set_nroChaves(byteBTree *no, int n);

/**
 * @brief   Obtém a i-ésima chave de um nó
 *
 * @param[in]   no      Ponteiro para buffer de nó
 * @param[in]   idx     Índice da chave
 *
 * @return  Valor da chave
 */
int get_chave(byteBTree *no, int idx);

/**
 * @brief   Define a i-ésima chave de um nó
 *
 * @param[in,out]   no      Ponteiro para buffer de nó
 * @param[in]       idx     Índice da chave
 * @param[in]       chave   Valor a armazenar
 */
void set_chave(byteBTree *no, int idx, int chave);

/**
 * @brief   Obtém o byte offset de dados da i-ésima entrada
 *
 * @param[in]   no      Ponteiro para buffer de nó
 * @param[in]   idx     Índice da entrada
 *
 * @return  Byte offset no arquivo de dados
 */
int get_RRNdados(byteBTree *no, int idx);

/**
 * @brief   Define o byte offset de dados da i-ésima entrada
 *
 * @param[in,out]   no      Ponteiro para buffer de nó
 * @param[in]       idx     Índice da entrada
 * @param[in]       rrn     Byte offset a armazenar
 */
void set_RRNdados(byteBTree *no, int idx, int rrn);

/**
 * @brief   Obtém o i-ésimo ponteiro de filho (RRN)
 *
 * @param[in]   no      Ponteiro para buffer de nó
 * @param[in]   idx     Índice do filho
 *
 * @return  RRN do filho
 * @return  -1 se não há filho nessa posição
 */
int get_filho(byteBTree *no, int idx);

/**
 * @brief   Define o i-ésimo ponteiro de filho (RRN)
 *
 * @param[in,out]   no          Ponteiro para buffer de nó
 * @param[in]       idx         Índice do filho
 * @param[in]       rrnFilho    RRN a armazenar
 */
void set_filho(byteBTree *no, int idx, int rrnFilho);

/* ========================================================================== */
/*                   FUNÇÕES DE MANIPULAÇÃO DE ENTRADAS                      */
/* ========================================================================== */

/**
 * @brief   Cria uma entrada nula (marcador de não-existência)
 *
 * @return  ENTRADA_INDICE com chave, BOdados e RRNdescendente = -1
 *
 * @details Usada como marcador em operações de inserção para indicar
 *          ausência de promoção ou descendência.
 */
ENTRADA_INDICE get_entrada_nula(void);

/**
 * @brief   Testa se uma entrada é a entrada nula
 *
 * @param[in]   entrada Entrada a testar
 *
 * @return  true se entrada.chave == -1 && entrada.BOdados == -1 &&
 *               entrada.RRNdescendente == -1
 * @return  false caso contrário
 */
bool check_entrada_nula(ENTRADA_INDICE entrada);

/**
 * @brief   Comparador de entradas para qsort()
 *
 * @param[in]   entrada1    Ponteiro para primeira entrada
 * @param[in]   entrada2    Ponteiro para segunda entrada
 *
 * @return  > 0 se entrada1->chave > entrada2->chave
 * @return  < 0 se entrada1->chave < entrada2->chave
 * @return  0 se entrada1->chave == entrada2->chave
 */
int comparar_entradas(const void *entrada1, const void *entrada2);

/**
 * @brief   Testa se um nó pode emprestar uma entrada para um irmão
 *
 * @param[in]   no  Ponteiro para buffer de nó
 *
 * @return  true se nó tem mais de NUM_MIN_CHAVES entradas
 * @return  false caso contrário
 */
bool no_pode_emprestar(byteBTree *no);

/* ========================================================================== */
/*                      FUNÇÕES DE CRIAÇÃO DE NÓS                            */
/* ========================================================================== */

/**
 * @brief   Cria e inicializa um novo nó na árvore-B
 *
 * @param[out]  novoNo      Ponteiro para buffer de nó (não inicializado)
 * @param[in]   arvoreB     Ponteiro FILE* do arquivo de índice
 * @param[in,out] cabecalho  Cabeçalho da árvore (será atualizado)
 * @param[in]   tipoNo      Tipo do nó a criar (TIPORAIZ, TIPOINTERMEDIARIO,
 *                          ou TIPOFOLHA)
 *
 * @return  RRN do novo nó
 *
 * @details Aloca um RRN reutilizando da pilha de removidos se disponível,
 *          caso contrário usa proxRRN sequencial. Atualiza contadores no
 *          cabeçalho (topo, proxRRN, nroNos).
 *
 * @remarks Não escreve o nó em disco. Chamador é responsável por chamar
 *          armazenar_no().
 */
int criar_no(byteBTree *novoNo, FILE *arvoreB, byteBTree *cabecalho, int tipoNo);

/**
 * @brief   Marca um nó como removido e o adiciona à pilha de livres
 *
 * @param[in]   arvoreB     Ponteiro FILE* do arquivo de índice
 * @param[in,out] cabecalho  Cabeçalho da árvore (será atualizado)
 * @param[in]   rrnPagina   RRN do nó a marcar como livre
 *
 * @details Coloca o nó no topo da pilha de nós removidos (LIFO) para
 *          reutilização futura. Decrementa contador de nós ativos.
 *          O conteúdo anterior do nó é preservado.
 */
void empilhar_pagina_livre(FILE *arvoreB, byteBTree *cabecalho, int rrnPagina);

/* ========================================================================== */
/*                    FUNÇÕES DE NAVEGAÇÃO EM NÓ                             */
/* ========================================================================== */

/**
 * @brief   Busca uma posição em um nó para inserir ou descer na busca
 *
 * @param[out]  ret         Ponteiro para inteiro de saída
 * @param[in]   no          Ponteiro para buffer de nó
 * @param[in]   chaveBusca  Chave a procurar
 *
 * @return  true se chaveBusca foi encontrada no nó
 * @return  false se não foi encontrada
 *
 * @details Se encontrada, *ret contém byte offset (BO) dos dados.
 *          Se não encontrada, *ret contém RRN do nó filho onde chaveBusca
 *          pode estar (-1 se não há filho).
 */
bool percorrer_no(int *ret, const byteBTree *no, int chaveBusca);

/* ========================================================================== */
/*                              FUNÇÕES DE DEBUG                              */
/* ========================================================================== */

/**
 * @brief   Imprime conteúdo de um nó em formato hexadecimal
 *
 * @param[in]   no  Ponteiro para buffer de nó
 *
 * @details Útil para debbug em nível de memória durante desenvolvimento
 */
void imprimir_no_bytes(byteBTree *no);

/**
 * @brief   Imprime conteúdo de um nó em formato legível
 *
 * @param[in]   noB Ponteiro para buffer de nó
 * @param[in]   RRN Número de referência do nó (para identificação na saída)
 *
 * @details Mostra tipo, número de chaves, valores das chaves e RRNs
 */
void imprimir_no(byteBTree *noB, int RRN);

/**
 * @brief   Imprime conteúdo do cabeçalho da árvore
 *
 * @param[in]   cabecalho   Ponteiro para buffer de cabeçalho
 *
 * @details Mostra status, RRN raiz, topo da pilha, proxRRN, número de nós
 */
void imprimir_cabecalho(byteBTree *cabecalho);

/* ========================================================================== */

#endif /* ARVOREB_INTERNA_H */