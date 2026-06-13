// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/datamanager.h"
#include "arvore_avl/tabelafuncoes.h"
#include "arvore_avl/arvore_avl.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

/*
O uso de árvore binária foi a solução encontrada para o problema de calcular a
quantidade de elementos distintos, seja estações, sejam pares (codEstacao, codProxEstacao).
Os elementos são inseridos em suas respectivas árvores. Então é feito um percurso em ordem 
contabilizando a quantidade de vezes que o conteúdo do nó atual muda em relação ao anterior.
Como a inserção de um item em uma árvore balanceada é O(log k), em que k é a quantidade
atual de nós, a inserção de n nós é O(log n!), que é assintoticamente igual a O(n log n).

São definidas duas árvores visíveis para todo o arquivo, mas invisíveis para os outros. A única
forma de outros arquivos interagirem com essas árvores é a partir da função atualizar_cabecalho 

Cada árvore utiliza funções customizadas para ordenar, identificar e apagar o tipo de item 
para os quais cada nó aponta. Os tipos dessas funções são especificados em tabelafuncoes.h.

Esse arquivo define esses dois tipos de itens e as funções aplicáveis a cada um.
*/



static ABB* nroEstacoesTracker = NULL;
static ABB* nroParesEstacaoTracker = NULL;



typedef struct {
    int codEstacao; // Necessário para identificar o nó na hora de remover
    char* nomeEstacao; // Critério para ordenar os nós
} nroEstacoesItem;

// Deve apagar o item e modificar a referência a ele para NULL
void nroEstacoesItem_apagar(void** item){
    if(item == NULL || *item == NULL) return;

    nroEstacoesItem** ptr = (nroEstacoesItem**) item; // Casting para acessar o membro da struct
    free((*ptr)->nomeEstacao); // apagar a string do nome da estação
    free(*ptr);
    *ptr = NULL; // evitar use-after-free
}

// Deve retornar <0 se o item1 deve aparecer antes do item2, 0 se tanto faz, >0 se o item2 deve aparecer antes do item1.
int nroEstacoesItem_ordenar(void* item1, void* item2){
    if(item1 == NULL || item2 == NULL){
        //DEBUG("ERRO EM nroEstacoes: Uma das estações a serem ordenadas é nula.\n");
        return 0;
    }

    nroEstacoesItem* ptr1 = (nroEstacoesItem*) item1;
    nroEstacoesItem* ptr2 = (nroEstacoesItem*) item2;
    return strcmp(ptr1->nomeEstacao, ptr2->nomeEstacao);
}

// Deve retornar true se os dois itens têm a mesma chave
bool nroEstacoesItem_identificar(void* item1, void* item2){
    if(item1 == NULL || item2 == NULL){
        return false;
    }

    nroEstacoesItem* ptr1 = (nroEstacoesItem*) item1;
    nroEstacoesItem* ptr2 = (nroEstacoesItem*) item2;

    return (ptr1->codEstacao == ptr2->codEstacao);
}



typedef struct {
    int codEstacao; // Necessário para identificar o nó na hora de remover
    int codProxEstacao;
    // O par (codEstacao, codProxEstacao) é o critério para ordenar os nós
} nroParesEstacaoItem;

// Deve apagar o item e modificar a referência a ele para NULL
void nroParesEstacaoItem_apagar(void** item) {
    if(item == NULL || *item == NULL) return;

    free(*item);
    *item = NULL; // evitar use-after-free
}

// Deve retornar <0 se o item1 deve aparecer antes do item2, 0 se tanto faz, >0 se o item2 deve aparecer antes do item1.
int nroParesEstacaoItem_ordenar(void* item1, void* item2){
    if(item1 == NULL || item2 == NULL){
        //DEBUG("ERRO EM nroParesEstacaoItem_ordenar: Um dos pares de estação a serem ordenados é nulo.");
        return 0;
    }

    nroParesEstacaoItem* ptr1 = (nroParesEstacaoItem*) item1;
    nroParesEstacaoItem* ptr2 = (nroParesEstacaoItem*) item2;

    int criterio1 = ptr1->codEstacao - ptr2->codEstacao;
    if(criterio1 != 0) return criterio1;

    int criterio2 = ptr1->codProxEstacao - ptr2->codProxEstacao;
    return criterio2;
}

