// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

void func_8(char* arquivoBin, char* arquivoIndice, int n){
    FILE* fpDados = abre_binario(arquivoBin, false);
    if(fpDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE* fpIndice = abre_binario(arquivoIndice, false);
    if(fpIndice == NULL){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpDados);
        return;
    }

    char status;
    if(fread(&status, 1, 1, fpIndice) != 1 || status != '1'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpDados);
        fclose(fpIndice);
        return;
    }

    int topoPilha, proxRRN;
    for(int i = 0; i < n; i++){
        REG_DADOS_STRUCT registro_busca;
        int mask;
        ler_campos(&registro_busca, &mask);

        bool flag_encontrou = false;

        if(mask & 1){
            int byteOffset = buscar_chave(fpIndice, registro_busca.codEstacao);
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
                fclose(fpDados);
                fclose(fpIndice);
                printf("Falha no processamento do arquivo.\n");
                return;
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

    if(fecha_binario(fpDados) != 0){
        DEBUG("DEBUG: ERRO AO FECHAR BIN %s\n", arquivoBin);
        printf("Falha no processamento do arquivo.\n");
    }
    if(fecha_binario(fpIndice) != 0){
        DEBUG("DEBUG: ERRO AO FECHAR INDICE %s\n", arquivoIndice);
        printf("Falha no processamento do arquivo.\n");
    }
}
