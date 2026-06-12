#include "../../include/arvoreb/arvoreb_interna.h"

// Insere uma nova entrada em nó não cheio, deslocando as entradas conforme necessário para mantê-las ordenadas
static void inserir_entrada_em_no_shiftada(byteBTree* no, ENTRADA_INDICE novaEntrada){
    
    // Verificando se o nó está cheio. Se estiver, não pode inserir.
    
    int nroChaves = get_inteiro(no, BO_nroChaves); // obtendo número de chaves no nó
    if(nroChaves == ORDEM_BTREE - 1){ // comparando com o valor máximo possível
        printf("O nó está lotado! Não há como inserir.\n");
        exit(1);
    }

    // Obtendo o vetor de entradas do nó, vamos inserir a nova entrada nesse vetor mantendo a ordenação

    ENTRADA_INDICE entradas[nroChaves+1]; // alocando vetor com uma entrada a mais do que o nó tem
    get_entradas(entradas, no, nroChaves); // copia as entradas do nó para o vetor, deixando a última posição do vetor livre
    bool inseriu = false;
    int i = nroChaves-1; // penúltima posição do vetor de entradas
    
    for(; i>=0; i--){ // iterando da penúltima posição até a primeira
        if(entradas[i].chave > novaEntrada.chave){ // se a chave da entrada atual for maior do que a da nova entrada
            entradas[i+1] = entradas[i]; // copia a entrada atual para a posição seguinte e continua iterando
        }else{
            entradas[i+1] = novaEntrada; // insere a nova entrada no espaço seguinte e para de iterar
            inseriu = true;
            break;
        }
    }

    if(!inseriu){ // se shiftou todas as entradas sem inserir a nova entrada
        entradas[0] = novaEntrada; // insere a nova entrada no começo
    }

    set_entradas(no, entradas, nroChaves+1); // reescreve o nó em memória principal considerando o novo vetor de entradas
}

/**
 * Insere uma nova entrada em um nó qualquer. Se não estiver cheio, faz a shiftada.
 * Se estiver cheio, cria um novo nó, distribui as entradas e promove uma delas.
 * Retorna a entrada promovida
 */
ENTRADA_INDICE inserir_entrada_em_no(FILE* arvoreB, byteBTree* cabecalho, byteBTree* no, ENTRADA_INDICE entradaInserir, int* tipoNoCriar){

    if(check_entrada_nula(entradaInserir)){ // verifica se a entrada recebida é nula
        return entradaInserir; // caso seja nula, retorna sem fazer nada, pois não ocorreu promoção na chamada anterior
    }

    int nroChaves = get_inteiro(no, BO_nroChaves); // obtendo nroChaves do nó
    
    if(nroChaves < ORDEM_BTREE - 1){ // Se existe espaço no nó, então
        
        inserir_entrada_em_no_shiftada(no, entradaInserir); // insere a entrada ordenadamente no nó em memória principal
        ENTRADA_INDICE retorno = get_entrada_nula(); // retorna entrada nula

        return retorno; // retorna uma entrada inválida, para indicar que não há promoção
    }
    else // Se o nó estiver cheio, então
    { 
        // Lembrando que m == ORDEM_BTREE
        // Ordenando as entradas do nó e a nova entrada:
        
        ENTRADA_INDICE vetorEntradas[m]; // alocando vetor de entradas com uma entrada a mais do que o nó tem
        get_entradas(vetorEntradas, no, m-1); // copiando as entradas do nó para o vetor
        vetorEntradas[m-1] = entradaInserir; // inserindo a nova entrada no vetor
        qsort(vetorEntradas, m, sizeof(ENTRADA_INDICE), comparar_entradas); // ordenando o vetor

        // Escolhendo a entrada promovida
        
        ENTRADA_INDICE entradaPromovida = vetorEntradas[m/2];
        
        // Criando novo nó (se é o primeiro split, então é do tipo folha, senão tipo intermediário):
        
        byteBTree novoNo[TAM_NO_BTREE]; // alocando espaço na memória para o nó
        int RRNnovoNo = criar_no(novoNo, arvoreB, cabecalho, *tipoNoCriar); // preenchendo com valores iniciais
        *tipoNoCriar = TIPOINTERMEDIARIO; // os próximos nós criados devem ser do tipo intermediário
        
        // Gerenciando referências para manter a estrutura da árvore:
        
        set_inteiro(novoNo, BO_P1, entradaPromovida.RRNdescendente); // P1 do novo nó deve ser igual ao RRNdescendente da entrada promovida
        entradaPromovida.RRNdescendente = RRNnovoNo; // O RRNdescendente da entrada promovida deve ser o RRN do novo nó
        
        // Reescrevendo os nós com as entradas corretas e armazenando-os em disco:

        set_entradas(no, vetorEntradas, m/2); // nó antigo
        set_entradas(novoNo, vetorEntradas+m/2+1, m-m/2-1); // nó novo
        armazenar_no(arvoreB, novoNo, RRNnovoNo); // nó novo
        
        // Retornando a entradaPromovida, que mantém a referência para o nó criado:

        return entradaPromovida;
    }
    
}

