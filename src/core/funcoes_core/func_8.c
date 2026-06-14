// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_8(FILE* arquivoBin, FILE* arquivoIndice, int n){
    
    REG_DADOS_STRUCT registro_busca = {0}; // inicializando com zero para liberar memória seguramente
    registro_busca = (REG_DADOS_STRUCT){0};
    int mask = 0; 
    int topoPilha, proxRRN;

    // Lê campos de busca n vezes e realiza a busca no binário de dados
    for(int i = 0; i < n; i++){

        ler_campos(&registro_busca, &mask);

        bool flag_encontrou = false;

        if(mask & 1){  // Se o critério inclui codEstacao (bit 0 do mask)
            int byteOffset = buscar_entrada(arquivoIndice, registro_busca.codEstacao); // usa a árvore-B para obter o byteOffset do registro diretamente
            if (byteOffset != -1) {
                int rrn = (byteOffset - HEADER_S) / REG_DADOS_S;  // convert byte offset -> RRN
                if (check_registro(&registro_busca, mask, rrn, arquivoBin)) { // Verifica os demais campos com check_registro
                    fseek(arquivoBin, byteOffset, SEEK_SET);
                    print_registro(arquivoBin);
                    flag_encontrou = true;
                }
            }
        } else { // Senão, varre todo o arquivo de dados (varredura completa pois utiliza chave não única)
            fseek(arquivoBin, 1, SEEK_SET);
            if(fread(&topoPilha, 4, 1, arquivoBin) != 1 || fread(&proxRRN, 4, 1, arquivoBin) != 1){ // 
                DEBUG("ERRO EM func_8: NÃO CONSEGUIU LER topo DA PILHA E proxRRN.\n");
                goto erro;
            }

            for(int RRN = 0; RRN < proxRRN; RRN++){
                if(check_registro(&registro_busca, mask, RRN, arquivoBin)){
                    fseek(arquivoBin, RRN * REG_DADOS_S + HEADER_S, SEEK_SET);
                    print_registro(arquivoBin);
                    flag_encontrou = true;
                }
            }
        }

        if(!flag_encontrou) printf("Registro inexistente.\n");
        printf("\n");

        // Liberando memória alocada para strings do registro de busca, se necessário
        if(registro_busca.nomeEstacao != NULL) free(registro_busca.nomeEstacao);
        registro_busca.nomeEstacao = NULL;
        if(registro_busca.nomeLinha != NULL) free(registro_busca.nomeLinha);
        registro_busca.nomeLinha = NULL;
    }

    return true;

    erro:
    // limpa a memória em caso de erro se necessário
    if(registro_busca.nomeEstacao != NULL) free(registro_busca.nomeEstacao);
    if(registro_busca.nomeLinha != NULL) free(registro_busca.nomeLinha);

    return false;
}
