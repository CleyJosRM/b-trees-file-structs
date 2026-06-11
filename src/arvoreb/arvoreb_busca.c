#include "../../include/arvoreb/arvoreb_interna.h"

/**
 * Busca uma chave em um nó da árvore B em memória principal.
 * Retorna se a chave foi encontrada ou não, e escreve um inteiro no endereço fornecido.
 * Se a chave foi encontrada, esse inteiro é o byte offset de dados.
 * Se a chave não foi encontrada, esse inteiro é o RRN do nó descendente onde a chaveBusca pode estar.
 */
bool percorrer_no(int* ret, const byteBTree* no, int chaveBusca){

        int chaveAtual = get_inteiro(no, BO_C1); // C1
        int RRNdescendenteAtual = get_inteiro(no, BO_P1); // P1
        int BOdados; // será inicializado só se achar a chave de busca
        bool achou = false;

        // O loop a seguir percorre o nó chave por chave procurando por chaveBusca. 
        // Se encontrar, obtém o byteoffset do registro de dados. 
        // Senão, obtém o RRN do nó descendente onde a chaveBusca pode estar

        int nroChaves = get_inteiro(no, BO_nroChaves); // o loop será executado no máximo tantas vezes quanto for o número de chaves no nó
        for(int i=1; i<=nroChaves && chaveAtual <= chaveBusca; i++){ // enquanto C_i <= chaveBusca
            if(chaveAtual == chaveBusca){ // se encontrou a chaveBusca
                BOdados = get_inteiro(no, BO_PR1+8*i-8); // PR1, PR2, PR3
                achou = true;
                break;
            }
            chaveAtual = get_inteiro(no, BO_C1+8*i); // C2, C3, dontcare
            RRNdescendenteAtual = get_inteiro(no, BO_P1+4*i); // P2, P3, P4
        }

        if(achou){
            *ret = BOdados; 
        }else{
            *ret = RRNdescendenteAtual;
        }
        return achou;
}

/**
 * Percorre o nó fornecido procurando pela chaveBusca. 
 * Se encontrar, retorna o byteoffset do registro de dados com tal chave.
 * Se não encontrar, continua a busca no nó descendente.
 * Se não haver nó descendente, retorna -1 para indicar que a chaveBusca não está na árvore-B.
 */
int buscar_chave_rec(FILE* arvoreB, byteBTree* no, int chaveBusca){

    int BO_ou_RRN; // variável inteira inicializada pela função percorrer_no
    if ( percorrer_no(&BO_ou_RRN, no, chaveBusca) == true){ // percorre o nó. Se encontrou a chaveBusca, então
        return BO_ou_RRN; // retorna byteoffset dados onde o registro com tal chave se encontra
    }else if(BO_ou_RRN == -1){ // se percorrer_no retornou -1, é porque chegou ao fim da árvore e não achou a chaveBusca
        return -1; // retorna -1 pois não encontrou chaveBusca na árvore-B
    }else{
        byteBTree descendente[TAM_NO_BTREE]; // buffer na memória para o nó descendente
        carregar_no(descendente, arvoreB, BO_ou_RRN); // trazendo o nó descendente do disco para a memória
        return buscar_chave_rec(arvoreB, descendente, chaveBusca); // continuando a busca recursiva no nó descendente
    }

}

/**
 * Navega recursivamente pela árvore-B especificada procurando a chaveBusca
 * Se encontrar, retorna o byteoffset do registro de dados com tal chave
 * Se não encontrar, retorna -1
 */
int buscar_entrada(FILE* arvoreB, int chaveBusca){
    
    byteBTree cabecalho[TAM_CABECALHO_BTREE]; // buffer na memória para cabeçalho
    carregar_cabecalho(cabecalho, arvoreB, false); // carregando cabeçalho sem alterar status, pois não iremos escrever na árvore

    byteBTree raiz[TAM_NO_BTREE]; // buffer na memória para nó raiz
    int RRNraiz = get_inteiro(cabecalho, BO_RRNraiz); // obtendo o RRN do nó raiz
    carregar_no(raiz, arvoreB, RRNraiz); // trazendo o nó raiz do disco para a memória

    return buscar_chave_rec(arvoreB, raiz, chaveBusca); // iniciando a busca recursiva pelo nó raiz e retornando o resultado
}