void substituir_entrada_em_no(byteBTree* no, ENTRADA_INDICE entradaInserir){

        int chaveAtual = get_inteiro(no, BO_C1); // C1
        int chaveBusca = entradaInserir.chave;
        bool achou = false;

        // O loop a seguir percorre o nó chave por chave procurando pela entrada com a mesma chave que entradaInserir. 
        // Se encontrar, substitui o BOdados da entrada no nó pelo BOdados de entradaInserir. 

        int nroChaves = get_inteiro(no, BO_nroChaves); // o loop será executado no máximo tantas vezes quanto for o número de chaves no nó
        int i;
        for(i=1; i<nroChaves && chaveAtual != chaveBusca; i++){ // enquanto C_i != chaveBusca
            chaveAtual = get_inteiro(no, BO_C1+8*i); // C2, C3
        }

        if(chaveAtual == chaveBusca){ // se encontrou a chaveBusca
            set_inteiro(no, BO_PR1+8*i-8, entradaInserir.BOdados); // PR1, PR2, PR3
            achou = true;
        }
        
        if(!achou){
            DEBUG("ERRO EM substituir_entrada_em_no: TENTOU SUBSTITUIR A ENTRADA (%d, %d) MAS ELA NÃO EXISTE NO NÓ.\n", entradaInserir.chave, entradaInserir.BOdados); 
            exit(1);
        }

        return;
}

/**
 * Percorre o nó fornecido procurando pelo lugar certo para inserir a nova entrada 
 * Se for uma folha, insere no nó atual
 * Se não for, chamada recursiva no nó descendente, obtém a entrada promovida pelo descendente 
 * e a insere no nó atual.
 * Retorna a entrada promovida pelo nó atual
 * 
 * A filestream deve permitir leitura e escrita.
 * O cabeçalho da árvore é necessário para criar novos nós.
 * O tipoNoCriado aponta para um inteiro, cujo valor inicial deve ser TIPOFOLHA. Após a primeira inserção, a função atualiza esse valor para TIPOINTERMEDIARIO
 */
static ENTRADA_INDICE inserir_entrada_na_arvore_rec(FILE* arvoreB, byteBTree* cabecalho, byteBTree* noAtual, int RRNatual, ENTRADA_INDICE entradaInserir, int* tipoNoCriado) {
    if(RRNatual < 0){
        DEBUG("ERRO EM inserir_entrada_na_arvore_rec: RRNatual negativo.\n");
        exit(1);
    }

    DEBUG("VAMOS INSERIR (%d, %d) NO NÓ:", entradaInserir.chave, entradaInserir.BOdados);
    #ifdef PRINT_ERROS
    imprimir_no(noAtual, RRNatual);
    #endif

    // Percorrendo o nó atual e obtendo o RRN do nó descendente para continuar a busca:

    int RRNdescendente; 
    bool achou = percorrer_no(&RRNdescendente, noAtual, entradaInserir.chave);

    if(achou){
        DEBUG("Entrada já está inserida no arquivo de índice.\n");
        substituir_entrada_em_no(noAtual, entradaInserir);
        return get_entrada_nula();
    }

    // Se o RRNdescendente é -1, a inserção deve ser feita no nó atual. 
    // Caso contrário, continua a busca no nó descendente.

    if(RRNdescendente == -1){

        return inserir_entrada_em_no(arvoreB, cabecalho, noAtual, entradaInserir, tipoNoCriado); // retorna entrada promovida
    
    }else{

        byteBTree descendente[TAM_NO_BTREE]; // alocando memória para armazenar o nó descendente
        carregar_no(descendente, arvoreB, RRNdescendente); // carregando o nó descendente
        entradaInserir = inserir_entrada_na_arvore_rec(arvoreB, cabecalho, descendente, RRNdescendente, entradaInserir, tipoNoCriado); // inserindo no nó descendente e obtendo a entrada promovida pelo descendente
        armazenar_no(arvoreB, descendente, RRNdescendente);

        return inserir_entrada_em_no(arvoreB, cabecalho, noAtual, entradaInserir, tipoNoCriado); // insere no nó atual a entrada promovida pelo descendente e retorna a entrada promovida agora
    }
}