// Deve retornar true se os dois itens têm a mesma chave
bool nroParesEstacaoItem_identificar(void* item1, void* item2){
    if(item1 == NULL || item2 == NULL){
        return false;
    }

    nroParesEstacaoItem* ptr1 = (nroParesEstacaoItem*) item1;
    nroParesEstacaoItem* ptr2 = (nroParesEstacaoItem*) item2;
    
    return (ptr1->codEstacao == ptr2->codEstacao) && (ptr1->codProxEstacao == ptr2->codProxEstacao);
}

// A seguir seguem dois vetores de ponteiros para função. Cada árvore recebe a sua respectiva Tabela
// de Funções após serem criadas. 

static const TABELA_FUNCOES nroEstacoesItemFuncoes = {
    nroEstacoesItem_apagar,
    nroEstacoesItem_ordenar,
    nroEstacoesItem_identificar,
};

static const TABELA_FUNCOES nroParesEstacaoItemFuncoes = {
    nroParesEstacaoItem_apagar,
    nroParesEstacaoItem_ordenar,
    nroParesEstacaoItem_identificar,
};







// INÍCIO DAS FUNÇÕES QUE GERENCIAM DISCO E ESTRUTURA DE DADOS






bool modo_eh_valido(char* modo){

    char* modos_possiveis[] = {"rb", "rb+", "wb", "wb+"};
    // contador inicializado como long unsigned int para o compilador parar de reclamar
    for(long unsigned int i=0; i < sizeof(modos_possiveis)/sizeof(char*); i++){
        if(strcmp(modo, modos_possiveis[i]) == 0)
            return true;
    }
    return false;
}

FILE* abre_binario(char* nome, char* modo){
    
    // Verificando o modo correto:
    if(modo == NULL || modo_eh_valido(modo) == false){
        DEBUG("ERRO EM abre_binario: NÃO É UM MODO VÁLIDO.\n");
        return NULL;
    }

    // Abrindo o arquivo
    FILE* arquivoDados = fopen(nome, modo);
    if(arquivoDados == NULL){
        DEBUG("ERRO EM abre_binario: ERRO AO ABRIR O BINÁRIO. VERIFIQUE EXISTÊNCIA E PERMISSÕES.\n");
        return NULL;
    }

    // Verificando se o status está consistente
    if(strcmp(modo, "wb+") != 0){ // arquivos novos não tem status para serem lidos
        fseek(arquivoDados, 0, SEEK_SET);
        unsigned char status;
        
        if (fread(&status, 1, 1, arquivoDados) == 1) {
            if(status != '1'){ // Se for diferente de consistente, fecha e retorna NULL
                DEBUG("ERRO EM abre_binario: ARQUIVO INCONSISTENTE. NÃO FOI POSSÍVEL ABRIR.\n");
                fclose(arquivoDados);
                return NULL;
            }
        }
    }

    // Se o arquivo for aberto para escrita, marca o status como inconsistente
    bool escrita = (strcmp(modo, "rb") == 0) ? false : true;

    if(escrita){
        unsigned char status = '0';
        fseek(arquivoDados, 0, SEEK_SET);
        fwrite(&status, 1, 1, arquivoDados);
    }

    fseek(arquivoDados, 0, SEEK_SET); // Garante o ponteiro no início 
    return arquivoDados;
}

