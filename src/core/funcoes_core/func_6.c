// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_6(FILE* arquivoBin, int n){

    REG_DADOS_STRUCT* registros_de_busca = NULL;
    REG_DADOS_STRUCT* campos_novos = NULL;
    int* mask_busca = NULL;
    int* mask_novos = NULL;

    // LENDO CAMPOS DE BUSCA E OS VALORES DE ATUALIZAÇÃO:

    registros_de_busca = (REG_DADOS_STRUCT* )malloc(n*sizeof(REG_DADOS_STRUCT));   // Cria um vetor com os registros chaves para busca dos registros a serem atualizados, 
    campos_novos = (REG_DADOS_STRUCT* )malloc(n*sizeof(REG_DADOS_STRUCT));   // Cria um vetor com os valores dos campos a serem atualizados 
    mask_busca = (int* )calloc(n, sizeof(int));  // e tambem dois vetores com um bit mask dos campos para busca e para atualização
    mask_novos = (int* )calloc(n, sizeof(int));
    if(registros_de_busca == NULL || campos_novos == NULL || mask_busca == NULL || mask_novos == NULL){
        DEBUG("ERRO EM func_6: ERRO DE ALOCAÇÃO.\n"); goto erro;
    }

    for(int i = 0; i < n; i++){  // lê as n entradas de argumentos para buscas e campos atualizados
        ler_campos(&(registros_de_busca[i]), &(mask_busca[i]));
        ler_campos(&(campos_novos[i]), &(mask_novos[i]));
    }

    // BUSCANDO OS REGISTROS NO ARQUIVO PARA ATUALIZAR

    int proxRRN, topoPilha;

    fseek(arquivoBin, 1, SEEK_SET);
    fread(&topoPilha, 4, 1, arquivoBin);
    fread(&proxRRN, 4, 1, arquivoBin);

    for(int i = 0; i < n; i++){     
    // Começa uma busca sequencial no arquivo a partir do RRN = 0 para cada uma das n buscas
        for(int RRN = 0; RRN < proxRRN; RRN++){

            if(check_registro(&(registros_de_busca[i]), mask_busca[i], RRN, arquivoBin)){
               
                if(atualiza_registro(&(campos_novos[i]), mask_novos[i], RRN, arquivoBin) == false){
                    DEBUG("ERRO EM func_6: FALHA AO ATUALIZAR REGISTRO.\n");
                }
            }
        }

        if (mask_busca[i] & 64){ free(registros_de_busca[i].nomeEstacao); registros_de_busca[i].nomeEstacao = NULL;}
        if (mask_busca[i] & 128){ free(registros_de_busca[i].nomeLinha); registros_de_busca[i].nomeEstacao = NULL;}
        if (mask_novos[i] & 64){ free(campos_novos[i].nomeEstacao); registros_de_busca[i].nomeEstacao = NULL;}
        if (mask_novos[i] & 128){ free(campos_novos[i].nomeLinha); registros_de_busca[i].nomeEstacao = NULL;}
    }

    // RETORNANDO

    free(registros_de_busca);
    free(campos_novos);
    free(mask_busca);
    free(mask_novos);

    return true;

    erro:

    free(registros_de_busca);
    free(campos_novos);
    free(mask_busca);
    free(mask_novos);

    return false;
}