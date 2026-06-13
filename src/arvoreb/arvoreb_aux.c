// Cleyton Jose Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "arvoreb/arvoreb_interna.h"
#include <string.h>




// MEXENDO COM ENTRADAS (ENTRIES) EM NÓS DA ÁRVORE B

/*
Um nó da árvore B pode ser decomposto da seguinte forma:

+-----------+----+-----------+-----------+-----------+
| Cabeçalho | P1 | Entrada 1 | Entrada 2 | Entrada 3 |
+-----------+----+-----------+-----------+-----------+

Uma entrada é formada por:
- chave
- BOdados
- RRNdescendente

De acordo com a especificação do trabalho, as entradas correspondem aos valores:
- Entrada 1: C1, PR1, P2
- Entrada 2: C2, PR2, P3
- Entrada 3: C3, PR3, P4

Durante a inserção, essas entradas são inseridas, ordenadas, reorganizadas e promovidas.
Portanto, a todo nó se associa um vetor de m-1 entradas, em que m é a ordem da árvore.
Para realizar essas operações, usamos funções auxiliares:
*/





// Retorna uma entrada nula, definida como tendo chave, BOdados e RRNdescendente iguais a -1.
ENTRADA_INDICE get_entrada_nula(){
    ENTRADA_INDICE entradaNula;
    entradaNula.chave = -1;
    entradaNula.BOdados = -1;
    entradaNula.RRNdescendente = -1;

    return entradaNula;
}

// Verifica se uma entrada qualquer é igual à entrada nula definida acima. Retorna true caso seja, false caso não seja.
bool check_entrada_nula(ENTRADA_INDICE entrada){
    ENTRADA_INDICE nula = get_entrada_nula(); // obtendo os valores da entrada nula
    if(entrada.chave == nula.chave && entrada.BOdados == nula.BOdados && entrada.RRNdescendente == nula.RRNdescendente){ // se todos os campos forem iguais
        return true; // retorna true
    }
    return false; // caso algum campo seja diferente, retorna false
}

/**
 * Lê n entradas de um nó em memória principal e coloca em um vetor de entradas fornecido.
 * Se o tamanho do vetor for maior que n, o resto do vetor é mantido do jeito que estava antes.
 */
void get_entradas(ENTRADA_INDICE vetorEntradas[], const byteBTree* no, int n){
    for(int i=0; i<n; i++){ // iterando pelas entradas do nó
            int C_i = get_inteiro(no, BO_C1+8*i); // obtendo C1, C2, C3
            int PR_i = get_inteiro(no, BO_PR1+8*i); // obtendo PR1, PR2, PR3
            int P_i = get_inteiro(no, BO_P1+4*i+4); // obtendo P2, P3, P4
            ENTRADA_INDICE temp = {C_i, PR_i, P_i}; // colocando em uma struct
            vetorEntradas[i] = temp; // inserindo a struct no vetor
    }
}

// Lê n entradas de um vetor e coloca em um nó em memória principal, preenchendo o resto do nó com entradas nulas.
void set_entradas(byteBTree* no, const ENTRADA_INDICE vetorEntradas[], int n){
    
    ENTRADA_INDICE nulo = get_entrada_nula(); // obtendo os valores da entrada nula
    
    for(int i=0; i<n; i++){ // percorrendo o vetor de entradas e inserindo cada uma no nó:
        set_inteiro(no, BO_C1+8*i, vetorEntradas[i].chave); // inserindo C1, C2, C3
        set_inteiro(no, BO_C1+8*i+4, vetorEntradas[i].BOdados); // inserindo PR1, PR2, PR3
        set_inteiro(no, BO_P1+4*i+4, vetorEntradas[i].RRNdescendente); // inserindo P2, P3, P4
    }
    for(int i=n; i<ORDEM_BTREE - 1; i++){ // preenchendo o resto do nó com entradas nulas
        set_inteiro(no, BO_C1+8*i, nulo.chave);
        set_inteiro(no, BO_C1+8*i+4, nulo.BOdados);
        set_inteiro(no, BO_P1+4*i+4, nulo.RRNdescendente);
    }
    set_inteiro(no, BO_nroChaves, n); // atualizando o número de chaves no nó com o número de entradas não nulas agora
}

// Função de comparação para o qsort. Retorna >0 se a chave da primeira entrada é maior que da segunda, <0 se for menor, 0 se for igual.
int comparar_entradas(const void* entrada1, const void* entrada2){
    ENTRADA_INDICE e1 = *(ENTRADA_INDICE*)entrada1;
    ENTRADA_INDICE e2 = *(ENTRADA_INDICE*)entrada2;
    return e1.chave - e2.chave;
}