int fecha_binario(FILE* arquivoDados, char* modo){
    if(arquivoDados == NULL){
        return 0;
    }

    // Se o modo foi aberto apenas para leitura, o status não deve ser alterado
    bool marcarConsistente = (strcmp(modo, "rb") == 0) ? false : true; 

    // Marcando o status do arquivo como consistente
    if(marcarConsistente){
        fseek(arquivoDados, 0, SEEK_SET);
        unsigned char status_consistente = '1';
        fwrite(&status_consistente, 1, 1, arquivoDados);
    }

    // Fechando o arquivo binário
    if(fclose(arquivoDados) != 0){
        DEBUG("ERRO EM fecha_binario: ERRO AO USAR fclose NO ARQUIVO BINÁRIO.\n");
        return -1;
    }

    return 0;
}

bool escreve_registro(REG_DADOS_STRUCT* registroInserir, FILE* arquivoDados){
    if(registroInserir == NULL){
        DEBUG("ERRO EM escreve_registro: REGISTRO NULO.\n");
        return false;
    }
    if(arquivoDados == NULL){
        DEBUG("ERRO EM escreve_registro: FILESTREAM NULA.\n");
        return false;
    }

    long pos_inicial = ftell(arquivoDados);   
    if( (pos_inicial - HEADER_S)%REG_DADOS_S != 0 ){
        DEBUG("ERRO EM escreve_registro: CURSOR DE ARQUIVO NÃO ESTÁ NO INÍCIO DE UM REGISTRO DE DADOS.\n");
        return false;
    }

    // ESCREVENDO O STRUCT NO BINÁRIO

    if (fwrite(&(registroInserir->removido), 1, 1, arquivoDados) != 1){
        DEBUG("DEBUG: ESCRITA DO REGISTRO FALHOU. VERIFIQUE SE O ARQUIVO ESTÁ ABERTO EM MODO DE ESCRITA.\n");
        fseek(arquivoDados, pos_inicial, SEEK_SET);
        return false;
    }
    fwrite(&(registroInserir->proximo), 1, 4, arquivoDados);
    fwrite(&(registroInserir->codEstacao), 1, 4, arquivoDados);
    fwrite(&(registroInserir->codLinha), 1, 4, arquivoDados);
    fwrite(&(registroInserir->codProxEstacao), 1, 4, arquivoDados);
    fwrite(&(registroInserir->distProxEstacao), 1, 4, arquivoDados);
    fwrite(&(registroInserir->codLinhaIntegra), 1, 4, arquivoDados);
    fwrite(&(registroInserir->codEstIntegra), 1, 4, arquivoDados);

    fwrite(&(registroInserir->tamNomeEstacao), 1, 4, arquivoDados); // armazena o tamanho do nome da estação
    if(registroInserir->tamNomeEstacao != 0){
        fwrite(registroInserir->nomeEstacao, 1, registroInserir->tamNomeEstacao, arquivoDados); // como tamNomeEstacao foi inicializado por strlen, o \0 no final não será escrito
    }

    fwrite(&(registroInserir->tamNomeLinha), 1, 4, arquivoDados); // armazena o tamanho do nome da linha
    if(registroInserir->tamNomeLinha != 0){
        fwrite(registroInserir->nomeLinha, 1, registroInserir->tamNomeLinha, arquivoDados); // como tamNomeLinha foi inicializado por strlen, o \0 no final não será escrito
    }

    int num_bytes_lixo = REG_DADOS_S - BYTES_FIXOS_S - registroInserir->tamNomeEstacao - registroInserir->tamNomeLinha; // Calcula o número de bytes a serem preenchidos com lixo
    char lixo = LIXO;
    for(int i = 0; i < num_bytes_lixo; i++){
        fwrite(&lixo, 1, 1, arquivoDados);
    }

    return true;
}

