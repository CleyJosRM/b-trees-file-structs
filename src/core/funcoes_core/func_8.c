// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

bool func_8(FILE* arquivoBin, FILE* arquivoIndice, int n){
    
    REG_DADOS_STRUCT registro_busca = {0}; // inicializando com zero para liberar memória seguramente

    // Lendo campos de busca n vezes e realizando a busca no binário de dados:

    registro_busca = (REG_DADOS_STRUCT){0}; // inicializando com zero para liberar memória seguramente
    int mask = 0; // inicializando com zero para liberar memória seguramente
    int topoPilha, proxRRN;
    for(int i = 0; i < n; i++){

        ler_campos(&registro_busca, &mask);

        bool flag_encontrou = false;

        if(mask & 1){
            int byteOffset = buscar_entrada(arquivoIndice, registro_busca.codEstacao);
            if (byteOffset != -1) {
                int rrn = (byteOffset - HEADER_S) / REG_DADOS_S;  // convert byte offset → RRN
                if (check_registro(&registro_busca, mask, rrn, arquivoBin)) {
                    fseek(arquivoBin, byteOffset, SEEK_SET);  // or: rrn * REG_DADOS_S + HEADER_S
                    print_registro(arquivoBin);
                    flag_encontrou = true;
                }
            }
        } else {
            fseek(arquivoBin, 1, SEEK_SET);
            if(fread(&topoPilha, 4, 1, arquivoBin) != 1 || fread(&proxRRN, 4, 1, arquivoBin) != 1){
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

        if(mask & 64) free(registro_busca.nomeEstacao);
        registro_busca.nomeEstacao = NULL;
        if(mask & 128) free(registro_busca.nomeLinha);
        registro_busca.nomeLinha = NULL;
    }

    return true;

    erro:

    if(mask & 64) free(registro_busca.nomeEstacao);
    if(mask & 128) free(registro_busca.nomeLinha);

    return false;
}
