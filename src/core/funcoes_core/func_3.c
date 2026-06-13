// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_3(FILE* arquivoBin, int n){

    REG_DADOS_STRUCT* registros_de_busca = NULL;
    int* mask = NULL;

    // COLETANDO CRITÉRIOS DE BUSCA

    // Cria um vetor com os registros chaves de busca, e tambem um vetor com um bit mask dos campos utilizados para busca
    registros_de_busca = (REG_DADOS_STRUCT* )malloc(n*sizeof(REG_DADOS_STRUCT));
    mask = (int* )calloc(n, sizeof(int));
    if(registros_de_busca == NULL || mask == NULL){DEBUG("ERRO EM func_3: ALOCAÇÃO DE MEMÓRIA.\n"); goto erro;}

    for(int i = 0; i < n; i++){ // lê as n entradas de argumentos para buscas
        ler_campos(&registros_de_busca[i], &mask[i]);
    }

    // BUSCANDO REGISTROS DE DADOS

    int topoPilha, proxRRN;
    fseek(arquivoBin, 1, SEEK_SET);
    fread(&topoPilha, 4, 1, arquivoBin);
    fread(&proxRRN, 4, 1, arquivoBin);
    // Começar busca sequencial no arquivo a partir do RRN = 0 para cada uma das n buscas
    for(int i = 0; i < n; i++){

        bool flag_encontrou = false;

        for(int RRN = 0; RRN < proxRRN; RRN++){
            if(check_registro(&registros_de_busca[i], mask[i], RRN, arquivoBin)){
                // Se o registro bate com a busca, posiciona e imprime
                fseek(arquivoBin, RRN * REG_DADOS_S + HEADER_S, SEEK_SET);
                print_registro(arquivoBin); 
                flag_encontrou = true;
            }
        }

        if(!flag_encontrou) printf("Registro inexistente.\n");
        printf("\n");

        if (mask[i] & 64){ free(registros_de_busca[i].nomeEstacao); registros_de_busca[i].nomeEstacao = NULL;}
        if (mask[i] & 128){ free(registros_de_busca[i].nomeLinha); registros_de_busca[i].nomeEstacao = NULL;}
    }

    // LIMPANDO E RETORNANDO

    free(registros_de_busca);
    free(mask);

    return true;

    erro:

    free(registros_de_busca);
    free(mask);

    return false;
}