bool atualiza_registro(REG_DADOS_STRUCT *campos_novos, int mask, int RRN, FILE *arquivoDados){
    if(campos_novos == NULL){
        DEBUG("ERRO EM atualiza_registro: NÃO FOI FORNECIDO O VALOR DOS NOVOS CAMPOS.\n");
        return false;
    }

    int proxRRN;
    fseek(arquivoDados, 5, SEEK_SET);
    if (fread(&proxRRN, 4, 1, arquivoDados) != 1){
        DEBUG("ERRO EM atualiza_registro: LEITURA FALHOU. VERIFIQUE SE O ARQUIVO ESTÁ ABERTO EM MODO DE LEITURA.\n");
        return false;
    }
    if(RRN < 0 || RRN >= proxRRN){
        if(RRN < 0){ DEBUG("ERRO EM atualiza_registro: RRN < 0.\n");
        }else{DEBUG("ERRO EM atualiza_registro: RRN MAIOR QUE O TAMANHO DO ARQUIVO.\n");}
        return false;
    }

    long int pos_reg = RRN * REG_DADOS_S + HEADER_S; // byte offset do registro atual
    
    // ATUALIZANDO CAMPOS INTEIROS

    /*
    - 1: codEstacao
    - 2: codLinha
    - 4: codProxEstacao
    - 8: distProxEstacao
    - 16: codLinhaIntegra
    - 32: codEstIntegra
    */

    if(mask & 1){
        fseek(arquivoDados, pos_reg + 5, SEEK_SET);
        fwrite(&(campos_novos->codEstacao), 4, 1, arquivoDados);
    }
    if(mask & 2){
        fseek(arquivoDados, pos_reg + 9, SEEK_SET);
        fwrite(&(campos_novos->codLinha), 4, 1, arquivoDados);
    }
    if(mask & 4){
        fseek(arquivoDados, pos_reg + 13, SEEK_SET);
        fwrite(&(campos_novos->codProxEstacao), 4, 1, arquivoDados);
    }
    if(mask & 8){
        fseek(arquivoDados, pos_reg + 17, SEEK_SET);
        fwrite(&(campos_novos->distProxEstacao), 4, 1, arquivoDados);
    }
    if(mask & 16){
        fseek(arquivoDados, pos_reg + 21, SEEK_SET);
        fwrite(&(campos_novos->codLinhaIntegra), 4, 1, arquivoDados);
    }
    if(mask & 32){
        fseek(arquivoDados, pos_reg + 25, SEEK_SET);
        fwrite(&(campos_novos->codEstIntegra), 4, 1, arquivoDados);
    }

    // ATUALIZANDO CAMPOS STRING

    /*
    - 64: nomeEst
    - 128: nomeLinha
    */

    int tamNomeEst, tamNomeLinha;
    char nomeEst[64], nomeLinha[64];

    // Transferir do disco para a memória os valores atuais:
    fseek(arquivoDados, pos_reg + 29, SEEK_SET);
    fread(&tamNomeEst, 4, 1, arquivoDados);
    fread(nomeEst, 1, tamNomeEst, arquivoDados);
    fread(&tamNomeLinha, 4, 1, arquivoDados);
    fread(nomeLinha, 1, tamNomeLinha, arquivoDados);

    DEBUG("DEBUG: LENDO NOME E LINHA ANTIGOS: %s\n%s\n", nomeEst, nomeLinha);

    // Atualizando os valores na memória
    if(mask & 64){
        if (campos_novos->nomeEstacao != NULL) {
            strcpy(nomeEst, campos_novos->nomeEstacao);
            tamNomeEst = strlen(nomeEst);
        }else{
            tamNomeEst = 0; // Se não tem nome, tamanho é 0
        }
    }

    if(mask & 128){
        if (campos_novos->nomeLinha != NULL) {
            strcpy(nomeLinha, campos_novos->nomeLinha);
            tamNomeLinha = strlen(nomeLinha);
        }else{
            tamNomeLinha = 0;
        }
    }

    // Transferindo os valores da memória para o disco
    fseek(arquivoDados, pos_reg + 29, SEEK_SET);
    fwrite(&tamNomeEst, 4, 1, arquivoDados);
    fwrite(nomeEst, 1, tamNomeEst, arquivoDados);
    fwrite(&tamNomeLinha, 4, 1, arquivoDados);
    fwrite(nomeLinha, 1, tamNomeLinha, arquivoDados);

    DEBUG("DEBUG: LENDO NOVO NOME E LINHA: %s\n%s\n", nomeEst, nomeLinha);

    // Preenchendo com lixo até o final
    int quantidade = REG_DADOS_S - 1 - 4*(CAMPOS_INT + CAMPOS_STRINGS) - tamNomeEst - tamNomeLinha;
    char *buffer_lixo = malloc(quantidade);
    memset(buffer_lixo, LIXO, quantidade); // Enche o buffer com a quantidade necessária de '$'
    DEBUG("DEBUG: FORAM ESCRITOS %d $.\n", quantidade);
    fwrite(buffer_lixo, 1, quantidade, arquivoDados);
    
    free(buffer_lixo);
    return true;
}

