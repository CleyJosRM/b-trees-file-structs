// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_4(FILE* arquivoBin, int n){

    REG_DADOS_STRUCT* registros_de_busca = NULL;
    int* mask = NULL;

    
    registros_de_busca = (REG_DADOS_STRUCT*)malloc(n*sizeof(REG_DADOS_STRUCT));   // Cria um vetor com os registros chaves para busca dos registros a serem excluídos, 
    mask = (int*)calloc(n, sizeof(int)); // e também um vetor com um bit mask dos campos utilizados para busca
    if(registros_de_busca == NULL || mask == NULL){DEBUG("ERRO EM func_3: ALOCAÇÃO DE MEMÓRIA.\n"); goto erro;}                           

    for(int i = 0; i < n; i++){  // lê as n entradas de argumentos para buscas
        ler_campos(&(registros_de_busca[i]), &(mask[i]));
    }

    int proxRRN, topoPilha;
    unsigned char removido = '1';

    fseek(arquivoBin, 1, SEEK_SET);
    fread(&topoPilha, 4, 1, arquivoBin);
    fread(&proxRRN, 4, 1, arquivoBin);

    for(int i = 0; i < n; i++){     // Começa uma busca sequencial no arquivo a partir do RRN = 0 para cada uma das n buscas

        for(int RRN = 0; RRN < proxRRN; RRN++){

            if(check_registro(&registros_de_busca[i], mask[i], RRN, arquivoBin)){
                // Se o registro bate com a busca, marca como removido e atualiza o campo 'proximo' para receber o topo da pilha
                fseek(arquivoBin, HEADER_S + RRN * REG_DADOS_S, SEEK_SET);
                fwrite(&removido, 1, 1, arquivoBin);
                fwrite(&topoPilha, 4, 1, arquivoBin);
                DEBUG("DEBUG: TOPO PILHA: %d\n", topoPilha);
                topoPilha = RRN; // atualiza o topo da pilha para o registro atual
            }
        }

        if (mask[i] & 64){ free(registros_de_busca[i].nomeEstacao); registros_de_busca[i].nomeEstacao = NULL;}
        if (mask[i] & 128){ free(registros_de_busca[i].nomeLinha); registros_de_busca[i].nomeEstacao = NULL;}
    }

    // ATUALIZANDO CABEÇALHO E FECHANDO ARQUIVO

    atualizar_cabecalho(arquivoBin, topoPilha, proxRRN);

    // LIMPANDO E RETORNANDO

    free(registros_de_busca);
    free(mask);

    return true;

    erro:

    free(registros_de_busca);
    free(mask);

    return false;
}