int criar_outra_raiz(byteBTree* novaRaiz, byteBTree* raizAntiga, int RRNraizAntiga, FILE* arvoreB, byteBTree* cabecalho){

    // o tipo do nó raiz será raiz
    int RRNnovaRaiz = criar_no(novaRaiz, arvoreB, cabecalho, TIPORAIZ); // preenchendo a nova raiz com valores iniciais
        
    // Atualizando o tipo do nó raiz antigo:

    if(get_inteiro(raizAntiga, BO_tipoNo) == TIPORAIZ){ // se o nó raiz antigo é do tipo raiz
        set_inteiro(raizAntiga, BO_tipoNo, TIPOINTERMEDIARIO); // então muda para intermediário
    } // caso o nó raiz antigo é do tipo folha, ele era o único nó da árvore e continua sendo folha após a criação da nova raiz
        
    // Conectando a raiz nova com a raiz antiga:

    set_inteiro(novaRaiz, BO_P1, RRNraizAntiga); // A raiz antiga é descendente da nova raiz
    
    // Atualizando o RRN da raiz no cabeçalho:
        
    set_inteiro(cabecalho, BO_RRNraiz, RRNnovaRaiz);
    
    return RRNnovaRaiz;
}

int criar_nova_raiz(byteBTree* novaRaiz, FILE* arvoreB, byteBTree* cabecalho){

    // o tipo do nó raiz será folha
    int RRNnovaRaiz = criar_no(novaRaiz, arvoreB, cabecalho, TIPOFOLHA); // preenchendo a nova raiz com valores iniciais

    // Atualizando o RRN da raiz no cabeçalho:
        
    set_inteiro(cabecalho, BO_RRNraiz, RRNnovaRaiz);

    return RRNnovaRaiz;    
}

/**
 * Insere uma entrada na árvore, com chave e byteoffset do arquivo de dados fornecidos.
 * 
 * A filestream deve permitir leitura e escrita.
 */
void inserir_entrada(FILE* arvoreB, int chave, int BOdados){
    
    ENTRADA_INDICE inserirNaArvore = {chave, BOdados, -1};
    byteBTree cabecalho[TAM_CABECALHO_BTREE];
    carregar_cabecalho(cabecalho, arvoreB, true);

    if(get_inteiro(cabecalho, BO_RRNraiz) == -1){ // Se não há raiz
        
        // Criando a primeira raiz da árvore
        byteBTree novaRaiz[TAM_NO_BTREE];
        int RRNnovaRaiz = criar_nova_raiz(novaRaiz, arvoreB, cabecalho);
        inserir_entrada_em_no_shiftada(novaRaiz, inserirNaArvore);
        armazenar_no(arvoreB, novaRaiz, RRNnovaRaiz);
        armazenar_cabecalho(arvoreB, cabecalho);
        return;
    }

    byteBTree raiz[TAM_NO_BTREE];
    int RRNraiz = get_inteiro(cabecalho, BO_RRNraiz);
    carregar_no(raiz, arvoreB, RRNraiz);
    int tipoNoCriar = TIPOFOLHA;

    ENTRADA_INDICE entradaRaiz = inserir_entrada_na_arvore_rec(arvoreB, cabecalho, raiz, RRNraiz, inserirNaArvore, &tipoNoCriar);
    
    if(!check_entrada_nula(entradaRaiz)){ // Se alguma entrada foi promovida, precisamos criar uma nova raiz
        byteBTree novaRaiz[TAM_NO_BTREE];
        int RRNnovaRaiz = criar_outra_raiz(novaRaiz, raiz, RRNraiz, arvoreB, cabecalho);
        inserir_entrada_em_no_shiftada(novaRaiz, entradaRaiz);
        armazenar_no(arvoreB, novaRaiz, RRNnovaRaiz);
    }

    armazenar_no(arvoreB, raiz, RRNraiz);
    armazenar_cabecalho(arvoreB, cabecalho);
}