bool check_registro(REG_DADOS_STRUCT* chave, int mask, int RRN, FILE* bin){
    fseek(bin, RRN * REG_DADOS_S + HEADER_S, SEEK_SET);
    
    // Verificando se o registro está removido e decidindo se deve continuar:
    unsigned char removido;
    fread(&removido, 1, 1, bin);
    if(removido == '1'){
        DEBUG("DEBUG: em check registro, registro de RRN %d removido\n", RRN)
        return false;
    };

    // Pular o campo 'proximo'
    fseek(bin, 4, SEEK_CUR);

    // Ler todos os campos em ordem
    int cEst, cLin, cProx, dist, cLinInt, cEstInt, tNomeE, tNomeL;
    
    fread(&cEst, 4, 1, bin);
    fread(&cLin, 4, 1, bin);
    fread(&cProx, 4, 1, bin);
    fread(&dist, 4, 1, bin);
    fread(&cLinInt, 4, 1, bin);
    fread(&cEstInt, 4, 1, bin);

    // Verificação de inteiros usando bitwise AND
    if((mask & 1) && chave->codEstacao != cEst) return false;
    if((mask & 2) && chave->codLinha != cLin) return false;
    if((mask & 4) && chave->codProxEstacao != cProx) return false;
    if((mask & 8) && chave->distProxEstacao != dist) return false;
    if((mask & 16) && chave->codLinhaIntegra != cLinInt) return false;
    if((mask & 32) && chave->codEstIntegra != cEstInt) return false;

    // Verificação de Strings
    // Nome Estação
    fread(&tNomeE, 4, 1, bin);
    if(mask & 64){
        if(tNomeE > 0){
            if(tNomeE >= 100) return false; // caso o arquivo esteja corrompido, devemos impedir um tamanho grande de ser lido e causar buffer overflow

            char temp[100];
            fread(temp, 1, tNomeE, bin);
            temp[tNomeE] = '\0';
            // Se a busca não é nula e o arquivo tem dado, compara
            if(chave->nomeEstacao != NULL){
                if(strcmp(chave->nomeEstacao, temp) != 0) return false;
            }else{
                // Buscando por NULO mas encontrou dado no arquivo
                return false;
            }
        }else{
            // Campo no arquivo é NULO
            if(chave->nomeEstacao != NULL) return false;
        }
    }else{
        fseek(bin, tNomeE, SEEK_CUR);
    }

    // Nome Linha
    fread(&tNomeL, 4, 1, bin);
    if(mask & 128){
        if(tNomeL > 0){
            char temp[100];
            fread(temp, 1, tNomeL, bin);
            temp[tNomeL] = '\0';
            // Se a busca não é nula e o arquivo tem dado, compara
            if(chave->nomeLinha != NULL){
                if(strcmp(chave->nomeLinha, temp) != 0) return false;
            }else{
                // Buscando por NULO mas encontrou dado no arquivo
                return false;
            }
        }else{
            // Campo no arquivo é NULO
            if(chave->nomeLinha != NULL) return false;
        }
    }else{
        fseek(bin, tNomeL, SEEK_CUR);
    }
    return true;
}

