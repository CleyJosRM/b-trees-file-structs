// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"

void func_9(char* arquivoBin, char* arquivoIndice, int n){
    FILE* fpDados = abre_binario(arquivoBin, true);
    if(fpDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    FILE* fpIndice = fopen(arquivoIndice, "rb+");
    if(fpIndice == NULL){
        printf("Falha no processamento do arquivo.\n");
        fecha_binario(fpDados);
        return;
    }

    char status;
    if(fread(&status, 1, 1, fpIndice) != 1 || status != '1'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpIndice);
        fecha_binario(fpDados);
        return;
    }

    int topo, proxRRN;
    fseek(fpDados, 1, SEEK_SET);
    if(fread(&topo, 4, 1, fpDados) != 1 || fread(&proxRRN, 4, 1, fpDados) != 1){
        printf("Falha no processamento do arquivo.\n");
        fclose(fpIndice);
        fecha_binario(fpDados);
        return;
    }

    for(int i = 0; i < n; i++){
        REG_DADOS_STRUCT* registro_lido = ler_input_reg();
        if(registro_lido == NULL){
            printf("Falha no processamento do arquivo.\n");
            continue;
        }

        int rrnInserido;

        if(topo != -1){
            rrnInserido = topo;
            long offset = (long)topo * REG_DADOS_S + HEADER_S;
            fseek(fpDados, offset + 1, SEEK_SET);
            int proximo_na_pilha;
            if(fread(&proximo_na_pilha, 4, 1, fpDados) != 1){
                printf("Falha no processamento do arquivo.\n");
                if(registro_lido->nomeEstacao) free(registro_lido->nomeEstacao);
                if(registro_lido->nomeLinha) free(registro_lido->nomeLinha);
                free(registro_lido);
                fclose(fpIndice);
                fecha_binario(fpDados);
                return;
            }
            topo = proximo_na_pilha;
            fseek(fpDados, offset, SEEK_SET);
        } else {
            rrnInserido = proxRRN;
            long offset = (long)proxRRN * REG_DADOS_S + HEADER_S;
            fseek(fpDados, offset, SEEK_SET);
            proxRRN++;
        }

        if(escreve_registro(registro_lido, fpDados) == false){
            printf("Falha no processamento do arquivo de dados em escreve.\n");
        }

        if(registro_lido->codEstacao != -1){
            inserir_entrada_na_arvore(fpIndice, registro_lido->codEstacao, rrnInserido);
        }

        if(registro_lido->nomeEstacao) free(registro_lido->nomeEstacao);
        if(registro_lido->nomeLinha) free(registro_lido->nomeLinha);
        free(registro_lido);
    }

    if(fecha_binario(fpDados) != 0){
        printf("Falha no processamento do arquivo de dados em fecha.\n");
        fclose(fpIndice);
        return;
    }

    atualizar_cabecalho(arquivoBin, topo, proxRRN);

    if(fecha_binario(fpIndice) != 0){
        printf("Falha no processamento do arquivo de índice em fecha indice.\n");
        return;
    }

    BinarioNaTela(arquivoBin);
    BinarioNaTela(arquivoIndice);
}