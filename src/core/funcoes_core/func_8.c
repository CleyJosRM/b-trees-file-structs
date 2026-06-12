// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

void func_8(char* arquivoBin, char* arquivoIndice, int n){
    
    FILE* fpDados = NULL;
    FILE* fpIndice = NULL;

    // Abrindo arquivos:

    fpDados = abre_binario(arquivoBin, false);
    if(fpDados == NULL){
        DEBUG("ERRO EM func_8: ERRO AO ABRIR O BINÁRIO DE DADOS. PODE ESTAR INCONSISTENTE.\n");
        goto erro;
    }

    fpIndice = abrir_indice(arquivoIndice, false); // como só iremos ler o índice, o status não precisa ser marcado como inconsistente
    if(fpIndice == NULL){
        DEBUG("ERRO EM func_8: ERRO AO ABRIR O ARQUIVO DE ÍNDICE. PODE ESTAR INCONSISTENTE.\n");
        goto erro;
    }

    // Lendo campos de busca n vezes e realizando a busca no binário de dados:

    int topoPilha, proxRRN;
    for(int i = 0; i < n; i++){

        REG_DADOS_STRUCT registro_busca;
        int mask;
        ler_campos(&registro_busca, &mask);

        bool flag_encontrou = false;

        if(mask & 1){
            int byteOffset = buscar_entrada(fpIndice, registro_busca.codEstacao);
            if (byteOffset != -1) {
                int rrn = (byteOffset - HEADER_S) / REG_DADOS_S;  // convert byte offset → RRN
                if (check_registro(&registro_busca, mask, rrn, fpDados)) {
                    fseek(fpDados, byteOffset, SEEK_SET);  // or: rrn * REG_DADOS_S + HEADER_S
                    print_registro(fpDados);
                    flag_encontrou = true;
                }
            }
        } else {
            fseek(fpDados, 1, SEEK_SET);
            if(fread(&topoPilha, 4, 1, fpDados) != 1 || fread(&proxRRN, 4, 1, fpDados) != 1){
                DEBUG("ERRO EM func_8: NÃO CONSEGUIU LER topo DA PILHA E proxRRN.\n");
                goto erro;
            }

            for(int RRN = 0; RRN < proxRRN; RRN++){
                if(check_registro(&registro_busca, mask, RRN, fpDados)){
                    fseek(fpDados, RRN * REG_DADOS_S + HEADER_S, SEEK_SET);
                    print_registro(fpDados);
                    flag_encontrou = true;
                }
            }
        }

        if(!flag_encontrou) printf("Registro inexistente.\n");
        printf("\n");

        if(mask & 64) free(registro_busca.nomeEstacao);
        if(mask & 128) free(registro_busca.nomeLinha);
    }

    // Fechando arquivos

    if(fecha_binario(fpDados) != 0){
        DEBUG("ERRO EM func_8: ERRO AO FECHAR BIN %s\n", arquivoBin);
        goto erro;
    }
    if(fechar_indice(fpIndice, false) == false){
        DEBUG("ERRO EM func_8: ERRO AO FECHAR INDICE %s\n", arquivoIndice);
        goto erro;
    }

    return;

    erro:

    printf("Falha no processamento do arquivo.\n");
    fecha_binario(fpDados);
    fechar_indice(fpIndice, false);
}