bool load_registro(FILE* arquivoDados, REG_DADOS_STRUCT* mem_destino){

    long pos_inicial = ftell(arquivoDados); // Armazena a posição inicial de leitura
    if((pos_inicial - HEADER_S)%REG_DADOS_S != 0){
        DEBUG("ERRO EM Load_registro: CURSOR NÃO ESTÁ POSICIONADO NO COMEÇO DE UM REGISTRO");
        return false;
    }
    DEBUG("DEBUG: load_registro sendo executada, operando com RRN: %ld\n", (pos_inicial - HEADER_S)/REG_DADOS_S);

    unsigned char removido;
    int campos_inteiros[CAMPOS_INT]; // 0-Proximo, 1-codEstacao, 2-codLinha, 3-codProxEstacao, 4-distProxEstacao, 5-codLinhaIntegra, 6-codEstIntegra

    // LENDO 'REMOVIDO' E CAMPOS INTEIROS
    if(fread(&removido, 1, 1, arquivoDados) != 1){
        DEBUG("ERRO EM load_registro: ERRO AO LER CAMPO removido.\n");
        return false;
    }
    if(fread(campos_inteiros, 4, CAMPOS_INT, arquivoDados) != CAMPOS_INT){
        DEBUG("ERRO EM load_registro: ERRO AO LER CAMPOS INTEIROS.\n");
        return false;
    }

    //DEBUG("CAMPOS INTEIROS FORAM LIDOS. codEstacao = %d\n", campos_inteiros[1]);

    // LENDO CAMPOS STRING
    char* campos_strings[CAMPOS_STRINGS] = {NULL}; // 0-nomeEstacao, 1-nomeLinha
    // Sempre inicialize ponteiros como NULL
    int indicadores_tamanhos[CAMPOS_STRINGS]; // 0-nomeEstacao, 1-nomeLinha
    
    for(int i=0; i<CAMPOS_STRINGS; i++){
        int tam;
        // Lê o indicador de tamanho
        fread(&tam, 4, 1, arquivoDados);

        indicadores_tamanhos[i] = tam;
        if(tam > 0){ // Se o tamanho do campo for maior que 0, o campo string não é NULO
            campos_strings[i] = (char*)malloc(tam + 1);
            if(campos_strings[i]){
                fread(campos_strings[i], 1, tam, arquivoDados);
                campos_strings[i][tam] = '\0'; // adiciona o '/0' no final da string
            }else{
                DEBUG("DEBUG: ERRO AO ALOCAR MEMÓRIA PARA NOME DO REGISTRO.\n");
                if (i == 1) free(campos_strings[0]);
                return false;
            }
        }else{ // Se o tamanho da string for 0, deve-se printar "NULO" ao invês de pular o campo
            campos_strings[i] = NULL;
        }
    }

    // TRANSFERINDO OS DADOS PARA O STRUCT

    mem_destino->removido = removido;
    mem_destino->proximo = campos_inteiros[0];
    mem_destino->codEstacao = campos_inteiros[1];
    mem_destino->codLinha = campos_inteiros[2];
    mem_destino->codProxEstacao = campos_inteiros[3];
    mem_destino->distProxEstacao = campos_inteiros[4];
    mem_destino->codLinhaIntegra = campos_inteiros[5];
    mem_destino->codEstIntegra = campos_inteiros[6];
    mem_destino->tamNomeEstacao = indicadores_tamanhos[0];
    mem_destino->nomeEstacao = campos_strings[0];
    mem_destino->tamNomeLinha = indicadores_tamanhos[1];
    mem_destino->nomeLinha = campos_strings[1];

    //
    DEBUG("load_registro: TODOS OS CAMPOS FORAM LIDOS. nomeEstacao = %s\n", mem_destino->nomeEstacao);

    // Move o cursor para o início do próximo registro
    fseek(arquivoDados, pos_inicial + REG_DADOS_S, SEEK_SET);

    return true;
}

static void carregar_dados(FILE* arquivoDados){

    // APAGANDO POSSÍVEIS ESTRUTURAS DE DADOS
    abb_apagar(&nroEstacoesTracker);
    abb_apagar(&nroParesEstacaoTracker);

    // CRIANDO NOVAS ESTRUTURAS DE DADOS CONFIGURADAS PARA CONTER OS ITENS QUE QUEREMOS CONTAR
    nroEstacoesTracker = abb_criar(&nroEstacoesItemFuncoes);
    nroParesEstacaoTracker = abb_criar(&nroParesEstacaoItemFuncoes);

    // LENDO OS REGISTROS DE DADOS DO DISCO PARA A MEMÓRIA, E INSERINDO NA ESTRUTURA DE DADOS

    fseek(arquivoDados, HEADER_S, SEEK_SET); // Move o cursor para o primeiro registro
    REG_DADOS_STRUCT registroLido;

    while(load_registro(arquivoDados, &registroLido)) {
        if(registroLido.removido == '0'){

            // Inserindo o codEstacao e o nomeEstacao na árvore binária que contabiliza número de estações:

            if(registroLido.codEstacao != -1 && registroLido.nomeEstacao != NULL){
                
                // Criando o item

                nroEstacoesItem* item1 = (nroEstacoesItem*) malloc (sizeof(nroEstacoesItem));
                item1->codEstacao = registroLido.codEstacao;
                item1->nomeEstacao = strdup(registroLido.nomeEstacao);

                abb_inserir(nroEstacoesTracker, item1); // Atualiza o tracker de nroEstacoes apenas se o novo registro contiver um código e nome válidos
            }

            // Inserindo o codEstacao e o codProxEstacao na árvore binária que contabiliza o número de pares diferentes:

            if(registroLido.codEstacao != -1 && registroLido.codProxEstacao != -1){

                // Criando o item

                nroParesEstacaoItem* item2 = (nroParesEstacaoItem*) malloc (sizeof(nroParesEstacaoItem));
                item2->codEstacao = registroLido.codEstacao;
                item2->codProxEstacao = registroLido.codProxEstacao;

                abb_inserir(nroParesEstacaoTracker, item2); // Atualiza o tracker de nroParesEstacao apenas se o novo registro contiver um par válido
            }
        }

        free(registroLido.nomeEstacao);
        free(registroLido.nomeLinha);
    }
}

void atualizar_cabecalho(FILE* arquivoDados, int topo, int proxRRN){

    carregar_dados(arquivoDados); // CRIANDO E POPULANDO AS ESTRUTURAS DE DADOS A PARTIR DA INFORMAÇÃO NO DISCO
    
    // ATUALIZANDO TOPO DA PILHA E PROX RRN
    DEBUG("atualizar_cabecalho: topo: %d, proxRRN: %d\n", topo, proxRRN);

    // Atualizando status

    unsigned char status_consistente = '1';
    fseek(arquivoDados, 0, SEEK_SET);
    fwrite(&status_consistente, 1, 1, arquivoDados);
    
    // Atualizando valores

    fwrite(&topo, 4, 1, arquivoDados);    // Novo topo da pilha
    fwrite(&proxRRN, 4, 1, arquivoDados); // Novo próximo RRN

    // CALCULANDO E ATUALIZANDO CONTADORES
    int nroEstacoes = abb_contar_distintos(nroEstacoesTracker);
    int nroParesEstacao = abb_contar_distintos(nroParesEstacaoTracker);

    DEBUG("atualizar_cabecalho: nroEstacoes = %d\n", nroEstacoes);
    DEBUG("atualizar_cabecalho: nroParesEstacao = %d\n", nroParesEstacao);

    fwrite(&nroEstacoes, 4, 1, arquivoDados);
    fwrite(&nroParesEstacao, 4, 1, arquivoDados);

    abb_apagar(&nroEstacoesTracker);
    abb_apagar(&nroParesEstacaoTracker);

    return;
}